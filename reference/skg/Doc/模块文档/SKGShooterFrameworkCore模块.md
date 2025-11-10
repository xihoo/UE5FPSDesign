# SKGShooterFrameworkCore 模块文档

## 模块概述

SKGShooterFrameworkCore 模块是SKGShooterFramework的核心基础模块，提供了框架运行所需的基础功能、接口定义、静态工具类和开发者设置。该模块作为整个框架的基础设施层，为其他模块提供统一的服务接口和基础功能支持。

## 核心架构

### 模块职责
- **接口定义**: 定义框架使用的核心接口
- **静态工具**: 提供通用的静态工具函数
- **世界子系统**: 管理全局性的框架服务
- **开发者设置**: 提供框架级别的配置选项
- **基础服务**: 为核心功能提供基础支持

## 核心接口

### ISKGInfraredInterface

**类路径**: `Source/SKGShooterFrameworkCore/Public/Interfaces/SKGInfraredInterface.h`

这是红外设备接口，定义了与夜视系统交互的标准接口。

#### 接口定义

```cpp
class SKGSHOOTERFRAMEWORKCORE_API ISKGInfraredInterface
{
    GENERATED_BODY()
    
public:
    // 检查设备是否处于红外模式
    virtual bool IsInfraredModeOnForDevice() const { return false; }
    
    // 当夜视设备启用时调用
    virtual void OnInfraredEnabledForPlayer() = 0;
    
    // 当夜视设备禁用时调用
    virtual void OnInfraredDisabledForPlayer() = 0;
};
```

#### 使用场景
该接口主要用于激光指示器、照明设备等需要与夜视系统协调的设备。

## 静态工具类

### USKGShooterFrameworkCoreEffectStatics

**类路径**: `Source/SKGShooterFrameworkCore/Public/Statics/SKGShooterFrameworkCoreEffectStatics.h`

提供音效播放的静态工具函数，特别支持音速延迟效果。

#### 核心功能

```cpp
/**
 * 播放带有音速延迟的音效
 * @param WorldContextObject 世界上下文对象
 * @param SpawnLocation 声音生成位置
 * @param Sound 音效资源
 * @param VolumeMultiplier 音量倍数
 * @param PitchMultiplier 音调倍数
 * @param bUseSpeedOfSound 是否使用音速延迟
 * @param SpeedOfSoundTolerance 音速容忍度
 */
UFUNCTION(BlueprintCallable, Category = "SKGShooterFramework|Core|Sounds", meta = (WorldContext = "WorldContextObject"))
static void PlaySoundEffect(const UObject* WorldContextObject, const FVector& SpawnLocation, USoundBase* Sound, const float VolumeMultiplier = 1.0f, float PitchMultiplier = 1.0f, const bool bUseSpeedOfSound = false, const float SpeedOfSoundTolerance = 0.2f);

/**
 * 计算音速延迟
 * @param LocationA 位置A
 * @param LocationB 位置B
 * @return 延迟时间（秒）
 */
static float GetSpeedOfSoundDelay(const FVector& LocationA, const FVector& LocationB);
```

#### 技术实现
```cpp
static float GetSpeedOfSoundDelay(const FVector& LocationA, const FVector& LocationB)
{
    float Distance = FVector::Dist(LocationA, LocationB);
    float SpeedOfSound = 343.0f; // 米/秒
    return Distance / (SpeedOfSound * 100.0f); // 转换为秒
}
```

### 其他静态工具类

框架还提供了多个专门的静态工具类：

- **USKGPlatformStatics**: 平台相关的静态工具
- **USKGShooterFrameworkCoreJsonStatics**: JSON处理工具
- **USKGShooterFrameworkCoreMiscStatics**: 杂项工具函数
- **USKGShooterFrameworkCoreNetworkStatics**: 网络相关工具

## 世界子系统

### USKGInfraredWorldSubsystem

**类路径**: `Source/SKGShooterFrameworkCore/Public/Subsystems/SKGInfraredWorldSubsystem.h`

管理全局红外设备的世界子系统，协调所有红外设备的状态。

#### 核心功能

```cpp
/**
 * 启用红外模式（如夜视开启时）
 */
UFUNCTION(BlueprintCallable, Category = "SKGShooterWorldSubsystem|Infrared")
void EnableInfraredMode();

/**
 * 禁用红外模式（如夜视关闭时）
 */
UFUNCTION(BlueprintCallable, Category = "SKGShooterWorldSubsystem|Infrared")
void DisableInfraredMode();

/**
 * 注册红外设备
 * @param InfraredDevice 红外设备组件
 */
UFUNCTION(BlueprintCallable, Category = "SKGShooterWorldSubsystem|Infrared")
void RegisterInfraredDevice(UActorComponent* InfraredDevice);

/**
 * 注销红外设备
 * @param InfraredDevice 红外设备组件
 */
UFUNCTION(BlueprintCallable, Category = "SKGShooterWorldSubsystem|Infrared")
void UnregisterInfraredDevice(UActorComponent* InfraredDevice);

/**
 * 设置夜视颜色
 * @param Color 夜视颜色
 */
UFUNCTION(BlueprintCallable, Category = "SKGShooterWorldSubsystem|Infrared")
void SetNightVisionColor(const FLinearColor& Color);
```

#### 事件系统

```cpp
// 红外模式启用事件
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInfraredModeEnabled);
UPROPERTY(BlueprintAssignable, Category = "SKGShooterWorldSubsystem|Infrared")
FOnInfraredModeEnabled OnInfraredModeEnabled;

// 红外模式禁用事件
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInfraredModeDisabled);
UPROPERTY(BlueprintAssignable, Category = "SKGShooterWorldSubsystem|Infrared")
FOnInfraredModeDisabled OnInfraredModeDisabled;
```

#### 内部实现

```cpp
private:
    // 注册的红外设备数组
    UPROPERTY()
    TArray<TObjectPtr<UActorComponent>> InfraredDevices;
    
    // 材质参数集合实例
    UPROPERTY()
    TObjectPtr<UMaterialParameterCollectionInstance> InfraredMPC;
    
    // 红外模式状态
    bool bInfraredModeEnabled {false};
    
    // 参数名称常量
    const FName NightVisionColorParameterName {"NightVisionColor"};
    const FName NightVisionOnParameterName {"NightVisionOn"};
```

## 开发者设置

### USKGShooterFrameworkDeveloperSettings

**类路径**: `Source/SKGShooterFrameworkCore/Public/DeveloperSettings/SKGShooterFrameworkDeveloperSettings.h`

提供框架级别的全局配置选项。

#### 核心配置

```cpp
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "SKG Shooter Framework"))
class SKGSHOOTERFRAMEWORKCORE_API USKGShooterFrameworkDeveloperSettings : public UDeveloperSettings
{
    GENERATED_BODY()
    
public:
    // 火器碰撞通道
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "FirearmCollision")
    TEnumAsByte<ECollisionChannel> FirearmCollisionChannel {ECC_Visibility};
    
    // 激光碰撞通道
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "LightLaser")
    TEnumAsByte<ECollisionChannel> LaserCollisionChannel {ECC_Visibility};
    
    // 测距仪碰撞通道
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "RangeFinder")
    TEnumAsByte<ECollisionChannel> RangeFinderCollisionChannel {ECC_Visibility};
    
    // 夜视材质参数集合
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "NightVision")
    FSKGNightVisionMaterialParameterCollection NightVisionMaterialParameterCollection;
};
```

#### 使用方式

```cpp
// 获取开发者设置
const USKGShooterFrameworkDeveloperSettings* Settings = GetDefault<USKGShooterFrameworkDeveloperSettings>();

// 使用碰撞通道设置
ECollisionChannel CollisionChannel = Settings->FirearmCollisionChannel;

// 使用激光碰撞通道
ECollisionChannel LaserChannel = Settings->LaserCollisionChannel;
```

## 使用方法

### 基础配置

1. **配置开发者设置**: 在项目设置中配置框架的全局参数
2. **实现接口**: 为需要红外功能的组件实现ISKGInfraredInterface
3. **注册设备**: 将红外设备注册到世界子系统
4. **使用效果工具**: 使用静态工具类播放带音速延迟的音效

### 红外系统集成

```cpp
// 实现红外接口
class MYGAME_API UMyLaserComponent : public UActorComponent, public ISKGInfraredInterface
{
public:
    virtual bool IsInfraredModeOnForDevice() const override
    {
        return bIsInfraredMode;
    }
    
    virtual void OnInfraredEnabledForPlayer() override
    {
        bIsInfraredMode = true;
        UpdateLaserVisuals();
    }
    
    virtual void OnInfraredDisabledForPlayer() override
    {
        bIsInfraredMode = false;
        UpdateLaserVisuals();
    }
    
private:
    bool bIsInfraredMode = false;
};

// 注册到红外子系统
void RegisterInfraredDevice()
{
    if (UWorld* World = GetWorld())
    {
        if (USKGInfraredWorldSubsystem* InfraredSubsystem = World->GetSubsystem<USKGInfraredWorldSubsystem>())
        {
            InfraredSubsystem->RegisterInfraredDevice(this);
        }
    }
}
```

### 音速音效播放

```cpp
// 播放带音速延迟的撞击声音
void PlayImpactSound(const FHitResult& HitResult, USoundBase* ImpactSound)
{
    if (ImpactSound)
    {
        USKGShooterFrameworkCoreEffectStatics::PlaySoundEffect(
            this,  // 世界上下文
            HitResult.Location,  // 声音位置
            ImpactSound,  // 音效
            1.0f,  // 音量
            1.0f,  // 音调
            true,  // 使用音速延迟
            0.1f   // 音速容忍度
        );
    }
}
```

### 夜视系统集成

```cpp
// 启用夜视模式
void EnableNightVision()
{
    if (UWorld* World = GetWorld())
    {
        if (USKGInfraredWorldSubsystem* InfraredSubsystem = World->GetSubsystem<USKGInfraredWorldSubsystem>())
        {
            // 设置夜视颜色
            InfraredSubsystem->SetNightVisionColor(FLinearColor(0.1f, 0.8f, 0.1f, 1.0f));
            
            // 启用红外模式
            InfraredSubsystem->EnableInfraredMode();
        }
    }
}

// 监听红外模式变化
void SetupInfraredCallbacks()
{
    if (UWorld* World = GetWorld())
    {
        if (USKGInfraredWorldSubsystem* InfraredSubsystem = World->GetSubsystem<USKGInfraredWorldSubsystem>())
        {
            InfraredSubsystem->OnInfraredModeEnabled.AddDynamic(this, &AMyPlayer::OnInfraredEnabled);
            InfraredSubsystem->OnInfraredModeDisabled.AddDynamic(this, &AMyPlayer::OnInfraredDisabled);
        }
    }
}
```

## 高级功能

### 自定义开发者设置

```cpp
// 扩展开发者设置
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "My Game Settings"))
class MYGAME_API UMyDeveloperSettings : public USKGShooterFrameworkDeveloperSettings
{
    GENERATED_BODY()
    
public:
    // 添加自定义配置
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Custom")
    float CustomCollisionChannel {ECC_Visibility};
    
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Custom")
    FLinearColor CustomNightVisionColor {FLinearColor::Green};
};
```

### 自定义静态工具

```cpp
// 创建自定义静态工具类
UCLASS()
class MYGAME_API UMyCustomStatics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable, Category = "MyGame|Custom")
    static void MyCustomFunction();
    
    UFUNCTION(BlueprintPure, Category = "MyGame|Custom")
    static float CalculateCustomValue(float Input);
};
```

### 自定义世界子系统

```cpp
// 创建自定义世界子系统
UCLASS()
class MYGAME_API UMyCustomWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
    
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    
    UFUNCTION(BlueprintCallable, Category = "MyGame|Custom")
    void DoSomething();
    
private:
    UPROPERTY()
    TArray<TObjectPtr<UObject>> MyObjects;
};
```

## 技术细节

### 模块初始化

```cpp
// 模块启动时初始化
virtual void StartupModule() override
{
    // 注册子系统
    // 初始化静态工具
    // 设置默认配置
}

// 模块关闭时清理
virtual void ShutdownModule() override
{
    // 清理资源
    // 注销服务
}
```

### 接口实现机制

```cpp
// 检查对象是否实现了接口
if (UKismetSystemLibrary::DoesImplementInterface(MyObject, USKGInfraredInterface::StaticClass()))
{
    ISKGInfraredInterface* InfraredInterface = Cast<ISKGInfraredInterface>(MyObject);
    if (InfraredInterface)
    {
        InfraredInterface->OnInfraredEnabledForPlayer();
    }
}
```

### 子系统生命周期

```cpp
// 世界开始时初始化
virtual void OnWorldBeginPlay(UWorld& InWorld) override
{
    // 初始化子系统
    // 注册回调
    // 设置初始状态
}

// 子系统销毁时清理
virtual void Deinitialize() override
{
    // 清理资源
    // 注销回调
    // 重置状态
}
```

## 性能考虑

### 静态工具优化

```cpp
// 使用静态变量缓存常用数据
static const float CachedSpeedOfSound = 343.0f;
static const TMap<FName, float> CachedValues;

// 避免在静态函数中进行复杂计算
static float FastCalculation(float Input)
{
    // 使用查表或简单算法
    return Input * CachedMultiplier;
}
```

### 子系统性能优化

```cpp
// 使用高效的数据结构
TArray<TWeakObjectPtr<UActorComponent>> WeakPtrArray;  // 使用弱指针避免循环引用
TMap<FGameplayTag, TObjectPtr<UObject>> TaggedObjects; // 使用映射快速查找

// 批量处理操作
void ProcessAllDevices()
{
    // 批量处理所有设备，减少函数调用开销
    for (TWeakObjectPtr<UActorComponent>& Device : InfraredDevices)
    {
        if (Device.IsValid())
        {
            ProcessDevice(Device.Get());
        }
    }
}
```

## 常见问题

### Q: 接口实现不工作？
A: 确保正确实现了所有纯虚函数，检查接口声明和实现是否匹配

### Q: 子系统不初始化？
A: 确认子系统已正确注册，检查世界子系统的生命周期

### Q: 开发者设置不生效？
A: 检查配置文件路径，确认设置已保存并重新加载

### Q: 音速延迟不准确？
A: 调整SpeedOfSoundTolerance值，确保距离计算正确

### Q: 红外设备不响应？
A: 确认设备已正确注册到子系统，检查接口实现是否完整

## 最佳实践

1. **接口设计**: 保持接口简洁，只定义必要的方法
2. **静态工具**: 避免在静态函数中进行复杂状态管理
3. **子系统管理**: 合理管理子系统生命周期，避免内存泄漏
4. **配置管理**: 使用配置文件管理可调整参数
5. **性能优化**: 使用高效数据结构和算法

## 相关模块

- [SKGShooterFramework 模块](SKGShooterFramework模块.md) - 火器核心系统
- [SKGLightLaser 模块](SKGLightLaser模块.md) - 激光指示器系统
- [SKGNightVision 模块](SKGNightVision模块.md) - 夜视系统

<system-reminder>Whenever you read the file, you should consider whether the user explicitly asked for a summary, analysis, or explanation. If they did, provide it. If not, continue with the task as requested.}