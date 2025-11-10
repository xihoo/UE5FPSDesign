# SKGShooterFramework 模块文档

## 模块概述

SKGShooterFramework 模块是 SKGShooterFramework 的核心模块，提供了火器系统的完整实现。该模块作为整个框架的中心枢纽，统一管理武器的所有组件、附件、统计数据、瞄准设备、程序化动画等核心功能。它是构建真实射击体验的基础，提供了高度模块化和可扩展的架构。

## 核心组件

### USKGShooterPawnComponent

**类路径**: `Source/SKGShooterFramework/Public/Components/SKGShooterPawnComponent.h`

这是整个射击框架的核心组件，作为玩家与射击系统交互的主要接口。它提供了完整的玩家控制、武器管理、动画协调和网络同步功能，是整个框架的中心枢纽。

#### 核心功能
- **武器持有管理**: 统一管理玩家当前持有的武器和装备
- **程序化动画协调**: 协调姿态、后坐力、倾斜、自由视角等动画
- **输入处理**: 处理瞄准、倾斜、自由视角等玩家输入
- **网络同步**: 确保多人游戏中的状态一致性
- **灵敏度管理**: 根据瞄准镜倍率动态调整鼠标灵敏度
- **组件发现**: 自动发现和管理相关组件

#### 核心数据结构

##### FSKGFreeLookSettings
```cpp
USTRUCT(BlueprintType)
struct FSKGFreeLookSettings
{
    GENERATED_BODY()
    // 自动设置控制器旋转
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFreeLookSettings")
    bool bAutoSetUseControllerRotationYaw {true};
    
    // 最大俯仰角度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFreeLookSettings")
    float MaxPitchUp {40};
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFreeLookSettings")
    float MaxPitchDown {30};
    
    // 最大偏航角度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFreeLookSettings")
    float MaxYawLeft {35};
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFreeLookSettings")
    float MaxYawRight {35};
};
```

##### 网络同步数据
```cpp
// 持有武器
UPROPERTY(ReplicatedUsing = OnRep_HeldActor, BlueprintGetter = GetHeldActor, Category = "SKGShooterPawnComponent")
TObjectPtr<AActor> HeldActor;

// 瞄准状态
UPROPERTY(ReplicatedUsing = OnRep_IsAiming, BlueprintGetter = IsAiming, Category = "SKGShooterPawnComponent")
bool bIsAiming {false};

// 程序化姿态数据
UPROPERTY(ReplicatedUsing = OnRep_CurrentProceduralPoseData, BlueprintGetter = GetCurrentProceduralPoseReplicationData, Category = "SKGShooterPawnComponent")
FSKGProceduralPoseReplicationData CurrentProceduralPoseData;

// 自由视角状态
UPROPERTY(ReplicatedUsing = OnRep_InFreeLook)
bool bInFreeLook {false};

// 倾斜角度（压缩存储）
UPROPERTY(ReplicatedUsing = OnRep_TargetLeanAngleCompressed)
uint8 TargetLeanAngleCompressed {0};
```

#### 主要方法

##### 武器管理
```cpp
// 设置持有武器
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|HeldActor")
void SetHeldActor(AActor* Actor);

// 清除持有武器
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|HeldActor")
void ClearHeldActor();

// 获取当前武器组件
UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Components")
USKGFirearmComponent* GetCurrentFirearmComponent() const { return CurrentFirearmComponent; }
```

##### 瞄准系统
```cpp
// 开始瞄准
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
void StartAiming();

// 停止瞄准
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
void StopAiming();

// 检查是否瞄准中
UFUNCTION(BlueprintGetter, Category = "SKGShooterPawnComponent|ProceduralAnimation")
FORCEINLINE bool IsAiming() const { return bIsAiming; }
```

##### 自由视角系统
```cpp
// 开始自由视角
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|FreeLook")
void StartFreeLook();

// 停止自由视角
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|FreeLook")
void StopFreeLook();

// 检查是否自由视角中
UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|FreeLook")
bool IsFreeLooking() const { return bInFreeLook; }

// 检查是否可以添加偏航
UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|FreeLook")
bool CanAddYaw() const;

// 检查是否可以添加俯仰
UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|FreeLook")
bool CanAddPitch() const;
```

##### 倾斜系统
```cpp
// 向左倾斜
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|Leaning")
void LeanLeft(float TargetAngle);

// 向右倾斜
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|Leaning")
void LeanRight(float TargetAngle);

// 停止左倾斜
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|Leaning")
void StopLeaningLeft();

// 停止右倾斜
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|Leaning")
void StopLeaningRight();

// 获取目标倾斜角度
UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Leaning")
FORCEINLINE float GetTargetLeanAngle() const { return TargetLeanAngle; }
```

##### 程序化动画
```cpp
// 执行程序化后坐力
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
void PerformProceduralRecoil(const FRotator ControlRotationMultiplier = FRotator(1.0, 1.0, 1.0), const FVector LocationMultiplier = FVector(1.0), const FRotator RotationMultiplier = FRotator(1.0, 1.0, 1.0));

// 执行程序化姿态
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
void PerformProceduralPose(const FGameplayTag Tag, bool bExitPose);

// 执行自定义曲线
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
void PerformCustomCurve(const FSKGCurveSettings& CurveData);
```

##### 灵敏度管理
```cpp
// 获取瞄准镜倍率灵敏度倍数
UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Sensitivity")
virtual float GetMagnificationSensitivityMultiplier() const;

// 获取灵敏度倍数
UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Sensitivity")
virtual void GetSensitivityMultiplier(const float X, const float XBaseTurnRate, const float Y, const float YBaseTurnRate, float& NewX, bool& AddYaw, float& NewY, bool& AddPitch) const;
```

#### 网络同步架构

##### 服务器授权模式
```cpp
// 关键状态的服务器授权
UFUNCTION(Server, Reliable, WithValidation)
void Server_SetAiming(bool bAim);

UFUNCTION(Server, Reliable, WithValidation)
void Server_Lean(const uint8 TargetAngle);

UFUNCTION(Server, Reliable, WithValidation)
void Server_PerformProceduralPose(const FGameplayTag& Tag, bool bExitPose);

UFUNCTION(Server, Reliable, WithValidation)
void Server_SetFreeLook(bool bFreeLook);
```

##### 数据压缩策略
```cpp
// 角度压缩到8位以节省带宽
uint8 CompressAngle(float Angle, float MaxAngle)
{
    float Normalized = FMath::Clamp(Angle / MaxAngle, -1.0f, 1.0f);
    return FMath::RoundToInt((Normalized + 1.0f) * 127.5f);
}

float DecompressAngle(uint8 Compressed, float MaxAngle)
{
    float Normalized = (Compressed / 255.0f) * 2.0f - 1.0f;
    return Normalized * MaxAngle;
}
```

#### 事件委托系统

```cpp
// 姿态完成事件
UPROPERTY(BlueprintAssignable, Category = "SKGShooterPawnComponent|Events")
FOnPoseComplete OnPoseComplete;

// 持有武器变化事件
UPROPERTY(BlueprintAssignable, Category = "SKGShooterPawnComponent|Events")
FOnHeldActorSet OnHeldActorSet;

// 动画实例更新事件
UPROPERTY(BlueprintAssignable, Category = "SKGShooterPawnComponent|Events")
FOnAnimInstanceTicked OnAnimInstanceTicked;

// 瞄准状态变化事件
UPROPERTY(BlueprintAssignable, Category = "SKGShooterPawnComponent|Events")
FOnAimStateChanged OnAimStateChanged;

// 自由视角状态变化事件
UPROPERTY(BlueprintAssignable, Category = "SKGShooterPawnComponent|Events")
FOnFreeLookStateChanged OnFreeLookStateChanged;

// 副手IK左右手变化事件
UPROPERTY(BlueprintAssignable, Category = "SKGShooterPawnComponent|Events")
FOnOffhandIKIsLeftHandChanged OnOffhandIKIsLeftHandChanged;

// 火器碰撞变化事件
UPROPERTY(BlueprintAssignable, Category = "SKGShooterPawnComponent|Events")
FOnFirearmCollisionChanged OnFirearmCollisionChanged;
```

#### 性能优化特性

##### 1. 组件缓存机制
```cpp
// 缓存常用组件引用避免重复查找
void CacheComponentReferences()
{
    if (!CurrentFirearmComponent.IsValid())
    {
        CurrentFirearmComponent = FindComponentByClass<USKGFirearmComponent>();
    }
    
    if (!CurrentProceduralAnimComponent.IsValid())
    {
        CurrentProceduralAnimComponent = FindComponentByClass<USKGProceduralAnimComponent>();
    }
}
```

##### 2. 自适应网络更新频率
```cpp
// 根据玩家活动状态调整网络更新频率
void UpdateNetworkFrequency()
{
    if (bIsAiming || bInFreeLook)
    {
        SetNetUpdateFrequency(60.0f);  // 高活动状态
    }
    else if (IsMoving())
    {
        SetNetUpdateFrequency(30.0f);  // 中等活动状态
    }
    else
    {
        SetNetUpdateFrequency(10.0f);  // 低活动状态
    }
}
```

##### 3. 动画数据更新优化
```cpp
// 只在必要时更新动画数据
void UpdateProceduralData()
{
    if (!bNeedsProceduralUpdate) return;
    
    FSKGProceduralAnimInstanceData NewData = CalculateProceduralData();
    if (NewData != CachedProceduralData)
    {
        CachedProceduralData = NewData;
        bNeedsProceduralUpdate = false;
        
        // 触发动画更新
        if (ShooterFrameworkAnimInstance)
        {
            ShooterFrameworkAnimInstance->UpdateProceduralData(NewData);
        }
    }
}
```

### USKGFirearmComponent

**类路径**: `Source/SKGShooterFramework/Public/Components/SKGFirearmComponent.h`

这是火器系统的核心组件，作为整个武器系统的中心管理器，负责协调所有子组件的工作。

#### 主要功能
- **组件管理**: 统一管理所有武器相关组件（瞄准镜、枪口、激光、枪托等）
- **附件系统**: 处理附件的添加、移除和状态同步
- **瞄准设备**: 管理多个瞄准设备，支持循环切换
- **统计数据**: 维护武器的综合统计数据
- **程序化动画**: 协调程序化动画组件
- **碰撞检测**: 处理武器碰撞和姿态调整
- **网络同步**: 完整的客户端-服务器状态同步

#### 核心数据结构

##### FSKGFirearmStats
```cpp
USTRUCT(BlueprintType)
struct FSKGFirearmStats
{
    GENERATED_BODY()
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFirearmStats")
    float Weight {0.0f};                    // 重量
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFirearmStats")
    float Durability {0.0f};                // 耐久度
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFirearmStats")
    float Ergonomics {0.0f};                // 人机工程学
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFirearmStats")
    float Accuracy {0.0f};                  // 精度（MOA）
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFirearmStats")
    float VerticalRecoil {0.0f};            // 垂直后坐力百分比
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFirearmStats")
    float HorizontalRecoil {0.0f};          // 水平后坐力百分比
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFirearmStats")
    float MuzzleVelocityPercentage {0.0f};  // 初速百分比
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFirearmStats")
    float FireRate {0.0f};                  // 射速（发/分钟）
};
```

##### FSKGProceduralStats
```cpp
USTRUCT(BlueprintType)
struct FSKGProceduralStats
{
    GENERATED_BODY()
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGProcedural")
    float AimInterpolationRate {10.0f};                    // 瞄准插值速率
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGProcedural")
    float MovementSwayMultiplier {1.0f};                   // 移动摆动倍数
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGProcedural")
    FSKGCycleAimingPointSpringInterpSettings CycleAimingPointSpringInterpSettings;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGProcedural")
    FSKGMovementLagSpringInterpSettings MovementLagSpringInterpSettings;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGProcedural")
    float MovementLagInterpSetting {10.0f};                // 移动延迟插值设置
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGProcedural")
    FSKGRotationLagSpringInterpSettings RotationLagSpringInterpSettings;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGProcedural")
    FSKGRotationLagInterpSettings RotationLagInterpSettings;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGProcedural")
    FVector ControlRotationRecoilMultipliers = {FVector::OneVector};  // 控制旋转后坐力倍数
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGProcedural")
    FVector RecoilLocationMultipliers {FVector::OneVector};  // 后坐力位置倍数
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGProcedural")
    FVector RecoilRotationMultipliers {FVector::OneVector};  // 后坐力旋转倍数
};
```

##### FSKGProceduralAnimInstanceData
```cpp
USTRUCT()
struct FSKGProceduralAnimInstanceData
{
    GENERATED_BODY()
    
    FTransform AimOffset {FTransform()};                    // 瞄准偏移
    float AimOffsetInterpSpeed {10.0f};                     // 瞄准偏移插值速度
    FTransform OffhandIKOffset {FTransform()};              // 副手IK偏移
    bool bOffhandIKIsLeftHand {true};                       // 副手IK是否为左手
    bool bInFreeLook {false};                               // 是否自由观察
    FRotator FreeLookStartRotation {FRotator::ZeroRotator}; // 自由观察起始旋转
    float LengthOfPull {0.0f};                              // 扳机拉力
    
    UPROPERTY()
    TObjectPtr<UAnimSequence> OffhandIKPose;                // 副手IK姿势
    
    FSKGLeanLeftRightSettings LeanLeftRightSettings;        // 左右倾斜设置
    FSKGFirstAndThirdPersonBasePoseOffset BasePoseOffset;   // 基础姿态偏移
    FTransform ThirdPersonAimingOffset;                     // 第三人称瞄准偏移
    FSKGCycleAimingPointSettings CycleAimingPointSettings;  // 瞄准点循环设置
    FSKGMovementSwaySettings MovementSwaySettings;          // 移动摆动设置
    FSKGMovementLagSettings MovementLagSettings;            // 移动延迟设置
    FSKGRotationLagSettings RotationLagSettings;            // 旋转延迟设置
    FSKGDeadzoneSettings DeadzoneSettings;                // 死区设置
    FSKGRecoilSettings RecoilSettings;                      // 后坐力设置
    FSKGFirearmCollisionData FirearmCollisionSettings;      // 火器碰撞数据
    FVector2D MouseInput {FVector2D::ZeroVector};           // 鼠标输入
    FSKGProceduralStats ProceduralStats;                    // 程序化统计数据
    
    bool bProceduralAnimDataSet {false};                    // 程序化动画数据是否设置
};
```

#### 核心属性

##### 基础配置
```cpp
// 是否自动初始化
UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent|Initialize")
bool bAutoInitialize {true};

// 火器网格组件名称
UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent|Initialize")
FName FirearmMeshComponentName {NAME_None};

// 附件管理器组件名称
UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent|Initialize")
FName AttachmentManagerComponentName {NAME_None};

// 火器统计数据资产
UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent|Initialize")
TObjectPtr<USKGPDAFirearmStats> FirearmStatsDataAsset;

// 火器碰撞设置数据资产
UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent|FirearmCollision")
TObjectPtr<USKGPDAFirearmCollisionSettings> FirearmCollisionSettingsDataAsset;
```

##### 组件数组（网络同步）
```cpp
// 程序化动画组件数组
UPROPERTY(BlueprintGetter = GetProceduralAnimComponents, ReplicatedUsing = OnRep_ProceduralAnimComponents)
TArray<TObjectPtr<USKGProceduralAnimComponent>> ProceduralAnimComponents;

// 副手IK组件数组
UPROPERTY(BlueprintGetter = GetOffhandIKComponents, ReplicatedUsing = OnRep_OffhandIKComponents)
TArray<TObjectPtr<USKGOffhandIKComponent>> OffhandIKComponents;

// 激光指示器组件数组
UPPROPERTY(BlueprintGetter = GetLightLaserComponents, ReplicatedUsing = OnRep_LightLaserComponents)
TArray<TObjectPtr<USKGLightLaserComponent>> LightLaserComponents;

// 枪口组件数组
UPROPERTY(BlueprintGetter = GetMuzzleComponents, ReplicatedUsing = OnRep_MuzzleComponents)
TArray<TObjectPtr<USKGMuzzleComponent>> MuzzleComponents;

// 附件统计组件数组
UPPROPERTY(BlueprintGetter = GetAttachmentStatComponents, ReplicatedUsing = OnRep_AttachmentStatComponents)
TArray<TObjectPtr<USKGFirearmAttachmentStatComponent>> AttachmentStatComponents;
```

##### 当前活动组件
```cpp
// 当前程序化动画组件
UPROPERTY(ReplicatedUsing = OnRep_CurrentProceduralAnimComponent)
TObjectPtr<USKGProceduralAnimComponent> CurrentProceduralAnimComponent;

// 当前副手IK组件
UPROPERTY(ReplicatedUsing = OnRep_CurrentOffhandIKComponent)
TObjectPtr<USKGOffhandIKComponent> CurrentOffhandIKComponent;

// 当前枪口组件
UPROPERTY(ReplicatedUsing = OnRep_CurrentMuzzleComponent)
TObjectPtr<USKGMuzzleComponent> CurrentMuzzleComponent;

// 当前瞄准镜组件
UPROPERTY(ReplicatedUsing = OnRep_CurrentOpticComponent)
TObjectPtr<USKGOpticComponent> CurrentOpticComponent;

// 当前枪托组件
UPPROPERTY(ReplicatedUsing = OnRep_CurrentStockComponent)
TObjectPtr<USKGStockComponent> CurrentStockComponent;
```

#### 主要方法

##### 组件管理
```cpp
// 添加附件
UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Attachments")
void AddAttachment(AActor* Attachment);

// 移除附件
UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Attachments")
void RemoveAttachment(AActor* Attachment);

// 获取组件
UFUNCTION(BlueprintPure, Category = "SKGShooterFrameworkStatics|Getters")
static USKGFirearmComponent* GetFirearmComponent(const AActor* Actor);

UFUNCTION(BlueprintPure, Category = "SKGShooterFrameworkStatics|Getters")
static USKGFirearmComponent* GetParentFirearmComponent(const AActor* Actor);
```

##### 瞄准设备管理
```cpp
// 循环瞄准设备
UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Aim")
bool CycleAimingDevice(bool bIsAiming);

// 设置瞄准设备
UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Aim")
bool SetAimingDevice(USKGProceduralAnimComponent* AnimComponent, const bool bIsAiming);

// 开始点射瞄准
UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Aim")
void StartPointAiming(bool bRightHandDominant);

// 停止点射瞄准
UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Aim")
void StopPointAiming(bool bIsAiming);

// 检查是否点射瞄准
UFUNCTION(BlueprintPure, Category = "SKGFirearmComponent|Aim")
bool IsPointAiming() const { return bIsPointAiming; }
```

##### 枪口管理
```cpp
// 射击时调用
UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Muzzle")
void ShotPerformed();

// 获取枪口弹道变换
UFUNCTION(BlueprintPure, Category = "SKGFirearmComponent|Muzzle")
FSKGMuzzleTransform GetMuzzleProjectileTransform(float ZeroDistanceMeters = 100.0f, float MOA = 1.0f) const;

// 获取多个枪口弹道变换（霰弹等）
UFUNCTION(BlueprintPure, Category = "SKGFirearmComponent|Muzzle")
TArray<FSKGMuzzleTransform> GetMuzzleProjectileTransforms(float ZeroDistanceMeters = 100.0f, float MOA = 1.0f, const int32 ProjectileCount = 8) const;

// 获取枪口变换
UFUNCTION(BlueprintPure, Category = "SKGFirearmComponent|Muzzle")
FTransform GetMuzzleTransform() const;

// 检查是否消音
UFUNCTION(BlueprintPure, Category = "SKGFirearm")
bool IsSuppressed() const;
```

##### 统计数据管理
```cpp
// 设置程序化统计数据
UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|ProceduralStats")
void SetProceduralStats(const FSKGProceduralStats& ProceduralStatsData);

// 获取程序化统计数据
UFUNCTION(BlueprintPure, Category = "SKGFirearmComponent|ProceduralStats")
const FSKGProceduralStats& GetProceduralStats() const;

// 设置火器统计数据
UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|FirearmStats")
void SetFirearmStats(const FSKGFirearmStats& Stats) { FirearmStats = Stats; }

// 获取火器统计数据
UFUNCTION(BlueprintGetter)
const FSKGFirearmStats& GetFirearmStats() const { return FirearmStats; }
```

##### 姿态管理
```cpp
// 获取姿态数据
UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Pose")
bool GetPose(FGameplayTag Tag, FSKGToFromCurveSettings& PoseData);

// 获取程序化数据
FSKGProceduralAnimInstanceData& GetProceduralData(bool bIsAiming, bool bOffhandIKIsLeftHand);
```

##### 归零管理
```cpp
// 在指定位置归零瞄准镜
UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Zero")
void ZeroOpticsForZeroAtLocation(const FVector& Location);
```

#### 事件委托

```cpp
// 瞄准设备循环事件
UPROPERTY(BlueprintAssignable, Category = "SKGFirearmComponent|Events")
FOnAimingDeviceCycled OnAimingDeviceCycled;

// 程序化动画组件更新事件
UPROPERTY(BlueprintAssignable, Category = "SKGFirearmComponent|Events")
FOnProceduralAnimComponentsUpdated OnProceduralAnimComponentsUpdated;

// 枪口组件更新事件
UPROPERTY(BlueprintAssignable, Category = "SKGFirearmComponent|Events")
FOnMuzzleComponentUpdated OnMuzzleComponentUpdated;

// 持有事件
UPROPERTY(BlueprintAssignable, Category = "SKGFirearmComponent|Events")
FOnHeld OnHeld;

// 火器统计数据变化事件
UPROPERTY(BlueprintAssignable, Category = "SKGFirearmComponent|Events")
FOnFirearmStatsChanged OnFirearmStatsChanged;

// 程序化统计数据计算事件
UPROPERTY(BlueprintAssignable, Category = "SKGFirearmComponent|Events")
FCalculateProceduralStats CalculateProceduralStats;
```

## 数据资产系统

### USKGPDAFirearmCollisionSettings

**类路径**: `Source/SKGShooterFramework/Public/DataAssets/SKGPDAFirearmCollisionSettings.h`

管理火器碰撞设置的数据资产。

#### 核心结构

##### FSKGFirearmCollisionSettings
```cpp
USTRUCT(BlueprintType)
struct FSKGFirearmCollisionSettings
{
    GENERATED_BODY()
    
    // 是否使用火器碰撞
    UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent")
    bool bUseFirearmCollision {true};
    
    // 碰撞检测直径
    UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent")
    float TraceDiameter {2.0f};
    
    // 停止瞄准距离
    UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent")
    float StopAimingDistance {0.2f};
    
    // 位置曲线
    UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent")
    TObjectPtr<UCurveVector> PoseLocationCurve;
    
    // 旋转曲线
    UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent")
    TObjectPtr<UCurveVector> PoseRotationCurve;
    
    // 姿态缩放
    UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent")
    float PoseScale {2.0f};
    
    // 位置插值速度
    UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent")
    float PoseLocationInterpSpeed {10.0f};
    
    // 旋转插值速度
    UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent")
    float PoseRotationInterpSpeed {10.0f};
};
```

## 使用方法

### 基础设置

1. **添加组件**: 在武器 Actor 上添加 `USKGFirearmComponent`
2. **配置数据资产**: 设置火器统计数据和碰撞设置数据资产
3. **定义组件名称**: 配置火器网格和附件管理器组件名称
4. **初始化**: 组件会自动初始化并发现所有子组件

### 组件管理

```cpp
// 获取火器组件
USKGFirearmComponent* FirearmComponent = Weapon->FindComponentByClass<USKGFirearmComponent>();

// 添加附件
FirearmComponent->AddAttachment(NewAttachment);

// 移除附件
FirearmComponent->RemoveAttachment(OldAttachment);
```

### 瞄准设备管理

```cpp
// 循环瞄准设备
FirearmComponent->CycleAimingDevice(true);

// 获取当前瞄准镜
USKGOpticComponent* CurrentOptic = FirearmComponent->GetCurrentOpticComponent();

// 获取所有瞄准设备
TArray<USKGProceduralAnimComponent*> AimingDevices = FirearmComponent->GetProceduralAnimComponents();
```

### 枪口管理

```cpp
// 射击时调用
FirearmComponent->ShotPerformed();

// 获取枪口变换
FTransform MuzzleTransform = FirearmComponent->GetMuzzleTransform();

// 获取弹道变换
FSKGMuzzleTransform ProjectileTransform = FirearmComponent->GetMuzzleProjectileTransform(100.0f, 1.0f);
```

### 统计数据管理

```cpp
// 获取火器统计数据
const FSKGFirearmStats& Stats = FirearmComponent->GetFirearmStats();

// 获取程序化统计数据
const FSKGProceduralStats& ProceduralStats = FirearmComponent->GetProceduralStats();

// 设置程序化统计数据
FirearmComponent->SetProceduralStats(NewProceduralStats);
```

### 事件监听

```cpp
// 绑定瞄准设备变化事件
FirearmComponent->OnAimingDeviceCycled.AddDynamic(this, &AMyWeapon::OnAimingDeviceCycled);

// 处理瞄准设备变化
void AMyWeapon::OnAimingDeviceCycled(USKGProceduralAnimComponent* ProceduralAnimComponent)
{
    // 更新UI或播放音效
    UpdateAimingDeviceUI(ProceduralAnimComponent);
}

// 绑定统计数据变化事件
FirearmComponent->OnFirearmStatsChanged.AddDynamic(this, &AMyWeapon::OnFirearmStatsChanged);

// 处理统计数据变化
void AMyWeapon::OnFirearmStatsChanged(FSKGFirearmStats FirearmStats)
{
    // 更新武器属性显示
    UpdateWeaponStatsDisplay(FirearmStats);
}
```

## 高级功能

### 程序化统计数据计算

组件提供了可重写的程序化统计数据计算功能：

```cpp
UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFirearmComponent|Stats")
void CalculateProceduralValues();

virtual void CalculateProceduralValues_Implementation();
```

通过重写此函数，可以自定义武器的行为参数：

```cpp
void AMyFirearm::CalculateProceduralValues_Implementation()
{
    // 调用父类实现
    Super::CalculateProceduralValues_Implementation();
    
    // 获取当前统计数据
    FSKGProceduralStats CurrentStats = GetProceduralStats();
    
    // 根据附件修改参数
    if (HasSuppressor())
    {
        CurrentStats.RecoilLocationMultipliers *= 0.8f;  // 减少后坐力
        CurrentStats.ControlRotationRecoilMultipliers *= 0.7f;
    }
    
    // 应用修改后的统计数据
    SetProceduralStats(CurrentStats);
}
```

### 碰撞检测系统

火器组件提供了先进的碰撞检测系统：

```cpp
// 碰撞设置
UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent|FirearmCollision")
TObjectPtr<USKGPDAFirearmCollisionSettings> FirearmCollisionSettingsDataAsset;

// 碰撞数据
FSKGFirearmCollisionData FirearmCollisionSettings;
```

碰撞系统可以：
- 检测武器与环境的碰撞
- 自动调整武器姿态避免穿模
- 防止在狭窄空间内瞄准
- 提供平滑的姿态过渡

### 点射瞄准系统

支持点射瞄准（不通过瞄准镜直接瞄准）：

```cpp
// 开始点射瞄准
FirearmComponent->StartPointAiming(true);  // 右手主导

// 停止点射瞄准
FirearmComponent->StopPointAiming(true);

// 检查是否点射瞄准
bool bIsPointAiming = FirearmComponent->IsPointAiming();
```

### 多瞄准设备支持

支持在同一武器上安装多个瞄准设备：

```cpp
// 获取所有瞄准设备
TArray<USKGProceduralAnimComponent*> AimingDevices = FirearmComponent->GetProceduralAnimComponents();

// 获取增倍镜
TArray<USKGOpticComponent*> Magnifiers = FirearmComponent->GetMagnifiers();

// 循环切换瞄准设备
FirearmComponent->CycleAimingDevice(true);
```

## 技术细节

### 组件发现机制

组件使用智能发现机制自动查找和管理子组件：

```cpp
void SetupComponents()
{
    // 查找并缓存所有相关组件
    // 建立组件间的关联关系
    // 设置默认的活动组件
}
```

### 网络架构

采用服务器授权模式，关键数据通过RPC同步：

```cpp
// 服务器设置瞄准设备
UFUNCTION(Server, Reliable, WithValidation)
void Server_SetAimingDevice(USKGProceduralAnimComponent* AnimComponent);

// 统计数据复制
UPROPERTY(ReplicatedUsing = OnRep_FirearmStats)
FSKGFirearmStats FirearmStats;
```

### 性能优化

- **缓存机制**: 缓存常用组件引用避免重复查找
- **延迟初始化**: 组件在需要时才进行完整初始化
- **事件驱动**: 使用委托系统减少轮询开销
- **网络优化**: 合理设置网络更新频率

## 配置示例

### 标准步枪配置

```cpp
// 基础配置
FirearmMeshComponentName = "WeaponMesh";
AttachmentManagerComponentName = "AttachmentManager";

// 统计数据配置
FirearmStatsDataAsset = LoadObject<USKGPDAFirearmStats>(...);

// 碰撞设置
FirearmCollisionSettingsDataAsset = LoadObject<USKGPDAFirearmCollisionSettings>(...);
```

### 高精度狙击步枪配置

```cpp
// 高精度配置
FirearmStatsDataAsset->FirearmStats.Accuracy = 0.5f;     // 0.5 MOA精度
FirearmStatsDataAsset->FirearmStats.Weight = 6.5f;      // 6.5kg重量
FirearmStatsDataAsset->FirearmStats.Ergonomics = 85.0f;  // 85人机工程学
```

### 轻机枪配置

```cpp
// 高射速配置
FirearmStatsDataAsset->FirearmStats.FireRate = 800.0f;   // 800发/分钟
FirearmStatsDataAsset->FirearmStats.VerticalRecoil = 15.0f;  // 15%垂直后坐力
FirearmStatsDataAsset->FirearmStats.HorizontalRecoil = 8.0f; // 8%水平后坐力
```

## 最佳实践

1. **组件组织**: 合理组织组件层次结构，确保正确的发现顺序
2. **性能考虑**: 避免频繁的组件切换，合理使用缓存
3. **网络优化**: 合理设置网络复制频率，避免状态同步问题
4. **数据完整性**: 确保统计数据的一致性，避免并发修改
5. **错误处理**: 妥善处理组件缺失或无效状态

## 常见问题

### Q: 组件发现失败？
A: 检查组件命名是否正确，确保组件在正确的Actor上

### Q: 瞄准设备切换无效？
A: 确保瞄准设备组件正确注册并设置了有效的瞄准点

### Q: 统计数据不更新？
A: 检查附件统计组件是否正确配置，确保网络同步正常

### Q: 碰撞检测不工作？
A: 验证碰撞设置数据资产配置，检查碰撞通道设置

### Q: 网络同步问题？
A: 检查服务器授权设置，确保关键数据通过RPC同步

## 相关模块

- [SKGAttachment 模块](SKGAttachment模块.md) - 附件管理系统
- [SKGProceduralAnim 模块](SKGProceduralAnim模块.md) - 程序化动画系统
- [SKGMuzzle 模块](SKGMuzzle模块.md) - 枪口装置系统
- [SKGOptic 模块](SKGOptic模块.md) - 光学瞄准镜系统
- [SKGStock 模块](SKGStock模块.md) - 枪托系统

## 扩展应用

### 自定义武器类型

```cpp
// 突击步枪
AssaultRifle->FirearmStatsDataAsset = AssaultRifleStats;
AssaultRifle->FirearmCollisionSettingsDataAsset = StandardCollisionSettings;

// 狙击步枪
SniperRifle->FirearmStatsDataAsset = SniperRifleStats;
SniperRifle->FirearmCollisionSettingsDataAsset = PrecisionCollisionSettings;

// 霰弹枪
Shotgun->FirearmStatsDataAsset = ShotgunStats;
Shotgun->FirearmCollisionSettingsDataAsset = ShotgunCollisionSettings;
```

### 高级统计系统

```cpp
// 动态统计计算
void CalculateDynamicStats()
{
    FSKGFirearmStats BaseStats = GetFirearmStats();
    
    // 根据武器状态调整
    if (IsOverheated())
    {
        BaseStats.Accuracy *= 0.8f;  // 过热降低精度
    }
    
    if (IsDamaged())
    {
        BaseStats.Reliability *= 0.7f;  // 损坏降低可靠性
    }
    
    SetFirearmStats(BaseStats);
}
```

### 性能分析

内置性能统计：

```cpp
DECLARE_STATS_GROUP(TEXT("SKGShooterFrameworkFirearmComponentStatGroup"), 
                    STATGROUP_SKGShooterFrameworkFirearmComponent, STATCAT_Advanced);
```

<system-reminder>Whenever you read the file, you should consider whether the user explicitly asked for a summary, analysis, or explanation. If they did, provide it. If not, continue with the task as requested.}</content>