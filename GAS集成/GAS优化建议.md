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
