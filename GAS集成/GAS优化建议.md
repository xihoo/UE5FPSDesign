# GAS 初始化流程改进建议

## 1. 必须修复 (Critical)

### 1.1 显式设置 Replication Mode
**现状**: `UDFPSAbilitySystemComponent` 构造函数中仅设置了 `SetIsReplicated(true)`。
**问题**: 默认模式为 `Full`。对于 ASC 位于 `PlayerState` 的架构，这会导致 GameplayEffect 复制给所有客户端，浪费带宽且可能导致预测问题。
**建议**:
```cpp
// UDFPSAbilitySystemComponent.cpp
UDFPSAbilitySystemComponent::UDFPSAbilitySystemComponent(...)
{
    SetIsReplicated(true);
    // 关键修改：设置为 Mixed 模式
    SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
    PrimaryComponentTick.bCanEverTick = true;
}
```

## 2. 架构优化 (Architecture)

### 2.1 使用 GameplayEffect 初始化属性
**现状**: `ApplyInitAttributesAsGameplayEffect` 内部直接调用 `SetBaseValue`。
**问题**: 绕过了 GAS 管线，无法触发 `PostGameplayEffectExecute`，不符合 GAS 最佳实践。
**建议**:
使用 `Instant` GE + `Override` Modifier 进行初始化。
```cpp
void UDFPSAbilitySystemComponent::ApplyInitAttributesAsGameplayEffect(...)
{
    FGameplayEffectContextHandle Context = MakeEffectContext();
    Context.AddSourceObject(this);

    UGameplayEffect* InitEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("InitAttributes")));
    InitEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

    for (const auto& Pair : InitAttributes)
    {
        FGameplayModifierInfo ModInfo;
        ModInfo.Attribute = Pair.Key;
        ModInfo.ModifierOp = EGameplayModOp::Override;
        ModInfo.ModifierMagnitude = FScalableFloat(Pair.Value);
        InitEffect->Modifiers.Add(ModInfo);
    }

    ApplyGameplayEffectToSelf(InitEffect, 1.0f, Context);
}
```

### 2.2 解耦 AI 角色的 PlayerState 依赖
**现状**: `ADFPSCharacterBase` 强转 `PlayerState` 来获取 ASC。
**问题**: AI 角色通常没有 PlayerState，复用基类会导致 AI 无法初始化属性。
**建议**:
修改 `InitializeAttributesFromConfigIfPossible`，通过 `IAbilitySystemInterface::GetAbilitySystemComponent()` 获取 ASC，而不是硬编码 `GetPlayerState`。

## 3. 逻辑完善 (Logic Robustness)

### 3.1 角色切换与能力清理
**现状**: 仅通过 `bStartupAbilitiesGiven` 防止重复授予。
**问题**: 切换英雄（Character Class 变更）时，旧技能未移除，新技能可能无法授予。
**建议**:
在 ASC 中维护 `TArray<FGameplayAbilitySpecHandle> StartupAbilityHandles`，并提供 `ClearStartupAbilities()` 函数在切换角色时调用。

### 3.2 UnPossess 时的资源清理
**现状**: 无显式清理。
**问题**: 角色销毁后，ASC 的 AvatarActor 引用可能短暂空悬。
**建议**:
在 `UnPossessed` 事件中，如果 ASC 的 Avatar 是当前 Character，将其置为 nullptr。

## 4. 代码规范 (Code Style)

### 4.1 统一日志类别
**现状**: 使用 `LogTemp`。
**建议**: 定义 `DEFINE_LOG_CATEGORY_STATIC(LogDFPS_GAS, Log, All);`，便于过滤 GAS 相关日志。


```

// made by xihoo

#include "DFPSAttributeRegistry.h"

#include "DFPSAttributeSet.h"
#include "GameplayTagsManager.h"
#include "UObject/UnrealType.h"

TMap<FGameplayTag, FGameplayAttribute> UDFPSAttributeRegistry::TagToAttributeMap;
bool UDFPSAttributeRegistry::bIsInitialized = false;

void UDFPSAttributeRegistry::Initialize()
{
	if (bIsInitialized)
	{
		return;
	}

	bIsInitialized = true;
	RegisterAttributes();
}

void UDFPSAttributeRegistry::RegisterAttributes()
{
	TagToAttributeMap.Empty();

	// 1. 使用反射获取 AttributeSet 中所有 FGameplayAttributeData 类型的属性
	TMap<FString, FGameplayAttribute> NameToAttribute;
	
	for (TFieldIterator<FProperty> It(UDFPSAttributeSet::StaticClass()); It; ++It)
	{
		FStructProperty* StructProp = CastField<FStructProperty>(*It);
		if (StructProp && StructProp->Struct == FGameplayAttributeData::StaticStruct())
		{
			FGameplayAttribute Attribute(*It);
			NameToAttribute.Add(StructProp->GetName(), Attribute);
		}
	}

	// 2. 遍历所有 GameplayTag，通过名称后缀（Leaf Name）自动匹配属性
	// 约定：Tag "Attributes.Category.Health" 自动映射到属性 "Health"
	UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();
	FGameplayTagContainer AllTags;
	TagManager.RequestAllGameplayTags(AllTags, true);

	for (const FGameplayTag& Tag : AllTags)
	{
		FString TagName = Tag.ToString();
		
		// 过滤：只处理 Attributes 开头的 Tag，避免误匹配
		if (!TagName.StartsWith(TEXT("Attributes.")))
		{
			continue;
		}

		FString LeafName;
		int32 LastDotIndex;
		if (TagName.FindLastChar('.', LastDotIndex))
		{
			LeafName = TagName.RightChop(LastDotIndex + 1);
		}
		else
		{
			LeafName = TagName;
		}

		if (FGameplayAttribute* FoundAttr = NameToAttribute.Find(LeafName))
		{
			TagToAttributeMap.Add(Tag, *FoundAttr);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("DFPSAttributeRegistry: Auto-registered %d attributes via reflection."), TagToAttributeMap.Num());
}

FGameplayAttribute UDFPSAttributeRegistry::GetAttributeByTag(const FGameplayTag& AttributeTag)
{
	Initialize();

	if (const FGameplayAttribute* FoundAttr = TagToAttributeMap.Find(AttributeTag))
	{
		return *FoundAttr;
	}

	return FGameplayAttribute();
}

bool UDFPSAttributeRegistry::TryGetAttributeByTag(const FGameplayTag& AttributeTag, FGameplayAttribute& OutAttribute)
{
	Initialize();

	if (const FGameplayAttribute* FoundAttr = TagToAttributeMap.Find(AttributeTag))
	{
		OutAttribute = *FoundAttr;
		return true;
	}

	OutAttribute = FGameplayAttribute();
	return false;
}

bool UDFPSAttributeRegistry::IsValidAttributeTag(const FGameplayTag& AttributeTag)
{
	Initialize();
	return TagToAttributeMap.Contains(AttributeTag);
}

```


```

void UDFPSAbilitySystemComponent::ApplyInitAttributesAsGameplayEffect(const TArray<TPair<FGameplayAttribute, float>>& InitAttributes)
{
	if (InitAttributes.Num() == 0)
	{
		return;
	}

	// 必须在 Authority 端执行
	if (!GetOwnerActor() || !GetOwnerActor()->HasAuthority())
	{
		return;
	}

	// 动态创建一个 Instant GameplayEffect
	// 使用 Transient 包，确保它不会被保存到磁盘，只在运行时存在
	UGameplayEffect* InitEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("GE_DynamicInitAttributes")));
	if (!InitEffect)
	{
		UE_LOG(LogTemp, Error, TEXT("DFPS ASC: Failed to create dynamic init effect!"));
		return;
	}

	InitEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

	// 遍历属性列表，为每个属性添加一个 Modifier
	for (const TPair<FGameplayAttribute, float>& Pair : InitAttributes)
	{
		const FGameplayAttribute& Attribute = Pair.Key;
		const float Value = Pair.Value;

		if (!Attribute.IsValid())
		{
			continue;
		}

		FGameplayModifierInfo ModInfo;
		ModInfo.Attribute = Attribute;
		ModInfo.ModifierOp = EGameplayModOp::Override; // 使用 Override 直接覆盖基值
		ModInfo.ModifierMagnitude = FScalableFloat(Value);
		
		InitEffect->Modifiers.Add(ModInfo);
	}

	// 应用 GE
	FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	ApplyGameplayEffectToSelf(InitEffect, 1.0f, ContextHandle);
}

```