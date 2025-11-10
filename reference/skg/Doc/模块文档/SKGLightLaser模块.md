# SKGLightLaser 模块文档

## 模块概述

SKGLightLaser 模块提供了完整的激光指示器和战术灯系统，支持多种工作模式、红外功能、以及网络同步。该模块专为现代射击游戏中的战术设备设计，提供了类似《逃离塔科夫》等游戏的光照设备体验。

## 核心组件

### USKGLightLaserComponent

**类路径**: `Source/SKGLightLaser/Public/Components/SKGLightLaserComponent.h`

这是激光指示器和战术灯系统的核心组件，统一管理激光和灯光功能。

#### 主要功能
- **激光指示**: 支持可见激光和红外激光
- **战术照明**: 支持普通照明和爆闪模式
- **红外模式**: 支持夜视设备兼容的红外功能
- **模式循环**: 类似塔科夫的设备模式切换
- **网络同步**: 完整的客户端-服务器状态同步
- **性能优化**: 高效的激光碰撞检测和渲染

#### 核心枚举类型

##### ESKGLaserMode
```cpp
UENUM(BlueprintType)
enum class ESKGLaserMode : uint8
{
    Off     UMETA(DisplayName = "Off"),      // 关闭
    On      UMETA(DisplayName = "On"),       // 开启
    MAX     UMETA(Hidden)
};
```

##### ESKGLightMode
```cpp
UENUM(BlueprintType)
enum class ESKGLightMode : uint8
{
    Off     UMETA(DisplayName = "Off"),      // 关闭
    On      UMETA(DisplayName = "On"),       // 开启
    Strobe  UMETA(DisplayName = "Strobe"),   // 爆闪
    MAX     UMETA(Hidden)
};
```

#### 核心属性

##### 组件配置
```cpp
// 光源组件名称
FName LightComponentName {"SpotLight"};

// 激光网格组件名称
FName LaserMeshComponentName {"LaserMesh"};

// 激光点组件名称
FName LaserDotComponentName {"LaserDot"};

// 是否支持红外模式
bool bHasInfraredMode {false};
```

##### 数据资产
```cpp
// 初始化设置数据资产
UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser|Initialize")
TObjectPtr<USKGPDALightLaserInitialize> InitializationSettingsDataAsset;

// 光照设置数据资产
UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser|Settings")
TObjectPtr<USKGPDALightLaserSettings> LightLaserSettingsDataAsset;
```

##### 状态管理
```cpp
// 激光状态
UPROPERTY(ReplicatedUsing = OnRep_LaserState)
FSKGLaserState LaserState;

// 光照状态
UPROPERTY(ReplicatedUsing = OnRep_LightState)
FSKGLightState LightState;

// 红外模式状态
UPROPERTY(ReplicatedUsing = OnRep_DeviceInfraredOn)
bool bDeviceInfraredOn {false};
```

#### 主要方法

##### 激光控制
```cpp
// 设置激光模式
UFUNCTION(BlueprintCallable, Category = "SKGLightLaser|Laser")
void SetLaserMode(ESKGLaserMode LaserMode);

// 获取当前激光模式
UFUNCTION(BlueprintPure, Category = "SKGLightLaser|Laser")
ESKGLaserMode GetLaserMode() const { return LaserState.LaserMode; }

// 检查激光是否开启
UFUNCTION(BlueprintPure, Category = "SKGLightLaser|Laser")
bool IsLaserOn() const { return LaserState.LaserMode != ESKGLaserMode::Off; }
```

##### 光照控制
```cpp
// 设置光照模式
UFUNCTION(BlueprintCallable, Category = "SKGLightLaser|Light")
void SetLightMode(ESKGLightMode LightMode);

// 获取当前光照模式
UFUNCTION(BlueprintPure, Category = "SKGLightLaser|Light")
ESKGLightMode GetLightMode() const { return LightState.LightMode; }

// 检查光照是否开启
UFUNCTION(BlueprintPure, Category = "SKGLightLaser|Light")
bool IsLightOn() const { return LightState.LightMode != ESKGLightMode::Off; }

// 循环光照强度
UFUNCTION(BlueprintCallable, Category = "SKGLightLaser|Light")
void CycleLightIntensity();
```

##### 红外模式
```cpp
// 设置红外模式
UFUNCTION(BlueprintCallable, Category = "SKGLightLaser|LightLaser")
void SetInfraredMode(bool InfraredModeOn);

// 检查是否处于红外模式
UFUNCTION(BlueprintPure, Category = "SKGLightLaser|LightLaser")
bool IsInInfraredMode() const { return bDeviceInfraredOn; }
```

##### 模式循环
```cpp
// 循环光照/激光模式
UFUNCTION(BlueprintCallable, Category = "SKGLightLaser|Laser")
void CycleLightLaserMode();

// 设置模式索引
UFUNCTION(BlueprintCallable, Category = "SKGLightLaser|Laser")
bool SetLightLaserCycleModeIndex(const int32 Index);

// 获取当前模式索引
UFUNCTION(BlueprintPure, Category = "SKGLightLaser|Laser")
int32 GetLightLaserCycleModeIndex() const { return LightLaserCycleModes.GetIndex(); }
```

#### 事件委托

```cpp
// 激光状态变化事件
UPROPERTY(BlueprintAssignable, Category = "SKGLightLaser|Events")
FOnLaserStateChanged OnLaserStateChanged;

// 光照模式变化事件
UPROPERTY(BlueprintAssignable, Category = "SKGLightLaser|Events")
FOnLightModeChanged OnLightModeChanged;

// 红外模式变化事件
UPROPERTY(BlueprintAssignable, Category = "SKGLightLaser|Events")
FOnInfraredModeChanged OnInfraredModeChanged;

// 光照爆闪事件
UPROPERTY(BlueprintAssignable, Category = "SKGLightLaser|Events")
FOnLightStrobed OnLightStrobed;

// 激光碰撞事件
UPROPERTY(BlueprintAssignable, Category = "SKGLightLaser|Events")
FOnLaserImpact OnLaserImpact;
```

### 红外接口支持

组件实现了 `ISKGInfraredInterface` 接口，支持与夜视系统集成：

```cpp
// 红外接口实现
virtual bool IsInfraredModeOnForDevice() const override { return bDeviceInfraredOn; }
virtual void OnInfraredEnabledForPlayer() override;
virtual void OnInfraredDisabledForPlayer() override;
```

## 使用方法

### 基础设置

1. **添加组件**: 在武器或设备 Actor 上添加 `USKGLightLaserComponent`
2. **配置数据资产**: 设置初始化和配置数据资产
3. **定义组件名称**: 配置光源、激光网格等组件名称
4. **启用红外支持**: 根据需要设置 `bHasInfraredMode`

### 激光控制

```cpp
// 获取激光组件
USKGLightLaserComponent* LightLaserComponent = Weapon->FindComponentByClass<USKGLightLaserComponent>();

// 开启激光
LightLaserComponent->SetLaserMode(ESKGLaserMode::On);

// 切换激光状态
if (LightLaserComponent->IsLaserOn())
{
    LightLaserComponent->SetLaserMode(ESKGLaserMode::Off);
}
else
{
    LightLaserComponent->SetLaserMode(ESKGLaserMode::On);
}
```

### 光照控制

```cpp
// 设置光照模式
LightLaserComponent->SetLightMode(ESKGLightMode::On);

// 开启爆闪模式
LightLaserComponent->SetLightMode(ESKGLightMode::Strobe);

// 循环光照强度
LightLaserComponent->CycleLightIntensity();
```

### 模式循环

```cpp
// 循环设备模式（类似塔科夫）
LightLaserComponent->CycleLightLaserMode();

// 获取当前模式
int32 CurrentMode = LightLaserComponent->GetLightLaserCycleModeIndex();
```

### 红外模式

```cpp
// 启用红外模式
LightLaserComponent->SetInfraredMode(true);

// 检查红外状态
if (LightLaserComponent->IsInInfraredMode())
{
    // 红外模式下的特殊处理
}
```

### 事件监听

```cpp
// 绑定激光状态变化事件
LightLaserComponent->OnLaserStateChanged.AddDynamic(this, &AMyWeapon::OnLaserStateChanged);

// 处理激光状态变化
void AMyWeapon::OnLaserStateChanged(ESKGLaserMode LaserMode)
{
    // 更新UI或播放音效
    UpdateLaserIndicator(LaserMode);
}

// 绑定光照模式变化事件
LightLaserComponent->OnLightModeChanged.AddDynamic(this, &AMyWeapon::OnLightModeChanged);

// 处理光照模式变化
void AMyWeapon::OnLightModeChanged(ESKGLightMode LightMode)
{
    // 更新光照效果
    UpdateLightEffects(LightMode);
}
```

## 高级功能

### 激光碰撞检测

组件支持激光碰撞检测，可以用于瞄准辅助或交互系统：

```cpp
// 绑定激光碰撞事件
LightLaserComponent->OnLaserImpact.AddDynamic(this, &AMyWeapon::OnLaserImpact);

// 处理激光碰撞
void AMyWeapon::OnLaserImpact(const FHitResult& HitResult, bool bHit)
{
    if (bHit)
    {
        // 激光击中物体
        ShowLaserHitEffect(HitResult.Location);
    }
}
```

### 数据资产配置

通过数据资产可以详细配置设备行为：

#### 初始化数据资产 (USKGPDALightLaserInitialize)
- 组件引用配置
- 默认状态设置
- 网络优化选项

#### 设置数据资产 (USKGPDALightLaserSettings)
- 激光参数（颜色、强度、范围）
- 光照参数（亮度、范围、角度）
- 模式循环配置
- 红外模式设置

### 网络同步

所有关键状态都支持网络同步：

```cpp
// 服务器设置激光模式
UFUNCTION(Server, Reliable, WithValidation)
void Server_SetLaserMode(ESKGLaserMode LaserMode);

// 服务器设置光照模式
UFUNCTION(Server, Reliable, WithValidation)
void Server_SetLightMode(ESKGLightMode LightMode);

// 服务器设置红外模式
UFUNCTION(Server, Reliable, WithValidation)
void Server_SetInfraredMode(bool bInfraredOn);
```

## 技术细节

### 组件架构

```cpp
UActorComponent
└── IGameplayTagAssetInterface
    └── ISKGInfraredInterface
        └── USKGLightLaserComponent
```

### 状态管理

组件使用结构体来管理复杂状态：

```cpp
USTRUCT()
struct FSKGLaserState
{
    GENERATED_BODY()
    UPROPERTY()
    ESKGLaserMode LaserMode;
    // 其他激光相关状态
};

USTRUCT()
struct FSKGLightState
{
    GENERATED_BODY()
    UPROPERTY()
    ESKGLightMode LightMode;
    UPROPERTY()
    int32 LightIntensityIndex;
    // 其他光照相关状态
};
```

### 性能优化

- **高效渲染**: 使用静态网格组件进行激光渲染
- **智能更新**: 只在状态变化时更新视觉效果
- **网络优化**: 支持自定义网络更新频率

## 配置示例

### 标准战术灯配置

```cpp
// 组件配置
LightComponentName = "SpotLight";
LaserMeshComponentName = "LaserMesh";
LaserDotComponentName = "LaserDot";
bHasInfraredMode = true;

// 数据资产配置
InitializationSettingsDataAsset = LoadObject<USKGPDALightLaserInitialize>(...);
LightLaserSettingsDataAsset = LoadObject<USKGPDALightLaserSettings>(...);
```

### 模式循环配置

通过数据资产配置可用的模式循环：

```cpp
// 模式1: 全部关闭
// 模式2: 仅激光
// 模式3: 仅光照
// 模式4: 激光+光照
// 模式5: 激光+光照爆闪
```

## 最佳实践

1. **性能考虑**: 合理设置激光更新频率，避免过度使用激光碰撞检测
2. **用户体验**: 提供清晰的模式指示和切换反馈
3. **网络优化**: 合理设置网络更新频率，避免状态同步问题
4. **兼容性**: 确保与夜视系统的正确集成
5. **视觉效果**: 使用合适的光照强度和激光颜色

## 常见问题

### Q: 激光不显示？
A: 检查激光网格组件和数据资产配置是否正确

### Q: 红外模式不工作？
A: 确保启用了红外支持并正确集成了夜视系统

### Q: 网络同步问题？
A: 检查服务器授权和客户端复制设置

### Q: 性能问题？
A: 优化激光碰撞检测频率和光照更新频率

## 相关模块

- [SKGOptic 模块](SKGOptic模块.md) - 光学瞄准镜系统
- [SKGNightVision 模块](SKGNightVision模块.md) - 夜视系统
- [SKGAttachment 模块](SKGAttachment模块.md) - 附件管理系统
- [SKGShooterFrameworkCore 模块](SKGShooterFrameworkCore模块.md) - 核心框架功能