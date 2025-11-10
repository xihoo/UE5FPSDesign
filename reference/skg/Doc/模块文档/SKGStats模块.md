# SKGStats 模块文档

## 模块概述

SKGStats 模块提供了完整的武器统计数据管理系统，支持武器和附件的属性定义、统计计算、数据驱动配置等功能。该模块是SKGShooterFramework的核心数据层，为所有武器相关的数值计算提供基础数据支持，包括重量、精度、后坐力、射速等关键属性。

## 核心组件

### USKGFirearmAttachmentStatComponent

**类路径**: `Source/SKGStats/Public/Components/SKGFirearmAttachmentStatComponent.h`

这是附件统计数据组件，用于为武器附件提供统计数据接口，通常与附件系统配合使用。

#### 主要功能
- **统计数据引用**: 提供对附件统计数据资产的访问
- **组件发现**: 支持通过组件方式获取附件统计
- **数据接口**: 为其他系统提供统计数据访问接口
- **标签支持**: 支持GameplayTag系统

#### 核心属性

```cpp
// 附件统计数据资产
UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetAttachmentStats, Category = "SKGFirearmAttachmentStatComponent|Initialize")
TObjectPtr<USKGPDAFirearmAttachmentStats> AttachmentStatsDataAsset;

// 游戏标签
UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFirearmAttachmentStatComponent|Initialize")
FGameplayTagContainer GameplayTags;
```

#### 主要方法

```cpp
// 获取附件统计数据
UFUNCTION(BlueprintGetter)
USKGPDAFirearmAttachmentStats* GetAttachmentStats() const { return AttachmentStatsDataAsset; }

// 静态方法获取组件
UFUNCTION(BlueprintPure, Category = "SKGShooterFrameworkStatics|Getters")
static USKGFirearmAttachmentStatComponent* GetFirearmAttachmentStatComponent(const AActor* Actor);

// 网络权限检查
FORCEINLINE bool HasAuthority() const { return GetOwnerRole() == ROLE_Authority; }
```

## 数据资产系统

### USKGPDAFirearmStats

**类路径**: `Source/SKGStats/Public/DataAssets/SKGPDAFirearmStats.h`

这是火器基础统计数据资产，定义了武器的基本属性。

#### 核心属性

```cpp
UCLASS()
class SKGSTATS_API USKGPDAFirearmStats : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    // 重量（千克或其他单位）
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SKGFirearmStats")
    float Weight {0.0f};
    
    // 耐久度（百分比，100为满耐久）
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SKGFirearmStats")
    float Durability {100.0f};
    
    // 人机工程学（影响操控性）
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SKGFirearmStats")
    float Ergonomics {50.0f};
    
    // 精度（MOA - Minute of Angle）
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SKGFirearmStats")
    float Accuracy {2.0f};
    
    // 垂直后坐力（百分比）
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SKGFirearmStats")
    float VerticalRecoil {100.0f};
    
    // 水平后坐力（百分比）
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SKGFirearmStats")
    float HorizontalRecoil {100.0f};
    
    // 射速（发/分钟）
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SKGFirearmStats")
    float FireRate {600.0f};
};
```

### USKGPDAFirearmAttachmentStats

**类路径**: `Source/SKGStats/Public/DataAssets/SKGPDAFirearmAttachmentStats.h`

这是附件统计数据资产，定义了武器附件对武器属性的影响。

#### 核心属性

```cpp
UCLASS()
class SKGSTATS_API USKGPDAFirearmAttachmentStats : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    // 重量影响
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SKGFirearmAttachmentStats")
    float Weight {0.0f};
    
    // 人机工程学影响
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SKGFirearmAttachmentStats")
    float Ergonomics {0.0f};
    
    // 精度影响（MOA）
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SKGFirearmAttachmentStats")
    float Accuracy {0.0f};
    
    // 垂直后坐力影响
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SKGFirearmAttachmentStats")
    float VerticalRecoil {0.0f};
    
    // 水平后坐力影响
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SKGFirearmAttachmentStats")
    float HorizontalRecoil {0.0f};
    
    // 初速影响百分比（如枪管长度影响）
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SKGFirearmAttachmentStats")
    float MuzzleVelocityPercentage {0.0f};
    
    // 射速变化百分比
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SKGFirearmAttachmentStats")
    float FireRateChangePercent {0.0f};
};
```

## 使用方法

### 基础数据配置

1. **创建数据资产**: 创建火器和附件的统计数据资产
2. **配置属性值**: 设置各种武器属性的基础值
3. **关联组件**: 将数据资产关联到相应的组件
4. **使用数据**: 在其他系统中使用这些统计数据

### 火器基础统计配置

```cpp
// 创建火器统计数据资产
USKGPDAFirearmStats* RifleStats = NewObject<USKGPDAFirearmStats>();
RifleStats->Weight = 3.5f;                    // 3.5kg重量
RifleStats->Durability = 85.0f;               // 85%耐久度
RifleStats->Ergonomics = 65.0f;               // 65人机工程学
RifleStats->Accuracy = 2.0f;                  // 2 MOA精度
RifleStats->VerticalRecoil = 100.0f;          // 100%垂直后坐力（基准）
RifleStats->HorizontalRecoil = 80.0f;         // 80%水平后坐力
RifleStats->FireRate = 600.0f;                // 600发/分钟射速
```

### 附件统计配置

```cpp
// 创建附件统计数据资产
USKGPDAFirearmAttachmentStats* HeavyBarrelStats = NewObject<USKGPDAFirearmAttachmentStats>();
HeavyBarrelStats->Weight = 0.5f;              // 增加0.5kg重量
HeavyBarrelStats->Ergonomics = -5.0f;         // 降低5点人机工程学
HeavyBarrelStats->Accuracy = -0.5f;           // 改善0.5 MOA精度
HeavyBarrelStats->VerticalRecoil = -15.0f;    // 减少15%垂直后坐力
HeavyBarrelStats->HorizontalRecoil = -10.0f;  // 减少10%水平后坐力
HeavyBarrelStats->MuzzleVelocityPercentage = 5.0f;  // 增加5%初速
HeavyBarrelStats->FireRateChangePercent = 0.0f;     // 不影响射速
```

### 组件使用示例

```cpp
// 在火器组件中使用统计数据
void USKGFirearmComponent::InitializeFirearmComponent()
{
    // 获取基础统计数据
    if (FirearmStatsDataAsset)
    {
        FirearmStats.Weight = FirearmStatsDataAsset->Weight;
        FirearmStats.Durability = FirearmStatsDataAsset->Durability;
        FirearmStats.Ergonomics = FirearmStatsDataAsset->Ergonomics;
        FirearmStats.Accuracy = FirearmStatsDataAsset->Accuracy;
        FirearmStats.VerticalRecoil = FirearmStatsDataAsset->VerticalRecoil;
        FirearmStats.HorizontalRecoil = FirearmStatsDataAsset->HorizontalRecoil;
        FirearmStats.FireRate = FirearmStatsDataAsset->FireRate;
    }
}

// 在附件组件中使用统计数据
void USKGFirearmComponent::AddFirearmAttachmentStats(USKGFirearmAttachmentStatComponent* StatComponent)
{
    if (StatComponent && StatComponent->GetAttachmentStats())
    {
        const USKGPDAFirearmAttachmentStats* AttachmentStats = StatComponent->GetAttachmentStats();
        
        // 应用附件统计到总统计
        FirearmStats.Weight += AttachmentStats->Weight;
        FirearmStats.Ergonomics += AttachmentStats->Ergonomics;
        FirearmStats.Accuracy += AttachmentStats->Accuracy;
        FirearmStats.VerticalRecoil += AttachmentStats->VerticalRecoil;
        FirearmStats.HorizontalRecoil += AttachmentStats->HorizontalRecoil;
        
        // 触发统计更新事件
        OnFirearmStatsChanged.Broadcast(FirearmStats);
    }
}
```

## 高级功能

### 统计计算系统

在火器组件中实现复杂的统计计算：

```cpp
void CalculateProceduralValues()
{
    // 基础统计数据
    FSKGProceduralStats Stats;
    
    // 根据重量影响移动速度
    float WeightFactor = FMath::Clamp(FirearmStats.Weight / 5.0f, 0.5f, 1.5f);
    Stats.MovementSwayMultiplier *= WeightFactor;
    
    // 根据人机工程学影响瞄准速度
    float ErgonomicsFactor = FMath::Clamp(FirearmStats.Ergonomics / 50.0f, 0.7f, 1.3f);
    Stats.AimInterpolationRate *= ErgonomicsFactor;
    
    // 根据精度影响后坐力
    float AccuracyFactor = FMath::Clamp(FirearmStats.Accuracy / 2.0f, 0.8f, 1.2f);
    Stats.ControlRotationRecoilMultipliers *= AccuracyFactor;
    
    // 根据后坐力影响动画强度
    Stats.RecoilLocationMultipliers.X *= FirearmStats.VerticalRecoil / 100.0f;
    Stats.RecoilRotationMultipliers.Y *= FirearmStats.HorizontalRecoil / 100.0f;
    
    SetProceduralStats(Stats);
}
```

### 动态统计修改

支持运行时的统计动态修改：

```cpp
// 武器磨损影响
void ApplyWearAnd Tear(float WearAmount)
{
    // 减少耐久度
    FirearmStats.Durability = FMath::Max(0.0f, FirearmStats.Durability - WearAmount);
    
    // 磨损影响精度
    float WearFactor = 1.0f - (WearAmount * 0.01f);
    FirearmStats.Accuracy *= (2.0f - WearFactor);  // 精度变差
    
    // 磨损影响可靠性
    FirearmStats.Ergonomics *= WearFactor;
    
    // 触发统计更新
    OnFirearmStatsChanged.Broadcast(FirearmStats);
}

// 环境因素影响
void ApplyEnvironmentalFactors(float Temperature, float Humidity)
{
    // 温度影响
    if (Temperature < 0.0f)
    {
        // 低温影响操作
        FirearmStats.Ergonomics *= 0.9f;
    }
    else if (Temperature > 40.0f)
    {
        // 高温影响精度
        FirearmStats.Accuracy *= 1.1f;
    }
    
    // 湿度影响
    if (Humidity > 80.0f)
    {
        // 高湿度影响可靠性
        FirearmStats.Durability *= 0.95f;
    }
}
```

### 统计可视化

```cpp
// UI显示统计数据
FString GetStatsDisplayString() const
{
    FString DisplayString;
    
    DisplayString += FString::Printf(TEXT("重量: %.1f kg\n"), FirearmStats.Weight);
    DisplayString += FString::Printf(TEXT("耐久度: %.0f%%\n"), FirearmStats.Durability);
    DisplayString += FString::Printf(TEXT("人机工程学: %.0f\n"), FirearmStats.Ergonomics);
    DisplayString += FString::Printf(TEXT("精度: %.1f MOA\n"), FirearmStats.Accuracy);
    DisplayString += FString::Printf(TEXT("垂直后坐力: %.0f%%\n"), FirearmStats.VerticalRecoil);
    DisplayString += FString::Printf(TEXT("水平后坐力: %.0f%%\n"), FirearmStats.HorizontalRecoil);
    DisplayString += FString::Printf(TEXT("射速: %.0f RPM\n"), FirearmStats.FireRate);
    
    return DisplayString;
}
```

## 集成应用

### 与附件系统集成

```cpp
// 附件添加时的统计处理
void OnAttachmentAdded(AActor* Attachment)
{
    USKGFirearmAttachmentStatComponent* StatComponent = Attachment->FindComponentByClass<USKGFirearmAttachmentStatComponent>();
    if (StatComponent)
    {
        AddFirearmAttachmentStats(StatComponent);
        
        // 更新UI显示
        UpdateStatsDisplay();
        
        // 重新计算程序化数值
        CalculateProceduralValues();
    }
}

// 附件移除时的统计处理
void OnAttachmentRemoved(AActor* Attachment)
{
    USKGFirearmAttachmentStatComponent* StatComponent = Attachment->FindComponentByClass<USKGFirearmAttachmentStatComponent>();
    if (StatComponent)
    {
        RemoveFirearmAttachmentStats(StatComponent);
        
        // 更新UI显示
        UpdateStatsDisplay();
        
        // 重新计算程序化数值
        CalculateProceduralValues();
    }
}
```

### 与游戏平衡系统集成

```cpp
// 游戏难度影响
void ApplyDifficultySettings(EDifficultyLevel Difficulty)
{
    switch (Difficulty)
    {
        case EDifficultyLevel::Easy:
            // 简单模式：降低后坐力，提高精度
            FirearmStats.VerticalRecoil *= 0.8f;
            FirearmStats.HorizontalRecoil *= 0.8f;
            FirearmStats.Accuracy *= 0.8f;  // 更好的精度
            break;
            
        case EDifficultyLevel::Hard:
            // 困难模式：增加后坐力，降低精度
            FirearmStats.VerticalRecoil *= 1.2f;
            FirearmStats.HorizontalRecoil *= 1.2f;
            FirearmStats.Accuracy *= 1.2f;  // 更差的精度
            break;
    }
    
    OnFirearmStatsChanged.Broadcast(FirearmStats);
}
```

### 与技能系统集成

```cpp
// 玩家技能影响
void ApplySkillBonuses(float WeaponHandlingSkill, float MarksmanshipSkill)
{
    // 武器处理技能影响人机工程学
    float HandlingBonus = FMath::Lerp(0.8f, 1.2f, WeaponHandlingSkill / 100.0f);
    FirearmStats.Ergonomics *= HandlingBonus;
    
    // 射击技能影响精度
    float MarksmanshipBonus = FMath::Lerp(1.2f, 0.8f, MarksmanshipSkill / 100.0f);
    FirearmStats.Accuracy *= MarksmanshipBonus;
    
    OnFirearmStatsChanged.Broadcast(FirearmStats);
}
```

## 技术细节

### 数据结构设计

统计数据采用结构体形式，便于批量操作：

```cpp
USTRUCT(BlueprintType)
struct FSKGFirearmStats
{
    GENERATED_BODY()
    
    // 基础属性
    float Weight;
    float Durability;
    float Ergonomics;
    float Accuracy;
    
    // 后坐力属性
    float VerticalRecoil;
    float HorizontalRecoil;
    
    // 性能属性
    float FireRate;
    
    // 运算符重载，便于统计计算
    FSKGFirearmStats& operator+=(const USKGFirearmAttachmentStatComponent* StatComponent);
    FSKGFirearmStats& operator-=(const USKGFirearmAttachmentStatComponent* StatComponent);
};
```

### 数据验证

```cpp
void ValidateStats()
{
    // 确保数值在合理范围内
    FirearmStats.Weight = FMath::Max(0.0f, FirearmStats.Weight);
    FirearmStats.Durability = FMath::Clamp(FirearmStats.Durability, 0.0f, 100.0f);
    FirearmStats.Ergonomics = FMath::Max(0.0f, FirearmStats.Ergonomics);
    FirearmStats.Accuracy = FMath::Max(0.1f, FirearmStats.Accuracy);  // 最小精度限制
    FirearmStats.VerticalRecoil = FMath::Max(0.0f, FirearmStats.VerticalRecoil);
    FirearmStats.HorizontalRecoil = FMath::Max(0.0f, FirearmStats.HorizontalRecoil);
    FirearmStats.FireRate = FMath::Max(1.0f, FirearmStats.FireRate);  // 最小射速限制
}
```

### 性能优化

```cpp
// 缓存统计数据引用，避免重复查找
TWeakObjectPtr<USKGPDAFirearmStats> CachedFirearmStats;
TWeakObjectPtr<USKGPDAFirearmAttachmentStats> CachedAttachmentStats;

// 批量更新机制
void BatchUpdateStats(const TArray<USKGFirearmAttachmentStatComponent*>& AttachmentStats)
{
    // 重置为基准值
    ResetToBaseStats();
    
    // 批量应用所有附件统计
    for (USKGFirearmAttachmentStatComponent* StatComponent : AttachmentStats)
    {
        if (StatComponent && StatComponent->GetAttachmentStats())
        {
            ApplyAttachmentStats(StatComponent->GetAttachmentStats());
        }
    }
    
    // 一次性触发更新事件
    OnFirearmStatsChanged.Broadcast(FirearmStats);
}
```

## 常见问题

### Q: 统计数据不生效？
A: 检查数据资产是否正确关联到组件，确认属性值设置合理

### Q: 附件统计叠加错误？
A: 确保正确实现了运算符重载，检查统计计算逻辑

### Q: 性能问题？
A: 使用批量更新机制，避免频繁的单独统计更新

### Q: 数据同步问题？
A: 确保在网络环境下正确同步统计数据变化

### Q: 统计验证失败？
A: 检查数值是否在合理范围内，确认验证逻辑正确

## 最佳实践

1. **数据一致性**: 确保基础数据和附件数据的单位一致
2. **范围验证**: 始终验证统计数据在合理范围内
3. **性能考虑**: 使用批量更新和缓存机制优化性能
4. **可扩展性**: 设计易于扩展的统计结构
5. **平衡性**: 仔细调整各项属性的数值平衡

## 相关模块

- [SKGShooterFramework 模块](SKGShooterFramework模块.md) - 火器核心系统
- [SKGAttachment 模块](SKGAttachment模块.md) - 附件管理系统
- [SKGProceduralAnim 模块](SKGProceduralAnim模块.md) - 程序化动画系统