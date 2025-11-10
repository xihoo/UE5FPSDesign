# SKGOptional 模块文档

## 模块概述

SKGOptional 模块提供了可选的框架功能实现，包括AI感知接口、角色基础类等非核心但实用的功能。该模块包含了一些可选的增强功能，开发者可以根据需要选择使用，为射击游戏提供额外的功能支持。

## 核心组件

### ASKGCharacter

**类路径**: `Source/SKGOptional/Public/Actors/SKGCharacter.h`

这是框架提供的可选角色类，扩展了标准的ACharacter类，添加了AI感知接口支持。

#### 核心功能
- **AI感知接口**: 实现IAISightTargetInterface，支持AI视觉感知
- **骨骼检查**: 提供特定的骨骼检查点，用于AI视线检测
- **基础角色功能**: 继承所有标准角色功能

#### 核心属性

```cpp
// 需要检查的骨骼名称数组（用于AI视线检测）
UPROPERTY(EditDefaultsOnly, Category = "SKG|AI")
TArray<FName> BonesToCheck {"head", "spine_03", "lowerarm_l", "hand_l", "lowerarm_r", "hand_r", "thigh_l", "calf_l", "thigh_r", "calf_r" };
```

#### 主要方法

```cpp
/**
 * AI感知接口实现 - 检查角色是否可见
 * @param Context 感知上下文
 * @param OutSeenLocation 输出可见位置
 * @param OutNumberOfLoSChecksPerformed 输出视线检查次数
 * @param OutNumberOfAsyncLosCheckRequested 输出异步检查请求数
 * @param OutSightStrength 输出视觉强度
 * @param UserData 用户数据
 * @param Delegate 委托回调
 * @return 可见性结果
 */
virtual UAISense_Sight::EVisibilityResult CanBeSeenFrom(
    const FCanBeSeenFromContext& Context, 
    FVector& OutSeenLocation, 
    int32& OutNumberOfLoSChecksPerformed, 
    int32& OutNumberOfAsyncLosCheckRequested, 
    float& OutSightStrength, 
    int32* UserData, 
    const FOnPendingVisibilityQueryProcessedDelegate* Delegate
) override;
```

## 使用方法

### 基础角色创建

1. **继承角色类**: 从ASKGCharacter继承创建自定义角色
2. **配置AI感知**: 设置合适的骨骼检查点
3. **集成AI系统**: 与Unreal Engine的AI感知系统配合使用
4. **自定义行为**: 添加角色特定的逻辑

### 角色类实现示例

```cpp
// 创建自定义角色类
UCLASS()
class MYGAME_API AMyCharacter : public ASKGCharacter
{
    GENERATED_BODY()
    
public:
    AMyCharacter();
    
protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
private:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void LookUp(float Value);
    void Turn(float Value);
};

// 构造函数
AMyCharacter::AMyCharacter()
{
    // 设置默认属性
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    
    // 配置AI感知骨骼检查点
    BonesToCheck = {
        "head",
        "spine_03", 
        "lowerarm_l",
        "hand_l",
        "lowerarm_r", 
        "hand_r",
        "thigh_l",
        "calf_l",
        "thigh_r",
        "calf_r"
    };
}
```

### AI感知集成

```cpp
// 在AI控制器中使用角色感知
void AMyAIController::SetupPerception()
{
    // 创建视觉感知组件
    UAIPerceptionComponent* PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    
    // 配置视觉感知
    UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 3000.0f;
    SightConfig->LoseSightRadius = 3500.0f;
    SightConfig->PeripheralVisionAngleDegrees = 90.0f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    
    PerceptionComponent->ConfigureSense(*SightConfig);
    PerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());
    
    // 绑定感知更新事件
    PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AMyAIController::OnPerceptionUpdated);
}

// 处理感知更新
void AMyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    for (AActor* Actor : UpdatedActors)
    {
        if (ASKGCharacter* Character = Cast<ASKGCharacter>(Actor))
        {
            // 处理角色感知
            HandleCharacterPerception(Character);
        }
    }
}
```

### 自定义骨骼检查

```cpp
// 重写骨骼检查逻辑
virtual UAISense_Sight::EVisibilityResult CanBeSeenFrom(
    const FCanBeSeenFromContext& Context, 
    FVector& OutSeenLocation, 
    int32& OutNumberOfLoSChecksPerformed, 
    int32& OutNumberOfAsyncLosCheckRequested, 
    float& OutSightStrength, 
    int32* UserData, 
    const FOnPendingVisibilityQueryProcessedDelegate* Delegate
) override
{
    // 调用父类实现
    UAISense_Sight::EVisibilityResult Result = Super::CanBeSeenFrom(
        Context, OutSeenLocation, OutNumberOfLoSChecksPerformed, 
        OutNumberOfAsyncLosCheckRequested, OutSightStrength, UserData, Delegate
    );
    
    // 添加自定义逻辑
    if (Result == UAISense_Sight::EVisibilityResult::Visible)
    {
        // 检查是否在掩体后
        if (IsBehindCover(Context.ObserverLocation))
        {
            OutSightStrength *= 0.5f;  // 降低可见度
        }
    }
    
    return Result;
}
```

## 高级功能

### 自定义AI行为

```cpp
// 创建自定义AI控制器
UCLASS()
class MYGAME_API AMyAIController : public AAIController
{
    GENERATED_BODY()
    
public:
    AMyAIController();
    
protected:
    virtual void BeginPlay() override;
    
private:
    void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);
    void HandleCharacterSighted(ASKGCharacter* Character);
    void HandleCharacterLost(ASKGCharacter* Character);
    
    UPROPERTY()
    TArray<ASKGCharacter*> VisibleCharacters;
};

// AI行为树集成
void AMyAIController::HandleCharacterSighted(ASKGCharacter* Character)
{
    // 检查角色是否敌对
    if (IsHostile(Character))
    {
        // 设置攻击目标
        SetFocus(Character);
        
        // 切换到战斗状态
        UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
        if (BlackboardComp)
        {
            BlackboardComp->SetValueAsObject("EnemyTarget", Character);
            BlackboardComp->SetValueAsBool("HasEnemy", true);
        }
    }
}
```

### 潜行系统集成

```cpp
// 潜行状态管理
UENUM(BlueprintType)
enum class EStealthState : uint8
{
    Hidden,
    Exposed,
    Compromised
};

// 在角色中添加潜行功能
UCLASS()
class MYGAME_API AMyCharacter : public ASKGCharacter
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable, Category = "Stealth")
    EStealthState GetStealthState() const { return StealthState; }
    
    UFUNCTION(BlueprintCallable, Category = "Stealth")
    void UpdateStealthState(float NoiseLevel, float LightLevel);
    
private:
    UPROPERTY(VisibleAnywhere, Category = "Stealth")
    EStealthState StealthState;
    
    UPROPERTY(EditDefaultsOnly, Category = "Stealth")
    float VisibilityThreshold;
    
    UPROPERTY(EditDefaultsOnly, Category = "Stealth")
    float NoiseThreshold;
};

// 实现潜行逻辑
void AMyCharacter::UpdateStealthState(float NoiseLevel, float LightLevel)
{
    // 基于噪音和光照更新潜行状态
    if (NoiseLevel > NoiseThreshold || LightLevel > VisibilityThreshold)
    {
        StealthState = EStealthState::Exposed;
    }
    else if (NoiseLevel > NoiseThreshold * 0.5f)
    {
        StealthState = EStealthState::Compromised;
    }
    else
    {
        StealthState = EStealthState::Hidden;
    }
}
```

### 团队识别系统

```cpp
// 团队标识
UENUM(BlueprintType)
enum class ETeam : uint8
{
    Neutral,
    TeamA,
    TeamB
};

// 扩展角色类以支持团队
UCLASS()
class MYGAME_API AMyCharacter : public ASKGCharacter
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable, Category = "Team")
    ETeam GetTeam() const { return Team; }
    
    UFUNCTION(BlueprintCallable, Category = "Team")
    void SetTeam(ETeam NewTeam) { Team = NewTeam; }
    
    UFUNCTION(BlueprintCallable, Category = "Team")
    bool IsHostileTo(ASKGCharacter* OtherCharacter) const;
    
private:
    UPROPERTY(EditAnywhere, Category = "Team")
    ETeam Team;
};

// 实现敌对关系判断
bool AMyCharacter::IsHostileTo(ASKGCharacter* OtherCharacter) const
{
    if (!OtherCharacter) return false;
    
    // 相同团队或非团队角色不视为敌对
    if (Team == ETeam::Neutral || OtherCharacter->GetTeam() == ETeam::Neutral)
    {
        return false;
    }
    
    return Team != OtherCharacter->GetTeam();
}
```

## 技术实现

### AI感知优化

```cpp
// 优化AI感知性能
UAISense_Sight::EVisibilityResult ASKGCharacter::CanBeSeenFrom(
    const FCanBeSeenFromContext& Context, 
    FVector& OutSeenLocation, 
    int32& OutNumberOfLoSChecksPerformed, 
    int32& OutNumberOfAsyncLosCheckRequested, 
    float& OutSightStrength, 
    int32* UserData, 
    const FOnPendingVisibilityQueryProcessedDelegate* Delegate)
{
    // 使用骨骼检查点进行视线检测
    for (const FName& BoneName : BonesToCheck)
    {
        FVector BoneLocation = GetMesh()->GetSocketLocation(BoneName);
        
        // 执行视线检测
        if (Context.TestPoint(BoneLocation, OutSeenLocation, UserData, Delegate))
        {
            OutNumberOfLoSChecksPerformed++;
            OutSightStrength = 1.0f;  // 完全可见
            
            // 根据骨骼重要性调整可见度
            if (BoneName == "head")
            {
                OutSightStrength = 1.5f;  // 头部更容易被发现
            }
            else if (BoneName.Contains("arm") || BoneName.Contains("hand"))
            {
                OutSightStrength = 0.8f;  // 手臂较难发现
            }
            
            return UAISense_Sight::EVisibilityResult::Visible;
        }
        
        OutNumberOfLoSChecksPerformed++;
    }
    
    return UAISense_Sight::EVisibilityResult::NotVisible;
}
```

### 性能优化

```cpp
// 使用缓存优化频繁检查
bool AMyCharacter::IsVisibleToAI() const
{
    // 缓存可见性状态，避免重复计算
    static float LastVisibilityCheckTime = 0.0f;
    static bool bCachedVisibility = false;
    
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastVisibilityCheckTime > 0.1f)  // 每0.1秒更新一次
    {
        bCachedVisibility = CalculateVisibilityToAI();
        LastVisibilityCheckTime = CurrentTime;
    }
    
    return bCachedVisibility;
}

// 批量处理AI感知更新
void BatchUpdateAIVisibility(const TArray<ASKGCharacter*>& Characters)
{
    for (ASKGCharacter* Character : Characters)
    {
        if (Character && Character->IsValidLowLevel())
        {
            Character->UpdateVisibilityCache();
        }
    }
}
```

## 配置示例

### 基础角色配置

```cpp
// 创建基础角色类
UCLASS()
class MYGAME_API AMyBaseCharacter : public ASKGCharacter
{
    GENERATED_BODY()
    
public:
    AMyBaseCharacter();
    
protected:
    // 配置AI感知
    virtual void BeginPlay() override;
    
private:
    // 配置骨骼检查点
    void SetupBonesToCheck();
};

AMyBaseCharacter::AMyBaseCharacter()
{
    // 设置合适的骨骼检查点
    BonesToCheck = {
        "head",           // 头部 - 高优先级目标
        "spine_03",       // 胸部 - 主要目标区域
        "pelvis",         // 骨盆 - 较大目标区域
        "upperarm_l",     // 左臂
        "upperarm_r",     // 右臂
        "thigh_l",        // 左腿
        "thigh_r",        // 右腿
    };
}
```

### 高级角色配置

```cpp
// 创建高级角色类
UCLASS()
class MYGAME_API AMyAdvancedCharacter : public ASKGCharacter
{
    GENERATED_BODY()
    
public:
    AMyAdvancedCharacter();
    
    // 暴露给蓝图的可配置属性
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float VisibilityModifier;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    bool bCanBeDetectedInCover;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TArray<FName> CriticalBones;  // 关键骨骼（被发现时优先报告）
};

AMyAdvancedCharacter::AMyAdvancedCharacter()
{
    VisibilityModifier = 1.0f;
    bCanBeDetectedInCover = false;
    
    CriticalBones = {
        "head",
        "spine_03"
    };
}
```

## 集成应用

### 与AI控制器集成

```cpp
// 完整的AI感知系统
void AMyAIController::SetupPerceptionSystem()
{
    // 创建感知组件
    UAIPerceptionComponent* PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
    
    // 配置视觉感知
    UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 3000.0f;
    SightConfig->LoseSightRadius = 3500.0f;
    SightConfig->PeripheralVisionAngleDegrees = 90.0f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    
    PerceptionComp->ConfigureSense(*SightConfig);
    PerceptionComp->SetDominantSense(UAISense_Sight::StaticClass());
    
    // 配置听觉感知
    UAISenseConfig_Hearing* HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 2000.0f;
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    
    PerceptionComp->ConfigureSense(*HearingConfig);
    
    // 绑定事件
    PerceptionComp->OnPerceptionUpdated.AddDynamic(this, &AMyAIController::OnPerceptionUpdated);
}
```

### 与游戏模式集成

```cpp
// 在游戏模式中管理角色
void AMyGameMode::HandleCharacterSpawned(ASKGCharacter* Character)
{
    if (Character)
    {
        // 注册到AI系统
        RegisterCharacterWithAI(Character);
        
        // 设置初始团队
        SetCharacterTeam(Character, GetNextTeam());
        
        // 绑定角色事件
        Character->OnDestroyed.AddDynamic(this, &AMyGameMode::OnCharacterDestroyed);
    }
}
```

## 技术细节

### AI感知优化

```cpp
// 高效的视线检测
bool PerformLineOfSightCheck(const FVector& Start, const FVector& End, const AActor* TargetActor)
{
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(TargetActor);
    QueryParams.bTraceComplex = false;  // 使用简单碰撞提高性能
    QueryParams.bReturnPhysicalMaterial = false;  // 不需要物理材质
    
    return GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);
}
```

### 内存管理

```cpp
// 使用对象池管理频繁创建的角色
class FCharacterPool
{
public:
    ASKGCharacter* GetPooledCharacter()
    {
        if (PooledCharacters.Num() > 0)
        {
            return PooledCharacters.Pop();
        }
        return nullptr;
    }
    
    void ReturnToPool(ASKGCharacter* Character)
    {
        if (Character)
        {
            Character->Reset();  // 重置角色状态
            PooledCharacters.Add(Character);
        }
    }
    
private:
    TArray<ASKGCharacter*> PooledCharacters;
};
```

## 常见问题

### Q: AI感知不工作？
A: 检查是否正确实现了感知接口，确认AI控制器有感知组件，验证感知配置参数

### Q: 骨骼检查点无效？
A: 确保骨骼名称正确，检查骨骼是否存在于角色骨架中，验证骨骼位置是否合理

### Q: 性能问题？
A: 减少骨骼检查点数量，使用缓存机制避免频繁计算，考虑使用LOD系统

### Q: 团队识别错误？
A: 检查团队设置逻辑，确保团队状态正确同步，验证敌对关系判断条件

### Q: 潜行系统不准确？
A: 调整可见度和噪音阈值，考虑环境因素对潜行的影响，验证状态更新逻辑

## 最佳实践

1. **骨骼选择**: 选择具有代表性的骨骼作为检查点
2. **性能平衡**: 在准确性和性能之间找到平衡
3. **状态管理**: 清晰管理角色的各种状态
4. **扩展性**: 设计易于扩展的角色系统
5. **调试支持**: 提供良好的调试和可视化工具

## 相关模块

- [SKGShooterFramework 模块](SKGShooterFramework模块.md) - 火器核心系统
- [SKGShooterFrameworkActors 模块](SKGShooterFrameworkActors模块.md) - 框架Actor类
- [SKGShooterFrameworkCore 模块](SKGShooterFrameworkCore模块.md) - 核心框架

<system-reminder>Whenever you read the file, you should consider whether the user explicitly asked for a summary, analysis, or explanation. If you did, provide it. If not, continue with the task as requested.}</content>