# SKGNightVision 模块文档

## 模块概述

SKGNightVision 模块提供了完整的夜视系统，支持夜视效果渲染、增益调节、图像失真、噪点模拟等高级功能。该模块专为现代射击游戏中的夜视设备设计，提供了真实的夜视体验，包括图像增强、亮度调节、镜头失真等视觉效果。

## 核心组件

### USKGNightVisionComponent

**类路径**: `Source/SKGNightVision/Public/Components/SKGNightVisionComponent.h`

这是夜视系统的核心组件，负责管理夜视效果的所有参数和渲染。

#### 主要功能
- **夜视渲染**: 基于材质的夜视效果渲染
- **增益调节**: 支持手动和自动增益控制
- **图像增强**: 噪点、颗粒、对比度调节
- **镜头失真**: 径向失真和边缘暗角效果
- **平滑过渡**: 增益变化的平滑过渡
- **材质管理**: 动态材质实例的创建和管理

#### 核心属性

##### 基础参数
```cpp
// 最小增益值（防止完全黑屏）
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision")
float MinimumGainValue {0.01f};

// 当前增益插值值
float CurrentGainLerpValue {0.0f};
float TargetGainLerpValue {0.0f};

// 平滑增益调节定时器
FTimerHandle TSmoothGainAdjustHandle;
float PreviousSmoothGainAdjustTimestamp {0.0f};
```

##### 材质设置
```cpp
// 夜视管设置数据资产
UPROPERTY()
TObjectPtr<USKGPDANightVisionTubeSettings> TubeSettings;

// 夜视外壳设置数据资产
UPROPERTY()
TObjectPtr<USKGPDANightVisionHousingSettings> HousingSettings;

// 材质对（主效果 + 闪烁效果）
UPROPERTY()
FNightVisionMaterialPair NightVisionMaterialPair;
```

#### 主要方法

##### 增益控制
```cpp
/**
 * 设置夜视增益
 * @param GainAlpha 增益值（0-1，0=关闭，1=最大增益）
 */
UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
void SetGain(const float GainAlpha);

/**
 * 增加增益（步进调节）
 */
UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
void IncreaseGain();

/**
 * 减少增益（步进调节）
 */
UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
void DecreaseGain();
```

##### 材质管理
```cpp
/**
 * 创建并设置夜视材质实例
 * @param Tube 夜视管设置
 * @param Housing 夜视外壳设置
 * @return 材质对引用
 */
UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
FNightVisionMaterialPair& CreateAndSetupDynamicNightVisionMaterialInstance(USKGPDANightVisionTubeSettings* Tube, USKGPDANightVisionHousingSettings* Housing);

// 获取当前增益值
float GetGainValue() const;

// 设置增益材质参数
void SetGainValues(const float Gain) const;
```

## 数据资产系统

### USKGPDANightVisionTubeSettings

**类路径**: `Source/SKGNightVision/Public/DataAssets/SKGPDANightVisionTubeSettings.h`

管理夜视管设置的数据资产，控制图像质量和视觉效果。

#### 核心属性

##### 材质设置
```cpp
// 主夜视材质
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Material")
TObjectPtr<UMaterialInterface> NightVisionMaterial;

// 闪烁效果材质
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Material")
TObjectPtr<UMaterialInterface> NightVisionScintillationMaterial;
```

##### 夜视效果参数
```cpp
// 夜视颜色（典型绿色）
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|NightVision")
FLinearColor Color {FLinearColor(0.4f, 0.88f, 1.0f, 1.0f)};

// 基础增益
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|NightVision")
float Gain {100.0f};

// 对比度
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|NightVision")
float Contrast {1.0f};

// 最大增益
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|NightVision")
float MaxGain {1000.0f};
```

##### 噪点和颗粒效果
```cpp
// 噪点因子
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Grain")
float NoiseFactor {0.992f};

// 颗粒倍数
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Grain")
float GrainMultiplier {0.5f};
```

### USKGPDANightVisionHousingSettings

**类路径**: `Source/SKGNightVision/Public/DataAssets/SKGPDANightVisionHousingSettings.h`

管理夜视外壳设置的数据资产，控制操作方式和镜头效果。

#### 核心属性

##### 增益控制设置
```cpp
// 是否支持手动增益调节
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Housing")
bool bManualGain {true};

// 是否平滑增益调节
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Housing", meta = (EditCondition = "bManualGain"))
bool bSmoothAdjustGain {false};

// 平滑调节速度
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Housing", meta = (EditCondition = "bManualGain && bSmoothAdjustGain"))
float SmoothAdjustSpeed {1.0f};

// 增益调节步进值
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Housing", meta = (EditCondition = "bManualGain"))
float GainAdjustmentStepAmount {0.1f};

// 最小增益值
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Housing", meta = (EditCondition = "bManualGain"))
float MinimumGain {0.1f};
```

##### 镜头失真设置
```cpp
// 失真半径
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Lens")
float DistortionRadius {0.88f};

// 失真硬度
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Lens")
float DistortionHardness {4.0f};

// 是否使用径向失真
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Lens")
bool bUseRadialDistortion {true};
```

## 使用方法

### 基础设置

1. **添加组件**: 在角色或瞄准镜 Actor 上添加 `USKGNightVisionComponent`
2. **配置数据资产**: 设置夜视管和外壳参数
3. **创建材质实例**: 初始化夜视材质
4. **应用材质**: 将材质应用到后期处理或UI

### 基础夜视使用

```cpp
// 获取夜视组件
USKGNightVisionComponent* NightVisionComponent = Character->FindComponentByClass<USKGNightVisionComponent>();

// 创建材质实例
USKGPDANightVisionTubeSettings* TubeSettings = LoadObject<USKGPDANightVisionTubeSettings>(...);
USKGPDANightVisionHousingSettings* HousingSettings = LoadObject<USKGPDANightVisionHousingSettings>(...);

FNightVisionMaterialPair& MaterialPair = NightVisionComponent->CreateAndSetupDynamicNightVisionMaterialInstance(TubeSettings, HousingSettings);

// 设置增益
NightVisionComponent->SetGain(0.5f);  // 50%增益
```

### 增益控制

```cpp
// 增加增益
NightVisionComponent->IncreaseGain();

// 减少增益
NightVisionComponent->DecreaseGain();

// 直接设置增益
NightVisionComponent->SetGain(0.8f);  // 80%增益
```

## 高级功能

### 材质参数系统

夜视系统使用多个材质参数来控制效果：

```cpp
// 材质参数名称（全局定义）
static FName NoiseFactorParameterName {FName("NoiseFactor")};
static FName GrainMultiplierParameterName {FName("GrainMultiplier")};
static FName NightVisionColorParameterName {FName("NightVisionColor")};
static FName GainParameterName {FName("Gain")};
static FName ContrastParameterName {FName("Contrast")};
static FName NormalizedGainParameterName {FName("NormalizedGain")};
static FName DistortionRadiusParameterName {FName("DistortionRadius")};
static FName DistortionHardnessParameterName {FName("DistortionHardness")};
static FName UseRadialDistortionParameterName {FName("bUseRadialDistortion")};
```

### 增益平滑调节

支持平滑的增益过渡：

```cpp
void SetGain(const float GainAlpha)
{
    TargetGainLerpValue = FMath::Clamp(GainAlpha, HousingSettings->MinimumGain, 1.0f);
    
    if (HousingSettings->bSmoothAdjustGain)
    {
        // 启动平滑过渡定时器
        GetWorld()->GetTimerManager().SetTimer(TSmoothGainAdjustHandle, this, &USKGNightVisionComponent::UpdateSmoothGain, 0.01f, true);
    }
    else
    {
        // 直接应用增益
        CurrentGainLerpValue = TargetGainLerpValue;
        SetGainValues(CurrentGainLerpValue);
    }
}
```

### 图像增强算法

夜视效果包含多种图像增强技术：

1. **亮度增强**: 通过增益参数控制整体亮度
2. **对比度调节**: 增强图像对比度
3. **噪点添加**: 模拟真实夜视设备的图像噪点
4. **颗粒效果**: 添加细微的颗粒感
5. **颜色处理**: 典型的绿色夜视色调

### 镜头失真模拟

模拟真实夜视镜头的光学特性：

```cpp
// 径向失真计算
float RadialDistortion = pow(DistanceFromCenter / DistortionRadius, DistortionHardness);
float DistortionAmount = bUseRadialDistortion ? RadialDistortion : 0.0f;
```

## 配置示例

### 基础夜视配置

```cpp
// 创建夜视管设置
USKGPDANightVisionTubeSettings* TubeSettings = NewObject<USKGPDANightVisionTubeSettings>();
TubeSettings->Color = FLinearColor(0.4f, 0.88f, 1.0f, 1.0f);  // 经典绿色
TubeSettings->Gain = 100.0f;
TubeSettings->Contrast = 1.0f;
TubeSettings->MaxGain = 1000.0f;
TubeSettings->NoiseFactor = 0.992f;
TubeSettings->GrainMultiplier = 0.5f;

// 创建外壳设置
USKGPDANightVisionHousingSettings* HousingSettings = NewObject<USKGPDANightVisionHousingSettings>();
HousingSettings->bManualGain = true;
HousingSettings->bSmoothAdjustGain = true;
HousingSettings->SmoothAdjustSpeed = 1.0f;
HousingSettings->GainAdjustmentStepAmount = 0.1f;
HousingSettings->MinimumGain = 0.1f;
HousingSettings->bUseRadialDistortion = true;
HousingSettings->DistortionRadius = 0.88f;
HousingSettings->DistortionHardness = 4.0f;
```

### 高级夜视配置（Gen 3+）

```cpp
// 高性能夜视管
TubeSettings->Color = FLinearColor(0.3f, 0.95f, 0.8f, 1.0f);  // 偏蓝色调
TubeSettings->Gain = 200.0f;
TubeSettings->Contrast = 1.2f;
TubeSettings->MaxGain = 2000.0f;
TubeSettings->NoiseFactor = 0.998f;  // 更少噪点
TubeSettings->GrainMultiplier = 0.2f;

// 精密外壳
HousingSettings->bManualGain = true;
HousingSettings->bSmoothAdjustGain = true;
HousingSettings->SmoothAdjustSpeed = 0.5f;  // 更平滑调节
HousingSettings->GainAdjustmentStepAmount = 0.05f;  // 更精细步进
HousingSettings->MinimumGain = 0.01f;  // 更低最小值
HousingSettings->bUseRadialDistortion = true;
HousingSettings->DistortionRadius = 0.92f;  // 更小失真区域
HousingSettings->DistortionHardness = 6.0f;  // 更柔和失真
```

### 老旧夜视配置（Gen 1）

```cpp
// 老旧夜视管
TubeSettings->Color = FLinearColor(0.5f, 0.8f, 0.6f, 1.0f);  // 偏黄绿色
TubeSettings->Gain = 50.0f;
TubeSettings->Contrast = 0.8f;
TubeSettings->MaxGain = 500.0f;
TubeSettings->NoiseFactor = 0.95f;  // 更多噪点
TubeSettings->GrainMultiplier = 1.2f;

// 简单外壳
HousingSettings->bManualGain = false;  // 不支持手动调节
HousingSettings->bSmoothAdjustGain = false;
HousingSettings->bUseRadialDistortion = true;
HousingSettings->DistortionRadius = 0.8f;  // 更大失真区域
HousingSettings->DistortionHardness = 2.0f;  // 更硬失真
```

## 集成应用

### 与瞄准镜系统集成

```cpp
// 在瞄准镜中使用夜视
void USKGOpticComponent::SetupNightVision()
{
    USKGNightVisionComponent* NightVision = GetOwner()->FindComponentByClass<USKGNightVisionComponent>();
    if (NightVision)
    {
        // 创建材质实例
        FNightVisionMaterialPair& MaterialPair = NightVision->CreateAndSetupDynamicNightVisionMaterialInstance(TubeSettings, HousingSettings);
        
        // 应用到瞄准镜材质
        ApplyNightVisionMaterial(MaterialPair.NightVisionMaterialInstance);
    }
}
```

### 与角色系统集成

```cpp
// 角色夜视控制
void AMyCharacter::ToggleNightVision()
{
    USKGNightVisionComponent* NightVision = FindComponentByClass<USKGNightVisionComponent>();
    if (NightVision)
    {
        float CurrentGain = NightVision->GetGainValue();
        if (CurrentGain > 0.1f)
        {
            // 关闭夜视
            NightVision->SetGain(0.0f);
            PlayNightVisionOffSound();
        }
        else
        {
            // 开启夜视
            NightVision->SetGain(0.7f);
            PlayNightVisionOnSound();
        }
    }
}
```

### 与UI系统集成

```cpp
// HUD显示夜视状态
void AMyHUD::DrawHUD()
{
    USKGNightVisionComponent* NightVision = GetOwningPawn()->FindComponentByClass<USKGNightVisionComponent>();
    if (NightVision && NightVision->GetGainValue() > 0.1f)
    {
        // 绘制夜视指示器
        DrawTexture(NightVisionIcon, 50, 50, 32, 32, 0.0f, 0.0f, 1.0f, 1.0f, FLinearColor::Green);
        
        // 绘制增益条
        float Gain = NightVision->GetGainValue();
        DrawRect(FLinearColor(0.0f, 1.0f, 0.0f, 0.8f), 100, 50, Gain * 100, 10);
    }
}
```

## 技术细节

### 材质参数系统

夜视系统通过材质参数动态控制效果：

```cpp
void SetGainValues(const float Gain) const
{
    if (NightVisionMaterialPair.NightVisionMaterialInstance)
    {
        float NormalizedGain = Gain * (TubeSettings->MaxGain / 100.0f);
        
        NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(GainParameterName, TubeSettings->Gain);
        NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(NormalizedGainParameterName, NormalizedGain);
        NightVisionMaterialPair.NightVisionMaterialInstance->SetVectorParameterValue(NightVisionColorParameterName, TubeSettings->Color);
        NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(ContrastParameterName, TubeSettings->Contrast);
        
        // 噪点和颗粒
        NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(NoiseFactorParameterName, TubeSettings->NoiseFactor);
        NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(GrainMultiplierParameterName, TubeSettings->GrainMultiplier);
        
        // 镜头失真
        NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(DistortionRadiusParameterName, HousingSettings->DistortionRadius);
        NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(DistortionHardnessParameterName, HousingSettings->DistortionHardness);
        NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(UseRadialDistortionParameterName, HousingSettings->bUseRadialDistortion ? 1.0f : 0.0f);
    }
}
```

### 平滑增益算法

```cpp
void UpdateSmoothGain()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    float DeltaTime = CurrentTime - PreviousSmoothGainAdjustTimestamp;
    PreviousSmoothGainAdjustTimestamp = CurrentTime;
    
    float GainDifference = TargetGainLerpValue - CurrentGainLerpValue;
    float Adjustment = GainDifference * HousingSettings->SmoothAdjustSpeed * DeltaTime;
    
    CurrentGainLerpValue += Adjustment;
    
    if (FMath::Abs(GainDifference) < 0.01f)
    {
        CurrentGainLerpValue = TargetGainLerpValue;
        GetWorld()->GetTimerManager().ClearTimer(TSmoothGainAdjustHandle);
    }
    
    SetGainValues(CurrentGainLerpValue);
}
```

## 性能优化

### 材质实例缓存

```cpp
FNightVisionMaterialPair& CreateAndSetupDynamicNightVisionMaterialInstance(USKGPDANightVisionTubeSettings* Tube, USKGPDANightVisionHousingSettings* Housing)
{
    TubeSettings = Tube;
    HousingSettings = Housing;
    
    if (Tube && Housing)
    {
        // 创建动态材质实例
        NightVisionMaterialPair.NightVisionMaterialInstance = UMaterialInstanceDynamic::Create(Tube->NightVisionMaterial, this);
        NightVisionMaterialPair.NightVisionScintillationMaterialInstance = UMaterialInstanceDynamic::Create(Tube->NightVisionScintillationMaterial, this);
        
        // 初始化参数
        SetGainValues(GetGainValue());
    }
    
    return NightVisionMaterialPair;
}
```

### 参数更新优化

只在参数变化时更新材质：

```cpp
void SetGain(const float GainAlpha)
{
    float NewTarget = FMath::Clamp(GainAlpha, HousingSettings->MinimumGain, 1.0f);
    if (FMath::Abs(NewTarget - TargetGainLerpValue) > 0.001f)
    {
        TargetGainLerpValue = NewTarget;
        // 启动更新...
    }
}
```

## 常见问题

### Q: 夜视效果不显示？
A: 检查材质是否正确创建，确认材质实例已应用到相应的渲染目标

### Q: 增益调节无效？
A: 确认材质参数名称正确，检查材质中是否实现了相应的参数接收

### Q: 性能问题？
A: 合理设置平滑调节速度，避免过频繁的材质参数更新

### Q: 颜色异常？
A: 检查夜视颜色设置，确认材质支持颜色参数调节

### Q: 失真效果过强？
A: 调整失真半径和硬度参数，或禁用径向失真

## 最佳实践

1. **材质优化**: 使用高效的材质算法，避免复杂的像素计算
2. **参数缓存**: 缓存材质参数引用，避免重复查找
3. **渐进更新**: 使用平滑过渡而非瞬时变化
4. **效果平衡**: 平衡真实感和性能，避免过度复杂的视觉效果
5. **用户控制**: 提供直观的增益控制界面

## 相关模块

- [SKGOptic 模块](SKGOptic模块.md) - 光学瞄准镜系统
- [SKGLightLaser 模块](SKGLightLaser模块.md) - 激光指示器系统
- [SKGThermal 模块](SKGThermal模块.md) - 热成像系统
- [SKGShooterFramework 模块](SKGShooterFramework模块.md) - 火器核心系统

<system-reminder>Whenever you read the file, you should consider whether the user explicitly asked for a summary, analysis, or explanation. If they did, provide it. If not, continue with the task as requested.}</content>