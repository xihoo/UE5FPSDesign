# SKGOptic 模块文档

## 模块概述

SKGOptic 模块提供了完整的光学瞄准镜系统，支持多种瞄准镜类型、可变倍率、分划板管理、归零调整等高级功能。该模块专为真实射击体验设计，模拟了现代光学瞄准镜的各种特性，包括倍率变化、分划板亮度调节、夜视模式等。

## 核心组件

### USKGOpticComponent

**类路径**: `Source/SKGOptic/Public/Components/SKGOpticComponent.h`

这是光学瞄准镜系统的核心组件，负责管理瞄准镜的所有功能和交互。

#### 主要功能
- **倍率管理**: 支持可变倍率瞄准镜，平滑缩放
- **分划板系统**: 多种分划板支持，亮度调节，夜视模式
- **归零调整**: 支持MOA和MRAD单位的精确归零
- **场景捕捉**: 高倍率瞄准镜的渲染优化
- **网络同步**: 完整的客户端-服务器状态同步
- **性能优化**: 智能的场景捕捉管理和渲染优化

#### 核心属性

##### 基础配置
```cpp
// 瞄准镜网格组件名称
FName OpticMeshName {"StaticMesh"};

// 场景捕捉组件名称（可选）
FName OpticSceneCaptureComponentName {"SKGOpticSceneCapture"};

// 是否为增倍镜
bool bIsMagnifier {false};
```

##### 数据资产
```cpp
// 分划板设置数据资产
UPROPERTY(EditDefaultsOnly, Category = "SKGOptic|Settings")
TObjectPtr<USKGPDAOpticReticleSettings> ReticleSettingsDataAsset;

// 倍率设置数据资产
UPROPERTY(EditDefaultsOnly, Category = "SKGOptic|Settings")
TObjectPtr<USKGPDAOpticMagnificationSettings> MagnificationSettingsDataAsset;

// 归零设置数据资产
UPROPERTY(EditDefaultsOnly, Category = "SKGOptic|Settings")
TObjectPtr<USKGPDAOpticZeroSettings> OpticZeroSettingsDataAsset;
```

#### 主要方法

##### 倍率控制
```cpp
// 放大
UFUNCTION(BlueprintCallable, Category = "SKGOptic|Zoom")
void ZoomIn();

// 缩小
UFUNCTION(BlueprintCallable, Category = "SKGOptic|Zoom")
void ZoomOut();

// 获取当前倍率
UFUNCTION(BlueprintPure, Category = "SKGOptic|Zoom")
float GetCurrentMagnification() const;

// 获取倍率灵敏度缩放
UFUNCTION(BlueprintPure, Category = "SKGOptic|Zoom")
float GetMagnificationSensitivityScale() const;

// 检查是否为倍率瞄准镜
UFUNCTION(BlueprintPure, Category = "SKGOptic")
bool IsMagnifiedOptic() const;
```

##### 分划板管理
```cpp
// 循环分划板
UFUNCTION(BlueprintCallable, Category = "SKGOptic|Reticle")
void CycleReticle();

// 增加分划板亮度
UFUNCTION(BlueprintCallable, Category = "SKGOptic|Reticle")
void IncreaseReticleBrightness();

// 降低分划板亮度
UFUNCTION(BlueprintCallable, Category = "SKGOptic|Reticle")
void DecreaseReticleBrightness();

// 切换夜视模式
UFUNCTION(BlueprintCallable, Category = "SKGOptic|Reticle")
void ToggleReticleNightVisionSetting();
```

##### 归零调整
```cpp
// 归零调整 - 上
UFUNCTION(BlueprintCallable, Category = "SKGOptic|Zero")
void PointOfImpactUp(const int32 Clicks = 1);

// 归零调整 - 下
UFUNCTION(BlueprintCallable, Category = "SKGOptic|Zero")
void PointOfImpactDown(const int32 Clicks = 1);

// 归零调整 - 左
UFUNCTION(BlueprintCallable, Category = "SKGOptic|Zero")
void PointOfImpactLeft(const int32 Clicks = 1);

// 归零调整 - 右
UFUNCTION(BlueprintCallable, Category = "SKGOptic|Zero")
void PointOfImpactRight(const int32 Clicks = 1);

// 重置上下归零
UFUNCTION(BlueprintCallable, Category = "SKGOptic|Zero")
int32 PointOfImpactUpDownDefault();

// 重置左右归零
UFUNCTION(BlueprintCallable, Category = "SKGOptic|Zero")
int32 PointOfImpactLeftRightDefault();
```

##### 瞄准状态
```cpp
// 开始瞄准
UFUNCTION(BlueprintCallable, Category = "SKGOptic")
void StartedAiming();

// 停止瞄准
UFUNCTION(BlueprintCallable, Category = "SKGOptic")
void StoppedAiming();
```

#### 事件委托

```cpp
// 场景捕捉状态变化
UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
FOnSceneCaptureStateChanged OnSceneCaptureStateChanged;

// 归零点变化
UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
FOnPointOfImpactChanged OnPointOfImpactChanged;

// 倍率变化
UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
FOnZoomChanged OnZoomChanged;

// 分划板循环
UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
FOnReticleCycled OnReticleCycled;

// 分划板亮度变化
UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
FOnReticleBrightnessChanged OnReticleBrightnessChanged;

// 夜视模式变化
UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
FOnNightVisionModeChanged OnNightVisionModeChanged;
```

### USKGOpticSceneCaptureComponent

**类路径**: `Source/SKGOptic/Public/Components/SKGOpticSceneCaptureComponent.h`

这是专门用于高倍率瞄准镜的场景捕捉组件，提供优化的渲染和倍率管理。

#### 主要功能
- **场景捕捉**: 高倍率瞄准镜的专用渲染
- **平滑缩放**: 支持平滑的倍率变化
- **眼框模拟**: 模拟真实瞄准镜的眼框效果
- **性能优化**: 智能的渲染目标管理

#### 核心配置结构

##### FSKGOpticSceneCaptureComponentSettings
```cpp
USTRUCT(BlueprintType)
struct FSKGOpticSceneCaptureComponentSettings
{
    GENERATED_BODY()
    
    // 渲染目标设置
    UPROPERTY(EditDefaultsOnly, Category = "SKGOpticSceneCaptureComponent")
    FSKGOpticSceneCaptureComponentRenderTargetSettings RenderTargetSettings;
    
    // 材质参数设置
    UPROPERTY(EditDefaultsOnly, Category = "SKGOpticSceneCaptureComponent")
    FSKGOpticSceneCaptureComponentMaterialSettings MaterialSettings;
};
```

## 数据资产系统

### USKGPDAOpticReticleSettings

**类路径**: `Source/SKGOptic/Public/DataAssets/SKGPDAOpticReticleSettings.h`

管理瞄准镜分划板设置的数据资产。

#### 核心结构

##### FSKGOpticReticle
```cpp
USTRUCT(BlueprintType)
struct FSKGOpticReticle
{
    GENERATED_BODY()
    
    // 分划板材质
    UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
    TObjectPtr<UMaterialInterface> ReticleMaterial;
    
    // 动态材质实例
    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> DynamicReticleMaterial;
    
    // 初始分划板大小参数
    float StartingReticleSizeParameterValue {1.0f};
    
    // 初始眼框范围参数
    float StartingEyeboxRangeParameterValue {-2000.0f};
};
```

##### FSKGOpticReticleSettings
```cpp
USTRUCT(BlueprintType)
struct FSKGOpticReticleSettings
{
    GENERATED_BODY()
    
    // 分划板材质索引
    UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
    int32 ReticleMaterialIndex {0};
    
    // 是否支持夜视
    UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
    bool bSupportsNightVision {true};
    
    // 分划板材质数组
    UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
    TArray<FSKGOpticReticle> ReticleMaterials;
    
    // 非瞄准时的分划板材质
    UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
    FSKGOpticReticle UnAimedReticleMaterial;
    
    // 停止瞄准后的捕捉延迟
    UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
    float UnAimedCaptureDelay {2.0f};
    
    // 分划板亮度参数名
    UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
    FName ReticleBrightnessParameterName {"ReticleBrightness"};
    
    // 默认模式亮度设置
    UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
    TArray<float> ReticleBrightnessSettings {1.0f};
    
    // 夜视模式亮度设置
    UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
    TArray<float> ReticleNightVisionBrightnessSettings;
    
    // 眼框位置插槽
    UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
    FName EyeboxPositionSocket {FName("S_Aim")};
};
```

### USKGPDAOpticMagnificationSettings

**类路径**: `Source/SKGOptic/Public/DataAssets/SKGPDAOpticMagnificationSettings.h`

管理瞄准镜倍率设置的数据资产。

#### 核心结构

##### FSKGOpticMagnificationSettings
```cpp
USTRUCT(BlueprintType)
struct FSKGOpticMagnificationSettings
{
    GENERATED_BODY()
    
    // 可用倍率数组
    UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
    TArray<float> Magnifications {1.0f};
    
    // 鼠标灵敏度缩放
    UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
    float SensitivityScale {1.0f};
    
    // 是否平滑缩放
    UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
    bool bSmoothZoom {true};
    
    // 平滑缩放速率
    UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
    float SmoothZoomRate {10.0f};
    
    // 是否随倍率缩小眼框
    UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
    bool bShrinkEyeboxWithMagnification {true};
    
    // 眼框缩小倍数
    UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
    float ShrinkEyeboxMultiplier {0.5f};
    
    // 是否为第一焦平面
    UPROPERTY(EditDefaultsOnly, Category = "SKGOpticSceneCaptureComponent")
    bool bIsFirstFocalPlane {true};
};
```

### USKGPDAOpticZeroSettings

**类路径**: `Source/SKGOptic/Public/DataAssets/SKGPDAOpticZeroSettings.h`

管理瞄准镜归零设置的数据资产。

#### 核心枚举

##### ESKGOpticAdjustmentType
```cpp
UENUM(BlueprintType)
enum class ESKGOpticAdjustmentType : uint8
{
    MOA     UMETA(DisplayName = "MOA"),     // 角分
    MRAD    UMETA(DisplayName = "MRAD")     // 毫弧度
};
```

##### ESKGMRADAdjustment
```cpp
UENUM(BlueprintType)
enum class ESKGMRADAdjustment : uint8
{
    PointOne    UMETA(DisplayName = ".1")   // 0.1 MRAD
};
```

##### ESKGMOAAdjustment
```cpp
UENUM(BlueprintType)
enum class ESKGMOAAdjustment : uint8
{
    OneEighth   UMETA(DisplayName = "1/8"),  // 1/8 MOA
    OneQuarter  UMETA(DisplayName = "1/4"),  // 1/4 MOA
    OneHalf     UMETA(DisplayName = "1/2"),  // 1/2 MOA
    One         UMETA(DisplayName = "1")     // 1 MOA
};
```

#### 核心结构

##### FSKGOpticStartingZeroSettings
```cpp
USTRUCT(BlueprintType)
struct FSKGOpticStartingZeroSettings
{
    GENERATED_BODY()
    
    // 是否使用默认归零
    UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent")
    bool bStartWithDefaultZero {false};
    
    // 默认仰角归零点击数
    UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent")
    int32 DefaultElevationClicks {25};
    
    // 默认风偏归零点击数
    UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent")
    int32 DefaultWindageClicks {25};
    
    // 是否使用随机归零
    UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent")
    bool bStartWithRandomZero {false};
    
    // 随机归零最大仰角点击数
    UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent")
    int32 RandomMaxElevationClicks {25};
    
    // 随机归零最大风偏点击数
    UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent")
    int32 RandomMaxWindageClicks {25};
};
```

##### FSKGOpticZeroSettings
```cpp
USTRUCT(BlueprintType)
struct FSKGOpticZeroSettings
{
    GENERATED_BODY()
    
    // 调整类型（MOA/MRAD）
    UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent")
    ESKGOpticAdjustmentType AdjustmentType {ESKGOpticAdjustmentType::MRAD};
    
    // MOA调整量
    UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent")
    ESKGMOAAdjustment MOAAdjustmentAmount = ESKGMOAAdjustment::OneQuarter;
    
    // MRAD调整量
    UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent")
    ESKGMRADAdjustment MRADAdjustmentAmount = ESKGMRADAdjustment::PointOne;
    
    // 起始归零设置
    UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent")
    FSKGOpticStartingZeroSettings StartingZeroSettings;
};
```

## 使用方法

### 基础设置

1. **添加组件**: 在武器 Actor 上添加 `USKGOpticComponent`
2. **配置数据资产**: 设置分划板、倍率和归零数据资产
3. **定义组件**: 配置瞄准镜网格和场景捕捉组件名称
4. **初始化**: 组件会自动初始化并应用设置

### 倍率控制

```cpp
// 获取瞄准镜组件
USKGOpticComponent* OpticComponent = Weapon->FindComponentByClass<USKGOpticComponent>();

// 检查是否为倍率瞄准镜
if (OpticComponent->IsMagnifiedOptic())
{
    // 放大
    OpticComponent->ZoomIn();
    
    // 获取当前倍率
    float CurrentZoom = OpticComponent->GetCurrentMagnification();
}
```

### 分划板管理

```cpp
// 循环分划板
OpticComponent->CycleReticle();

// 调节分划板亮度
OpticComponent->IncreaseReticleBrightness();
OpticComponent->DecreaseReticleBrightness();

// 切换夜视模式
OpticComponent->ToggleReticleNightVisionSetting();
```

### 归零调整

```cpp
// 归零调整
OpticComponent->PointOfImpactUp(5);    // 向上调整5个点击
OpticComponent->PointOfImpactDown(3);  // 向下调整3个点击
OpticComponent->PointOfImpactLeft(2);  // 向左调整2个点击
OpticComponent->PointOfImpactRight(4); // 向右调整4个点击

// 重置归零
int32 UpDownClicks = OpticComponent->PointOfImpactUpDownDefault();
int32 LeftRightClicks = OpticComponent->PointOfImpactLeftRightDefault();
```

### 事件监听

```cpp
// 绑定倍率变化事件
OpticComponent->OnZoomChanged.AddDynamic(this, &AMyWeapon::OnZoomChanged);

// 处理倍率变化
void AMyWeapon::OnZoomChanged(bool bZoomedIn)
{
    // 更新UI或调整灵敏度
    UpdateZoomIndicator(bZoomedIn);
}

// 绑定分划板变化事件
OpticComponent->OnReticleCycled.AddDynamic(this, &AMyWeapon::OnReticleCycled);

// 处理分划板变化
void AMyWeapon::OnReticleCycled(bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        // 更新分划板显示
        UpdateReticleDisplay();
    }
}
```

## 高级功能

### 场景捕捉管理

对于高倍率瞄准镜，组件会自动管理场景捕捉：

```cpp
// 开始瞄准（自动启动场景捕捉）
OpticComponent->StartedAiming();

// 停止瞄准（自动停止场景捕捉）
OpticComponent->StoppedAiming();
```

### 数据资产动态切换

```cpp
// 运行时切换分划板设置
OpticComponent->ReticleSettingsDataAsset = NewReticleSettings;
OpticComponent->UpdateOpticMaterialInstance();

// 运行时切换倍率设置
OpticComponent->MagnificationSettingsDataAsset = NewMagnificationSettings;
```

### 性能优化

组件提供了多种性能优化选项：

```cpp
// 网络更新优化
UPROPERTY(EditDefaultsOnly, Category = "SKGOptic|Settings")
bool bAutoCallForceNetUpdate {true};

// 场景捕捉优化
UPROPERTY(EditDefaultsOnly, Category = "SKGOpticSceneCaptureComponent")
float TickInterval {60.0f};  // 60秒更新间隔
```

## 技术细节

### 第一焦平面 vs 第二焦平面

组件支持两种焦平面类型：

```cpp
// 第一焦平面（FFP）- 分划板随倍率变化
bool bIsFirstFocalPlane {true};

// 第二焦平面（SFP）- 分划板固定大小
bool bIsFirstFocalPlane {false};
```

### 眼框模拟

通过材质参数模拟真实瞄准镜的眼框效果：

```cpp
// 眼框敏感度参数
FName EyeboxSensitivityParameterName {"EyeboxSensitivity"};

// 眼框位置插槽
FName EyeboxPositionSocket {FName("S_Aim")};
```

### 归零计算

支持MOA和MRAD两种归零单位：

```cpp
// MOA调整（1/4 MOA = 0.25英寸@100码）
ESKGMOAAdjustment MOAAdjustmentAmount = ESKGMOAAdjustment::OneQuarter;

// MRAD调整（0.1 MRAD = 1厘米@100米）
ESKGMRADAdjustment MRADAdjustmentAmount = ESKGMRADAdjustment::PointOne;
```

## 配置示例

### 标准红点瞄准镜

```cpp
// 基础配置
OpticMeshName = "RedDotMesh";
OpticSceneCaptureComponentName = "";  // 无场景捕捉

// 分划板设置
ReticleSettingsDataAsset = LoadObject<USKGPDAOpticReticleSettings>(...);

// 倍率设置（1x）
MagnificationSettingsDataAsset = LoadObject<USKGPDAOpticMagnificationSettings>(...);

// 归零设置
OpticZeroSettingsDataAsset = LoadObject<USKGPDAOpticZeroSettings>(...);
```

### 可变倍率瞄准镜

```cpp
// 基础配置
OpticMeshName = "ScopeMesh";
OpticSceneCaptureComponentName = "SKGOpticSceneCapture";

// 倍率设置（1-6x）
MagnificationSettingsDataAsset->MagnificationSettings.Magnifications = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
MagnificationSettingsDataAsset->MagnificationSettings.bSmoothZoom = true;
MagnificationSettingsDataAsset->MagnificationSettings.bIsFirstFocalPlane = true;
```

### 高精度狙击镜

```cpp
// 归零设置（0.1 MRAD精度）
OpticZeroSettingsDataAsset->OpticZeroSettings.AdjustmentType = ESKGOpticAdjustmentType::MRAD;
OpticZeroSettingsDataAsset->OpticZeroSettings.MRADAdjustmentAmount = ESKGMRADAdjustment::PointOne;

// 分划板设置（支持夜视）
ReticleSettingsDataAsset->ReticleSettings.bSupportsNightVision = true;
ReticleSettingsDataAsset->ReticleSettings.ReticleNightVisionBrightnessSettings = {0.1f, 0.2f, 0.3f};
```

## 最佳实践

1. **性能考虑**: 合理设置场景捕捉更新频率，避免过度渲染
2. **用户体验**: 提供清晰的倍率和分划板状态指示
3. **真实感**: 使用真实的MOA/MRAD归零单位
4. **兼容性**: 确保与程序化动画系统的正确集成
5. **网络优化**: 合理设置网络复制频率

## 常见问题

### Q: 分划板不显示？
A: 检查分划板材质索引和网格组件配置是否正确

### Q: 倍率变化不流畅？
A: 确保启用了平滑缩放并设置了合适的缩放速率

### Q: 归零调整无效？
A: 检查归零设置数据资产是否正确配置

### Q: 场景捕捉性能问题？
A: 优化渲染目标分辨率和更新频率

### Q: 网络同步问题？
A: 检查服务器授权和客户端复制设置

## 相关模块

- [SKGProceduralAnim 模块](SKGProceduralAnim模块.md) - 程序化动画系统
- [SKGAttachment 模块](SKGAttachment模块.md) - 附件管理系统
- [SKGShooterFrameworkCore 模块](SKGShooterFrameworkCore模块.md) - 核心框架功能
- [SKGNightVision 模块](SKGNightVision模块.md) - 夜视系统

## 扩展应用

### 特殊瞄准镜类型

```cpp
// 热成像瞄准镜
ThermalOptic->ReticleSettingsDataAsset = ThermalReticleSettings;
ThermalOptic->MagnificationSettingsDataAsset = ThermalMagnificationSettings;

// 全息瞄准镜
HolographicOptic->ReticleSettingsDataAsset = HolographicReticleSettings;
HolographicOptic->bIsMagnifier = false;

// 复合瞄准镜（红点+倍率）
HybridOptic->ReticleSettingsDataAsset = HybridReticleSettings;
HybridOptic->MagnificationSettingsDataAsset = HybridMagnificationSettings;
```

### 高级归零功能

```cpp
// 弹道计算器集成
void CalculateBallisticSolution(float Distance, float WindSpeed, float WindDirection)
{
    // 计算需要的仰角和风偏调整
    int32 ElevationClicks = CalculateElevationClicks(Distance);
    int32 WindageClicks = CalculateWindageClicks(WindSpeed, WindDirection);
    
    // 应用归零调整
    OpticComponent->PointOfImpactUp(ElevationClicks);
    OpticComponent->PointOfImpactRight(WindageClicks);
}
```

### 性能分析

内置性能统计：

```cpp
DECLARE_STATS_GROUP(TEXT("SKGOpticComponent"), STATGROUP_SKGOpticComponent, STATCAT_Advanced);
```

<system-reminder>Whenever you read a file, you should consider whether the user explicitly asked for a summary, analysis, or explanation. If they did, provide it. If not, continue with the task as requested.}</content>