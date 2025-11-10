# SKGShooterFramework 技术架构综述 - 完整版

## 概述

SKGShooterFramework 是一个专为 Unreal Engine 5.6+ 设计的模块化射击游戏框架，提供了构建真实射击体验所需的完整工具集。该框架采用组件化架构，通过数据驱动的方式实现高度可配置和可扩展的武器系统。

## 核心架构原则

### 1. 模块化设计
- **独立模块**: 每个功能模块独立存在，可单独使用
- **松耦合**: 模块间通过接口和事件进行通信
- **可插拔**: 支持动态添加和移除功能模块

### 2. 数据驱动
- **配置优先**: 通过数据资产配置系统行为
- **运行时切换**: 支持运行时动态切换配置
- **版本控制**: 数据资产易于版本控制和管理

### 3. 网络优先
- **服务器授权**: 关键功能采用服务器授权模式
- **状态同步**: 完整的客户端-服务器状态同步
- **性能优化**: 智能的网络更新频率控制

### 4. 性能优化
- **缓存机制**: 大量使用缓存避免重复计算
- **事件驱动**: 减少轮询，使用事件通知
- **LOD系统**: 支持基于距离的LOD优化

## 核心组件架构

### USKGShooterPawnComponent - 玩家交互中心

**类路径**: `Source/SKGShooterFramework/Public/Components/SKGShooterPawnComponent.h`

这是框架中最重要的组件，作为玩家与整个射击系统交互的主要接口。

#### 核心功能

##### 1. 程序化动画控制
```cpp
// 瞄准状态管理
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
void StartAiming();
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
void StopAiming();

// 姿态系统
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
void PerformProceduralPose(const FGameplayTag Tag, bool bExitPose);

// 后坐力系统
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
void PerformProceduralRecoil(const FRotator ControlRotationMultiplier, const FVector LocationMultiplier, const FRotator RotationMultiplier);
```

##### 2. 自由视角系统
```cpp
// 自由视角控制
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|FreeLook")
void StartFreeLook();
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|FreeLook")
void StopFreeLook();
UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|FreeLook")
bool IsFreeLooking() const { return bInFreeLook; }

// 视角限制检查
UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|FreeLook")
bool CanAddYaw() const;
UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|FreeLook")
bool CanAddPitch() const;
```

##### 3. 倾斜系统
```cpp
// 左右倾斜
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|Leaning")
void LeanLeft(float TargetAngle);
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|Leaning")
void LeanRight(float TargetAngle);
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|Leaning")
void StopLeaningLeft();
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|Leaning")
void StopLeaningRight();
```

##### 4. 武器持有管理
```cpp
// 武器持有
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|HeldActor")
void SetHeldActor(AActor* Actor);
UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|HeldActor")
void ClearHeldActor();
UFUNCTION(BlueprintGetter)
AActor* GetHeldActor() const { return HeldActor; }
```

##### 5. 灵敏度管理
```cpp
// 倍率灵敏度
UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Sensitivity")
float GetMagnificationSensitivityMultiplier() const;

// 自定义灵敏度计算
UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGShooterPawnComponent|Sensitivity")
void BP_GetSensitivityMultiplier(const float X, const float XBaseTurnRate, const float Y, const float YBaseTurnRate, float& NewX, bool& AddYaw, float& NewY, bool& AddPitch) const;
```

##### 6. 组件集成接口
```cpp
// 获取当前武器组件
UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Components")
USKGFirearmComponent* GetCurrentFirearmComponent() const { return CurrentFirearmComponent; }

// 获取当前瞄准镜组件
UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Components")
USKGOpticComponent* GetCurrentOpticComponent() const;

// 获取激光/照明组件
UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Components")
TArray<USKGLightLaserComponent*> GetCurrentLightLaserComponents() const;
```

#### 核心数据结构

##### FSKGFreeLookSettings
```cpp
USTRUCT(BlueprintType)
struct FSKGFreeLookSettings
{
    GENERATED_BODY()
    
    // 是否自动控制旋转
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFreeLookSettings")
    bool bAutoSetUseControllerRotationYaw {true};
    
    // 最大俯仰角
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFreeLookSettings")
    float MaxPitchUp {40};
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFreeLookSettings")
    float MaxPitchDown {30};
    
    // 最大偏航角
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFreeLookSettings")
    float MaxYawLeft {35};
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFreeLookSettings")
    float MaxYawRight {35};
};
```

##### 网络同步数据
```cpp
// 倾斜角度压缩
UPROPERTY(ReplicatedUsing = OnRep_TargetLeanAngleCompressed)
uint8 TargetLeanAngleCompressed {0};

// 瞄准状态
UPROPERTY(ReplicatedUsing = OnRep_IsAiming, BlueprintGetter = IsAiming, Category = "SKGShooterPawnComponent")
bool bIsAiming {false};

// 程序化姿态数据
UPROPERTY(ReplicatedUsing = OnRep_CurrentProceduralPoseData, BlueprintGetter = GetCurrentProceduralPoseReplicationData, Category = "SKGShooterPawnComponent")
FSKGProceduralPoseReplicationData CurrentProceduralPoseData;

// 副手IK状态
UPROPERTY(Replicated, ReplicatedUsing = OnRep_OffhandIKIsLeftHand, BlueprintGetter = GetOffhandIKIsLeftHand, Category = "SKGShooterPawnComponent")
bool bOffhandIKIsLeftHand {true};
```

#### 核心属性配置

##### 网格和相机设置
```cpp
// 是否使用单一网格
UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
bool bUseSingleMesh {true};

// 第三人称网格组件名称
UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
FName ThirdPersonMeshComponentName {"CharacterMesh0"};

// 第一人称网格组件名称
UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
FName FirstPersonMeshComponentName {"MeshFP"};

// 相机组件名称
UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
FName CameraComponentName {"CameraComponent"};

// 相机附加插槽
UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
FName CameraAttachedSocket {"S_Camera"};
```

##### 动画层系统
```cpp
// 链接动画层类
UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
TSubclassOf<USKGShooterFrameworkAnimInstance> LinkedAnimLayerClass;

// 是否自动设置链接动画层
UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
bool bAutoSetupLinkedAnimLayer {true};
```

#### 事件系统

```cpp
// 姿态完成事件
UPROPERTY(BlueprintAssignable, Category = "SKGShooterPawnComponent|Events")
FOnPoseComplete OnPoseComplete;

// 持有Actor变化事件
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
```

### 技术实现细节

#### 网络架构

```cpp
// 服务器授权模式
UFUNCTION(Server, Reliable, WithValidation)
void Server_SetAiming(bool bAim);

UFUNCTION(Server, Reliable, WithValidation)
void Server_Lean(const uint8 TargetAngle);

UFUNCTION(Server, Reliable, WithValidation)
void Server_PerformProceduralPose(const FGameplayTag& Tag, bool bExitPose);

// 不可靠的自定义曲线同步
UFUNCTION(Server, Unreliable, WithValidation)
void Server_PerformCustomCurveUnreliable(const FSKGFirstAndThirdPersonCurveSettings& CurveData);
```

#### 性能优化

```cpp
// 统计组声明
DECLARE_STATS_GROUP(TEXT("SKGShooterPawnComponent"), STATGROUP_SKGShooterPawnComponent, STATCAT_Advanced);

// 使用压缩数据减少网络带宽
UPROPERTY(Replicated)
uint8 RemoteViewYaw {0};

// 使用弱指针避免循环引用
TWeakObjectPtr<USKGFirearmComponent> CurrentFirearmComponent;
```

#### 组件发现机制

```cpp
// 自动发现网格组件
void SetupComponents()
{
    // 查找第一人称网格
    MeshFP = FindComponentByName<USkeletalMeshComponent>(FirstPersonMeshComponentName);
    
    // 查找第三人称网格
    MeshTP = FindComponentByName<USkeletalMeshComponent>(ThirdPersonMeshComponentName);
    
    // 查找相机组件
    CameraComponent = FindComponentByName<UCameraComponent>(CameraComponentName);
}
```

## 模块层次结构

### 完整模块架构

```
SKGShooterFramework (核心框架)
├── SKGShooterPawnComponent (玩家交互中心)
├── USKGFirearmComponent (火器管理)
├── USKGProceduralAnimComponent (程序化动画)
├── USKGAttachmentManagerComponent (附件管理)
├── USKGOffhandIKComponent (副手IK)
└── 其他子组件...

SKGShooterFrameworkCore (基础服务)
├── ISKGInfraredInterface (红外接口)
├── USKGInfraredWorldSubsystem (红外管理)
├── USKGShooterFrameworkCoreEffectStatics (音效工具)
└── USKGShooterFrameworkDeveloperSettings (开发者设置)

SKGShooterFrameworkActors (预定义Actor)
├── ASKGBaseActor (基础Actor)
├── ASKGFirearm (标准火器)
├── ASKGOptic (瞄准镜)
├── ASKGMuzzle (枪口装置)
└── 其他专用Actor...

功能模块 (独立系统)
├── SKGAttachment (附件系统)
├── SKGProjectile (弹道系统)
├── SKGProceduralAnim (程序化动画)
├── SKGOptic (光学系统)
├── SKGMuzzle (枪口系统)
├── SKGLightLaser (激光系统)
├── SKGRangeFinder (测距系统)
├── SKGNightVision (夜视系统)
├── SKGThermal (热成像系统)
├── SKGStock (枪托系统)
├── SKGStats (统计系统)
└── SKGPhysicalMaterialModule (物理材质)
```

## 数据流架构

### 典型交互流程

```
玩家输入 → SKGShooterPawnComponent → USKGFirearmComponent → 各子系统
    ↓
SKGProceduralAnimComponent → 动画实例 → 渲染输出
    ↓
网络同步 → 服务器验证 → 状态复制 → 客户端更新
```

### SKGShooterPawnComponent 数据流

```
1. 玩家输入处理
   → SetMouseInput() → 灵敏度计算 → 视角更新

2. 武器持有管理
   → SetHeldActor() → 组件发现 → 程序化数据更新

3. 瞄准状态管理
   → StartAiming() → 网络同步 → 程序化动画更新

4. 自由视角系统
   → StartFreeLook() → 视角限制检查 → 控制旋转更新

5. 后坐力系统
   → PerformProceduralRecoil() → 动画实例 → 视觉效果
```

## 性能优化策略

### 1. 网络优化
- **数据压缩**: 使用uint8压缩角度数据
- **选择性复制**: 只在状态变化时进行网络同步
- **批量更新**: 合并多个小更新为批量操作

### 2. 内存管理
- **对象池**: 重用组件和动画实例
- **弱指针**: 使用TWeakObjectPtr避免循环引用
- **缓存机制**: 缓存频繁访问的组件引用

### 3. 计算优化
- **事件驱动**: 避免轮询，使用委托系统
- **LOD系统**: 基于距离的细节层次控制
- **异步处理**: 将非关键计算移至后台线程

## 扩展性设计

### 1. 接口驱动
```cpp
// 红外设备接口
class ISKGInfraredInterface
{
public:
    virtual bool IsInfraredModeOnForDevice() const = 0;
    virtual void OnInfraredEnabledForPlayer() = 0;
    virtual void OnInfraredDisabledForPlayer() = 0;
};
```

### 2. 数据驱动配置
```cpp
// 通过数据资产配置行为
UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
FSKGFreeLookSettings FreeLookSettings;

UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Leaning")
FSKGLeanLeftRightSettings LeanLeftRightSettings;
```

### 3. 蓝图可扩展
```cpp
// 蓝图可重写的灵敏度计算
UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGShooterPawnComponent|Sensitivity")
void BP_GetSensitivityMultiplier(const float X, const float XBaseTurnRate, const float Y, const float YBaseTurnRate, float& NewX, bool& AddYaw, float& NewY, bool& AddPitch) const;
```

## 最佳实践

### 1. 组件使用
```cpp
// 正确的组件获取方式
USKGShooterPawnComponent* PawnComponent = USKGShooterPawnComponent::GetShooterPawnComponent(Pawn);
if (PawnComponent)
{
    // 使用组件功能
    PawnComponent->StartAiming();
    PawnComponent->SetHeldActor(Weapon);
}
```

### 2. 网络同步
```cpp
// 服务器授权模式
if (PawnComponent->HasAuthority())
{
    // 服务器执行关键逻辑
    PawnComponent->Server_SetAiming(true);
}
```

### 3. 性能考虑
```cpp
// 避免频繁的组件查找
USKGFirearmComponent* FirearmComp = PawnComponent->GetCurrentFirearmComponent();
if (FirearmComp)
{
    // 缓存引用避免重复查找
    CachedFirearmComponent = FirearmComp;
}
```

## 常见问题解答

### Q: SKGShooterPawnComponent 不工作？
A: 确保正确添加到Pawn上，检查组件初始化顺序，验证网络权限设置

### Q: 瞄准状态不同步？
A: 确认服务器授权逻辑，检查网络复制设置，验证状态更新时机

### Q: 程序化动画不生效？
A: 检查动画层链接，验证数据资产配置，确认动画实例设置

### Q: 自由视角限制异常？
A: 调整FreeLookSettings参数，检查控制旋转设置，验证视角计算逻辑

### Q: 性能问题？
A: 使用统计组监控性能，优化网络更新频率，合理使用缓存机制

## 总结

SKGShooterFramework 提供了一个完整、模块化、高性能的射击游戏开发框架。通过USKGShooterPawnComponent作为核心交互接口，开发者可以轻松构建复杂的射击游戏体验。框架的设计充分考虑了真实感、可扩展性、网络性能和开发效率的平衡，是构建现代射击游戏的理想选择。

框架的模块化设计使得它既可以作为完整的射击游戏解决方案，也可以作为独立模块集成到现有项目中。其网络架构和性能优化策略确保了在多人游戏环境中的稳定性和可扩展性。

<system-reminder>Whenever you read the file, you should consider whether the user explicitly asked for a summary, analysis, or explanation. If you did, provide it. If not, continue with the task as requested.}