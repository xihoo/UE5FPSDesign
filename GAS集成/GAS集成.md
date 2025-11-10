# GAS集成设计文档

## 概述

本文档定义了Gameplay Ability System (GAS)在本项目中的集成方案。GAS是UE5提供的强大游戏机制框架，用于实现技能、buff、伤害计算、属性管理等复杂游戏逻辑。本项目所有游戏机制都基于GAS构建。

## 设计原则

1. **统一的游戏机制框架** - 所有技能、射击、伤害都通过GAS实现
2. **数据驱动的能力系统** - 能力和效果通过DataAsset配置
3. **网络复制优先** - 确保多人游戏中的状态同步
4. **性能优化** - 合理使用GAS特性，避免不必要的开销
5. **元素反应系统** - 实现类似原神的元素反应机制

## GAS核心概念

### Ability System Component (ASC)
- 每个参与GAS的Actor都需要一个ASC
- 管理Abilities、Effects和Attributes
- 处理GameplayTags

### Gameplay Ability
- 定义可执行的游戏行为（技能、射击等）
- 支持网络复制和预测
- 可以被打断、冷却、消耗资源

### Gameplay Effect
- 修改Attributes的机制
- 支持即时、持续和周期性效果
- 可堆叠、可被移除

### Gameplay Tags
- 用于标记和分类游戏内容
- 支持层级结构和查询
- 用于控制能力激活条件

### Attribute Set
- 存储角色属性（生命、护盾、伤害等）
- 自动网络复制
- 支持Pre/Post修改回调

---

## 核心架构

### 1. Ability System Component配置

```cpp
// 在AFPSCharacterBase中
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
UAbilitySystemComponent* AbilitySystemComponent;

// 服务器端初始化（PossessedBy）
void AFPSCharacterBase::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
        InitializeAbilitySystem();
    }
}

// 客户端初始化（OnRep_PlayerState）
void AFPSCharacterBase::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    
    if (AbilitySystemComponent && InputComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}
```

---

### 2. Attribute Set设计

#### UFPSAttributeSet - 基础属性集

**文件路径**：`Source/[ProjectName]/AbilitySystem/FPSAttributeSet.h`

```cpp
UCLASS()
class PROJECTNAME_API UFPSAttributeSet : public UAttributeSet
{
    GENERATED_BODY()
    
public:
    UFPSAttributeSet();
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
    
    // ========== 生命值 ==========
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, Health)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, MaxHealth)
    
    // ========== 护盾值 ==========
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Shield", ReplicatedUsing = OnRep_Shield)
    FGameplayAttributeData Shield;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, Shield)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Shield", ReplicatedUsing = OnRep_MaxShield)
    FGameplayAttributeData MaxShield;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, MaxShield)
    
    // ========== 移动属性 ==========
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Movement", ReplicatedUsing = OnRep_MoveSpeed)
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, MoveSpeed)
    
    // ========== 战斗属性 ==========
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_AttackPower)
    FGameplayAttributeData AttackPower;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, AttackPower)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_Defense)
    FGameplayAttributeData Defense;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, Defense)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_CriticalChance)
    FGameplayAttributeData CriticalChance;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, CriticalChance)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_CriticalDamage)
    FGameplayAttributeData CriticalDamage;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, CriticalDamage)
    
    // ========== 元素抗性 ==========
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Resistance", ReplicatedUsing = OnRep_FireResistance)
    FGameplayAttributeData FireResistance;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, FireResistance)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Resistance", ReplicatedUsing = OnRep_IceResistance)
    FGameplayAttributeData IceResistance;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, IceResistance)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Resistance", ReplicatedUsing = OnRep_ElectricResistance)
    FGameplayAttributeData ElectricResistance;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, ElectricResistance)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Resistance", ReplicatedUsing = OnRep_PoisonResistance)
    FGameplayAttributeData PoisonResistance;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, PoisonResistance)
    
    // ========== Meta属性（不复制，仅用于计算） ==========
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Meta")
    FGameplayAttributeData Damage; // 临时伤害值，用于计算
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, Damage)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Meta")
    FGameplayAttributeData Healing; // 临时治疗值
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, Healing)
    
protected:
    // RepNotify函数
    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldHealth);
    
    UFUNCTION()
    void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
    
    UFUNCTION()
    void OnRep_Shield(const FGameplayAttributeData& OldShield);
    
    UFUNCTION()
    void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield);
    
    UFUNCTION()
    void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);
    
    // ... 其他RepNotify函数 ...
    
    // 辅助函数
    void AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
                                      const FGameplayAttributeData& MaxAttribute,
                                      float NewMaxValue,
                                      const FGameplayAttribute& AffectedAttributeProperty);
};
```

---

### 3. Gameplay Tags体系

#### 标签结构

```
GameplayTags/
├── Ability/                        # 能力相关
│   ├── Ability.Skill              # 技能类能力
│   ├── Ability.Weapon             # 武器相关能力
│   │   ├── Ability.Weapon.Fire    # 射击
│   │   ├── Ability.Weapon.Reload  # 换弹
│   │   └── Ability.Weapon.Aim     # 瞄准
│   └── Ability.Interaction        # 交互能力
│
├── State/                         # 状态标签
│   ├── State.Dead                 # 死亡状态
│   ├── State.Stunned              # 眩晕状态
│   ├── State.Knockdown            # 击倒状态
│   ├── State.Reloading            # 换弹中
│   ├── State.Aiming               # 瞄准中
│   └── State.Sprinting            # 冲刺中
│
├── Element/                       # 元素类型
│   ├── Element.Fire               # 火
│   ├── Element.Ice                # 冰
│   ├── Element.Electric           # 雷
│   ├── Element.Poison             # 毒
│   └── Element.Physical           # 物理
│
├── ElementalStatus/               # 元素状态
│   ├── ElementalStatus.Burning    # 燃烧
│   ├── ElementalStatus.Frozen     # 冰冻
│   ├── ElementalStatus.Shocked    # 感电
│   └── ElementalStatus.Poisoned   # 中毒
│
├── Damage/                        # 伤害类型
│   ├── Damage.Basic               # 基础伤害
│   ├── Damage.Critical            # 暴击伤害
│   ├── Damage.Headshot            # 爆头伤害
│   └── Damage.Elemental           # 元素伤害
│
├── Buff/                          # Buff类型
│   ├── Buff.Positive              # 正面Buff
│   └── Buff.Negative              # 负面Buff
│
└── Event/                         # 事件标签
    ├── Event.Death                # 死亡事件
    ├── Event.Respawn              # 重生事件
    ├── Event.LevelUp              # 升级事件
    └── Event.ElementalReaction    # 元素反应事件
```

---

## 伤害系统设计

### 1. 伤害计算执行器

**文件路径**：`Source/[ProjectName]/AbilitySystem/Executions/FPSDamageExecution.h`

```cpp
UCLASS()
class PROJECTNAME_API UFPSDamageExecution : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()
    
public:
    UFPSDamageExecution();
    
    virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                        FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
```

**实现文件**：`FPSDamageExecution.cpp`

```cpp
UFPSDamageExecution::UFPSDamageExecution()
{
    // 定义需要捕获的属性
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        UFPSAttributeSet::GetDamageAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true));
    
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        UFPSAttributeSet::GetAttackPowerAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        false));
    
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        UFPSAttributeSet::GetDefenseAttribute(),
        EGameplayEffectAttributeCaptureSource::Target,
        false));
    
    // ... 添加更多需要捕获的属性
}

void UFPSDamageExecution::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    // 获取效果规格
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    
    // 获取标签
    FGameplayTagContainer SourceTags;
    Spec.GetAllAssetTags(SourceTags);
    
    // 获取目标ASC
    UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
    AActor* TargetActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
    
    // 获取源ASC
    UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    AActor* SourceActor = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    
    // ========== 1. 获取基础伤害 ==========
    
    float BaseDamage = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        FGameplayEffectAttributeCaptureDefinition(
            UFPSAttributeSet::GetDamageAttribute(),
            EGameplayEffectAttributeCaptureSource::Source,
            true),
        FAggregatorEvaluateParameters(),
        BaseDamage);
    
    // ========== 2. 应用攻击力加成 ==========
    
    float AttackPower = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        FGameplayEffectAttributeCaptureDefinition(
            UFPSAttributeSet::GetAttackPowerAttribute(),
            EGameplayEffectAttributeCaptureSource::Source,
            false),
        FAggregatorEvaluateParameters(),
        AttackPower);
    
    BaseDamage *= (1.0f + AttackPower / 100.0f);
    
    // ========== 3. 爆头判定 ==========
    
    bool bIsHeadshot = Spec.GetContext().GetHitResult() && 
                       IsHeadshot(Spec.GetContext().GetHitResult()->BoneName);
    
    if (bIsHeadshot)
    {
        BaseDamage *= 2.0f; // 爆头双倍伤害
        // 添加爆头标签
        OutExecutionOutput.AddOutputTag(FGameplayTag::RequestGameplayTag(FName("Damage.Headshot")));
    }
    
    // ========== 4. 暴击判定 ==========
    
    float CritChance = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        FGameplayEffectAttributeCaptureDefinition(
            UFPSAttributeSet::GetCriticalChanceAttribute(),
            EGameplayEffectAttributeCaptureSource::Source,
            false),
        FAggregatorEvaluateParameters(),
        CritChance);
    
    bool bIsCritical = FMath::RandRange(0.0f, 100.0f) < CritChance;
    
    if (bIsCritical)
    {
        float CritDamage = 0.0f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
            FGameplayEffectAttributeCaptureDefinition(
                UFPSAttributeSet::GetCriticalDamageAttribute(),
                EGameplayEffectAttributeCaptureSource::Source,
                false),
            FAggregatorEvaluateParameters(),
            CritDamage);
        
        BaseDamage *= (1.5f + CritDamage / 100.0f); // 基础暴击150%
        OutExecutionOutput.AddOutputTag(FGameplayTag::RequestGameplayTag(FName("Damage.Critical")));
    }
    
    // ========== 5. 应用防御减免 ==========
    
    float Defense = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        FGameplayEffectAttributeCaptureDefinition(
            UFPSAttributeSet::GetDefenseAttribute(),
            EGameplayEffectAttributeCaptureSource::Target,
            false),
        FAggregatorEvaluateParameters(),
        Defense);
    
    float DamageReduction = Defense / (Defense + 100.0f); // 防御递减公式
    BaseDamage *= (1.0f - DamageReduction);
    
    // ========== 6. 元素伤害和抗性 ==========
    
    float ElementalDamage = BaseDamage;
    
    // 检查元素类型
    if (SourceTags.HasTagExact(FGameplayTag::RequestGameplayTag(FName("Element.Fire"))))
    {
        float FireResistance = 0.0f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
            FGameplayEffectAttributeCaptureDefinition(
                UFPSAttributeSet::GetFireResistanceAttribute(),
                EGameplayEffectAttributeCaptureSource::Target,
                false),
            FAggregatorEvaluateParameters(),
            FireResistance);
        
        ElementalDamage *= (1.0f - FireResistance / 100.0f);
        
        // 应用燃烧状态
        ApplyElementalStatus(TargetASC, "ElementalStatus.Burning");
    }
    // ... 处理其他元素类型
    
    // ========== 7. 输出最终伤害 ==========
    
    if (ElementalDamage > 0.0f)
    {
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
            UFPSAttributeSet::GetHealthAttribute(),
            EGameplayModOp::Additive,
            -ElementalDamage));
    }
}

bool UFPSDamageExecution::IsHeadshot(const FName& BoneName) const
{
    // 检查是否命中头部骨骼
    return BoneName.ToString().Contains(TEXT("head"), ESearchCase::IgnoreCase);
}

void UFPSDamageExecution::ApplyElementalStatus(UAbilitySystemComponent* TargetASC, 
                                                 const FName& StatusTag) const
{
    if (!TargetASC) return;
    
    FGameplayTag Tag = FGameplayTag::RequestGameplayTag(StatusTag);
    if (Tag.IsValid())
    {
        TargetASC->AddLooseGameplayTag(Tag);
        // TODO: 应用对应的GameplayEffect
    }
}
```

---

## 元素反应系统

### 1. 元素状态设计

#### 元素类型
- **火（Fire）**：造成持续燃烧伤害
- **冰（Ice）**：减速并可能冻结目标
- **雷（Electric）**：造成连锁闪电伤害
- **毒（Poison）**：造成持续中毒伤害并降低治疗效果

### 2. 元素反应表

| 元素1 + 元素2 | 反应名称 | 效果 |
|--------------|---------|------|
| 火 + 冰 | 融化（Melt） | 造成额外伤害 |
| 火 + 雷 | 过载（Overload） | 范围爆炸伤害 |
| 火 + 毒 | 燃毒（Toxic Burn） | 持续高额毒伤 |
| 冰 + 雷 | 超导（Superconduct） | 降低防御 |
| 冰 + 毒 | 脆弱（Brittle） | 增加受到的伤害 |
| 雷 + 毒 | 腐蚀（Corrosion） | 护甲穿透 |

### 3. 元素反应实现

```cpp
UCLASS()
class PROJECTNAME_API UFPSElementalReactionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    // 检测并触发元素反应
    UFUNCTION(BlueprintCallable, Category = "FPS|Elemental")
    static void CheckElementalReaction(AActor* Target, FGameplayTag NewElement);
    
    // 应用元素反应效果
    UFUNCTION(BlueprintCallable, Category = "FPS|Elemental")
    static void ApplyElementalReaction(AActor* Target, 
                                        EElementalReaction Reaction,
                                        AActor* Instigator);
    
private:
    static EElementalReaction DetermineReaction(FGameplayTag Element1, FGameplayTag Element2);
};

// 元素反应枚举
UENUM(BlueprintType)
enum class EElementalReaction : uint8
{
    None            UMETA(DisplayName = "无"),
    Melt            UMETA(DisplayName = "融化"),
    Overload        UMETA(DisplayName = "过载"),
    ToxicBurn       UMETA(DisplayName = "燃毒"),
    Superconduct    UMETA(DisplayName = "超导"),
    Brittle         UMETA(DisplayName = "脆弱"),
    Corrosion       UMETA(DisplayName = "腐蚀")
};
```

---

## Gameplay Abilities设计

### 1. 射击能力

**文件路径**：`Source/[ProjectName]/AbilitySystem/Abilities/FPGA_Fire.h`

```cpp
UCLASS()
class PROJECTNAME_API UFPGA_Fire : public UGameplayAbility
{
    GENERATED_BODY()
    
public:
    UFPGA_Fire();
    
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                  const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo,
                                  const FGameplayEventData* TriggerEventData) override;
    
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
                           const FGameplayAbilityActorInfo* ActorInfo,
                           const FGameplayAbilityActivationInfo ActivationInfo,
                           bool bReplicateEndAbility,
                           bool bWasCancelled) override;
    
    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                    const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayTagContainer* SourceTags,
                                    const FGameplayTagContainer* TargetTags,
                                    FGameplayTagContainer* OptionalRelevantTags) const override;
    
protected:
    // 射击逻辑
    UFUNCTION(BlueprintCallable, Category = "FPS|Fire")
    void PerformFire();
    
    // 发射弹道
    UFUNCTION(BlueprintCallable, Category = "FPS|Fire")
    void FireProjectile(const FVector& Start, const FVector& Direction);
    
    // 伤害效果
    UPROPERTY(EditDefaultsOnly, Category = "Config")
    TSubclassOf<UGameplayEffect> DamageEffect;
    
    // 弹药消耗效果
    UPROPERTY(EditDefaultsOnly, Category = "Config")
    TSubclassOf<UGameplayEffect> AmmoCostEffect;
    
    // 后坐力
    UPROPERTY(EditDefaultsOnly, Category = "Config")
    FVector2D RecoilAmount;
    
    // 射速
    UPROPERTY(EditDefaultsOnly, Category = "Config")
    float FireRate;
    
    // 自动射击
    UPROPERTY(EditDefaultsOnly, Category = "Config")
    bool bFullAuto;
    
    // 计时器
    FTimerHandle FireTimerHandle;
};
```

---

### 2. 换弹能力

```cpp
UCLASS()
class PROJECTNAME_API UFPGA_Reload : public UGameplayAbility
{
    GENERATED_BODY()
    
public:
    UFPGA_Reload();
    
    virtual void ActivateAbility(...) override;
    
protected:
    UFUNCTION(BlueprintCallable)
    void StartReload();
    
    UFUNCTION(BlueprintCallable)
    void CompleteReload();
    
    // 换弹时间
    UPROPERTY(EditDefaultsOnly, Category = "Config")
    float ReloadDuration;
    
    // 换弹动画
    UPROPERTY(EditDefaultsOnly, Category = "Config")
    UAnimMontage* ReloadMontage;
    
    // 弹药恢复效果
    UPROPERTY(EditDefaultsOnly, Category = "Config")
    TSubclassOf<UGameplayEffect> AmmoRestoreEffect;
};
```

---

### 3. 技能能力基类

```cpp
UCLASS()
class PROJECTNAME_API UFPGA_SkillBase : public UGameplayAbility
{
    GENERATED_BODY()
    
public:
    UFPGA_SkillBase();
    
protected:
    // 冷却时间
    UPROPERTY(EditDefaultsOnly, Category = "Skill")
    float Cooldown;
    
    // 消耗资源
    UPROPERTY(EditDefaultsOnly, Category = "Skill")
    float ResourceCost;
    
    // 技能等级
    UPROPERTY(BlueprintReadOnly, Category = "Skill")
    int32 SkillLevel;
    
    // 技能效果（根据等级缩放）
    UPROPERTY(EditDefaultsOnly, Category = "Skill")
    TSubclassOf<UGameplayEffect> SkillEffect;
};
```

---

## Gameplay Effects示例

### 1. 伤害效果

```cpp
// 在C++中配置，或通过蓝图DataAsset创建

FGameplayEffectSpec* DamageSpec = new FGameplayEffectSpec();
DamageSpec->Def = DamageEffectClass;
DamageSpec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Damage"), DamageValue);
DamageSpec->Duration = EGameplayEffectDurationType::Instant;

// 应用效果
TargetASC->ApplyGameplayEffectSpecToSelf(*DamageSpec);
```

### 2. 燃烧DoT效果

```
Effect Name: GE_Burning
Duration: 5秒
Period: 1秒（每秒触发一次）
Modifiers:
  - Attribute: Health
  - Operation: Add
  - Magnitude: -10（每秒10点伤害）
  - Scaling: None
Tags:
  - ElementalStatus.Burning
  - Buff.Negative
```

### 3. 移速Buff效果

```
Effect Name: GE_SpeedBoost
Duration: 10秒
Modifiers:
  - Attribute: MoveSpeed
  - Operation: Multiply
  - Magnitude: 1.5（150%移速）
Tags:
  - Buff.Positive
```

---

## 网络复制策略

### 1. Ability激活复制

```cpp
// 在Ability中设置
NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
```

### 2. 预测支持

```cpp
// 客户端预测的Ability
NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

// 在Ability中实现预测逻辑
virtual void PreActivate(...) override;
virtual void OnPreActivate(...);
```

### 3. 属性复制

```cpp
// 在AttributeSet中
void UFPSAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UFPSAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UFPSAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UFPSAttributeSet, Shield, COND_None, REPNOTIFY_Always);
    // ... 其他属性
}
```

---

## 性能优化建议

### 1. 合理使用Ability实例化策略
- **NonInstanced**：无状态能力，如被动
- **InstancedPerActor**：有状态但不需要多个实例
- **InstancedPerExecution**：每次激活创建新实例

### 2. 减少不必要的复制
- 使用COND_OwnerOnly减少非必要复制
- 对不重要的属性降低更新频率

### 3. 优化GameplayEffect
- 合并多个小Effect为一个大Effect
- 使用Instant而非Duration = 0.01f

### 4. GameplayTag缓存
```cpp
// 缓存常用Tag，避免每次查询
static const FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag("State.Dead");
```

---

## 调试工具

### 1. GAS调试命令

```
showdebug abilities  // 显示ASC调试信息
AbilitySystem.Debug.Enabled 1  // 启用GAS调试
```

### 2. 自定义调试

```cpp
#if !UE_BUILD_SHIPPING
void UFPSAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    UE_LOG(LogGAS, Log, TEXT("Attribute %s changed from %.2f to %.2f"),
           *Data.EvaluatedData.Attribute.GetName(),
           Data.EvaluatedData.Attribute.GetNumericValue(this) - Data.EvaluatedData.Magnitude,
           Data.EvaluatedData.Attribute.GetNumericValue(this));
}
#endif
```

---

## 相关文档

- [基类设计文档](../基类/基类.md)
- [射击框架文档](../射击框架/射击框架.md)
- [技能系统文档](../技能/技能.md)
- [库存系统文档](../库存/库存.md)
