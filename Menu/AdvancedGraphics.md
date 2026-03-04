# 高级图形功能设置设计

本文档定义 DLSS、FSR、XeSS、帧生成、Reflex 等现代图形技术的菜单设置方案。

---

## 1. 支持的技术列表

| 技术 | 厂商 | 类型 | UE5插件支持 |
|------|------|------|-------------|
| **DLSS** | NVIDIA | 超分辨率 | 官方DLSS插件 |
| **DLSS Frame Generation** | NVIDIA | 帧生成 | 官方DLSS插件 |
| **NVIDIA Reflex** | NVIDIA | 低延迟 | 官方DLSS插件 |
| **FSR 3** | AMD | 超分辨率 + 帧生成 | FidelityFX插件 |
| **XeSS** | Intel | 超分辨率 | XeSS插件 |
| **XeSS-FG** | Intel | 帧生成 | XeSS 2.0+ |
| **TSR** | Epic | 超分辨率 | 引擎内置 |

---

## 2. 设置项架构

### 2.1 数据结构

```cpp
// FPSGameSettings.h 扩展

UENUM(BlueprintType)
enum class EUpscaleMethod : uint8
{
    None            UMETA(DisplayName = "关闭"),
    TSR             UMETA(DisplayName = "TSR (虚幻内置)"),
    DLSS            UMETA(DisplayName = "DLSS (NVIDIA)"),
    FSR             UMETA(DisplayName = "FSR (AMD)"),
    XeSS            UMETA(DisplayName = "XeSS (Intel)"),
    Auto            UMETA(DisplayName = "自动选择")
};

UENUM(BlueprintType)
enum class EDLSQuality : uint8
{
    Auto            UMETA(DisplayName = "自动"),
    UltraQuality    UMETA(DisplayName = "超高质量"),
    Quality         UMETA(DisplayName = "质量"),
    Balanced        UMETA(DisplayName = "平衡"),
    Performance     UMETA(DisplayName = "性能"),
    UltraPerformance UMETA(DisplayName = "超高性能")
};

UENUM(BlueprintType)
enum class EFSRQuality : uint8
{
    Auto            UMETA(DisplayName = "自动"),
    NativeAA        UMETA(DisplayName = "原生AA"),
    Quality         UMETA(DisplayName = "质量 (1.5x)"),
    Balanced        UMETA(DisplayName = "平衡 (1.7x)"),
    Performance     UMETA(DisplayName = "性能 (2.0x)"),
    UltraPerformance UMETA(DisplayName = "超高性能 (3.0x)")
};

UENUM(BlueprintType)
enum class EXeSSQuality : uint8
{
    UltraQuality    UMETA(DisplayName = "超高质量"),
    Quality         UMETA(DisplayName = "质量"),
    Balanced        UMETA(DisplayName = "平衡"),
    Performance     UMETA(DisplayName = "性能"),
    UltraPerformance UMETA(DisplayName = "超高性能")
};

UENUM(BlueprintType)
enum class EReflexMode : uint8
{
    Disabled        UMETA(DisplayName = "关闭"),
    On              UMETA(DisplayName = "开启"),
    OnPlusBoost     UMETA(DisplayName = "开启 + 加速")
};

UENUM(BlueprintType)
enum class EFrameGenerationMode : uint8
{
    Disabled        UMETA(DisplayName = "关闭"),
    Auto            UMETA(DisplayName = "自动"),
    Enabled         UMETA(DisplayName = "开启")
};

// 扩展到 UFPSGameSettings
UCLASS()
class FPSGAME_API UFPSGameSettings : public USaveGame
{
    // ... 原有设置 ...
    
public:
    // ========== 高级图形设置 ==========
    
    // 超采样技术选择
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics")
    EUpscaleMethod UpscaleMethod;
    
    // DLSS设置
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|DLSS")
    EDLSQuality DLSSQuality;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|DLSS")
    bool bDLSSFrameGeneration;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|DLSS")
    bool bDLSSSharpness;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|DLSS")
    float DLSSSharpnessValue;
    
    // FSR设置
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|FSR")
    EFSRQuality FSRQuality;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|FSR")
    bool bFSRFrameGeneration;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|FSR")
    float FSRSharpness;
    
    // XeSS设置
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|XeSS")
    EXeSSQuality XeSSQuality;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|XeSS")
    bool bXeSSFrameGeneration;
    
    // Reflex低延迟
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|Reflex")
    EReflexMode ReflexMode;
    
    // 动态分辨率
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics")
    bool bDynamicResolution;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics")
    float DynamicResolutionMin;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics")
    float DynamicResolutionMax;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics")
    int32 DynamicResolutionTargetFPS;
    
    // HDR设置
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|HDR")
    bool bEnableHDR;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|HDR")
    float HDRBrightness;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|HDR")
    float HDRPaperWhite;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|HDR")
    float HDRMaxLuminance;
    
    // 光线追踪
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|RayTracing")
    bool bRayTracing;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|RayTracing")
    bool bRayTracedReflections;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|RayTracing")
    bool bRayTracedShadows;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|RayTracing")
    bool bRayTracedGI;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|RayTracing")
    EGraphicsQuality RayTracingQuality;
    
    // 路径追踪
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics")
    bool bPathTracing;
    
    // 显示设置
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|Display")
    bool bVRR;  // Variable Refresh Rate (G-Sync/FreeSync)
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedGraphics|Display")
    bool bFullscreenExclusive;
};
```

---

## 3. 技术可用性检测

```cpp
// FPSGraphicsSettingsLibrary.h

UCLASS()
class FPSGAME_API UFPSGraphicsSettingsLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    // 检查DLSS是否可用
    UFUNCTION(BlueprintPure, Category = "Graphics|Hardware")
    static bool IsDLSSSupported();
    
    // 检查DLSS FG是否可用
    UFUNCTION(BlueprintPure, Category = "Graphics|Hardware")
    static bool IsDLSSFrameGenerationSupported();
    
    // 检查FSR是否可用
    UFUNCTION(BlueprintPure, Category = "Graphics|Hardware")
    static bool IsFSRSupported();
    
    // 检查FSR FG是否可用
    UFUNCTION(BlueprintPure, Category = "Graphics|Hardware")
    static bool IsFSRFrameGenerationSupported();
    
    // 检查XeSS是否可用
    UFUNCTION(BlueprintPure, Category = "Graphics|Hardware")
    static bool IsXeSSSupported();
    
    // 检查Reflex是否可用
    UFUNCTION(BlueprintPure, Category = "Graphics|Hardware")
    static bool IsReflexSupported();
    
    // 检查HDR是否可用
    UFUNCTION(BlueprintPure, Category = "Graphics|Hardware")
    static bool IsHDRSupported();
    
    // 检查光追是否可用
    UFUNCTION(BlueprintPure, Category = "Graphics|Hardware")
    static bool IsRayTracingSupported();
    
    // 获取GPU信息
    UFUNCTION(BlueprintPure, Category = "Graphics|Hardware")
    static FString GetGPUName();
    
    UFUNCTION(BlueprintPure, Category = "Graphics|Hardware")
    static int32 GetGPUMemoryMB();
    
    // 获取推荐设置
    UFUNCTION(BlueprintPure, Category = "Graphics|Hardware")
    static EUpscaleMethod GetRecommendedUpscaleMethod();
    
    UFUNCTION(BlueprintPure, Category = "Graphics|Hardware")
    static EDLSQuality GetRecommendedDLSSQuality();
    
    UFUNCTION(BlueprintPure, Category = "Graphics|Hardware")
    static bool ShouldEnableFrameGeneration();
};
```

### 实现示例

```cpp
// FPSGraphicsSettingsLibrary.cpp

bool UFPSGraphicsSettingsLibrary::IsDLSSSupported()
{
#if WITH_DLSS
    return UNGXDLSSSupport::IsDLSSSupported();
#else
    return false;
#endif
}

bool UFPSGraphicsSettingsLibrary::IsDLSSFrameGenerationSupported()
{
#if WITH_DLSS
    return UNGXDLSSSupport::IsFrameGenerationSupported();
#else
    return false;
#endif
}

EUpscaleMethod UFPSGraphicsSettingsLibrary::GetRecommendedUpscaleMethod()
{
    if (IsDLSSSupported())
    {
        return EUpscaleMethod::DLSS;
    }
    else if (IsFSRSupported())
    {
        return EUpscaleMethod::FSR;
    }
    else if (IsXeSSSupported())
    {
        return EUpscaleMethod::XeSS;
    }
    return EUpscaleMethod::TSR;
}
```

---

## 4. 图形设置Widget扩展

```cpp
// FPSGraphicsSettingsWidget.h 扩展

UCLASS()
class FPSGAME_API UFPSGraphicsSettingsWidget : public UFPSMenuBase
{
    GENERATED_BODY()
    
protected:
    virtual void NativeOnInitialized() override;
    
    // 基础设置（原有）
    UPROPERTY(meta = (BindWidget))
    UFPSSettingCombo* OverallQualityCombo;
    
    // 超采样技术
    UPROPERTY(meta = (BindWidget))
    UFPSSettingCombo* UpscaleMethodCombo;
    
    // DLSS设置区域
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* DLSSEntryBox;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingCombo* DLSSQualityCombo;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* DLSSFGToggle;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* DLSSSharpnessToggle;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* DLSSSharpnessSlider;
    
    // FSR设置区域
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* FSREntryBox;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingCombo* FSRQualityCombo;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* FSRFGToggle;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* FSRSharpnessSlider;
    
    // XeSS设置区域
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* XeSSEntryBox;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingCombo* XeSSQualityCombo;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* XeSSFGToggle;
    
    // Reflex设置
    UPROPERTY(meta = (BindWidget))
    UFPSSettingCombo* ReflexCombo;
    
    // 动态分辨率
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* DynamicResToggle;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* DynamicResMinSlider;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* DynamicResMaxSlider;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* DynamicResTargetFPSSlider;
    
    // HDR设置
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* HDRSettingsBox;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* HDRToggle;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* HDRBrightnessSlider;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* HDRPaperWhiteSlider;
    
    // 光追设置
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* RayTracingBox;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* RayTracingToggle;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingCombo* RayTracingQualityCombo;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* RTReflectionsToggle;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* RTShadowsToggle;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* RTGIToggle;
    
    // GPU信息
    UPROPERTY(meta = (BindWidget))
    UCommonTextBlock* GPUInfoText;
    
    UPROPERTY(meta = (BindWidget))
    UCommonTextBlock* SupportedFeaturesText;
    
    // 自动优化按钮
    UPROPERTY(meta = (BindWidget))
    UFPSCommonButton* AutoOptimizeButton;
    
    // 刷新UI可用性
    UFUNCTION()
    void RefreshFeatureAvailability();
    
    // 根据选择的技术显示/隐藏对应设置
    UFUNCTION()
    void OnUpscaleMethodChanged(int32 MethodIndex);
    
    // 自动优化
    UFUNCTION()
    void OnAutoOptimizeClicked();
    
    // 显示技术说明
    UFUNCTION()
    void ShowUpscaleTechInfo(EUpscaleMethod Method);
};
```

---

## 5. 设置应用实现

```cpp
// FPSGameSettings.cpp 扩展

void UFPSGameSettings::ApplyGraphicsSettings()
{
    UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
    if (!UserSettings) return;
    
    // 基础设置...
    
    // ========== 应用超采样设置 ==========
    ApplyUpscaleSettings();
    
    // ========== 应用HDR设置 ==========
    ApplyHDRSettings();
    
    // ========== 应用光追设置 ==========
    ApplyRayTracingSettings();
    
    UserSettings->ApplySettings(false);
    UserSettings->SaveSettings();
}

void UFPSGameSettings::ApplyUpscaleSettings()
{
    // 获取视图配置
    FViewConfig ViewConfig;
    
    switch (UpscaleMethod)
    {
    case EUpscaleMethod::None:
        // 关闭所有超采样
        ViewConfig.AntiAliasingMethod = AAM_None;
        break;
        
    case EUpscaleMethod::TSR:
        // 启用TSR
        ViewConfig.AntiAliasingMethod = AAM_TemporalSuperResolution;
        break;
        
    case EUpscaleMethod::DLSS:
#if WITH_DLSS
        ApplyDLSSSettings();
#endif
        break;
        
    case EUpscaleMethod::FSR:
#if WITH_FSR
        ApplyFSRSettings();
#endif
        break;
        
    case EUpscaleMethod::XeSS:
#if WITH_XESS
        ApplyXeSSSettings();
#endif
        break;
        
    case EUpscaleMethod::Auto:
        // 自动选择最佳可用技术
        ApplyAutoUpscale();
        break;
    }
}

#if WITH_DLSS
void UFPSGameSettings::ApplyDLSSSettings()
{
    // 设置DLSS质量模式
    UNGXDLSSSettings* DLSSSettings = UNGXDLSSSettings::GetDLSSSettings();
    if (!DLSSSettings) return;
    
    // 映射质量设置
    UNGXDLSSMode DLSSMode = UNGXDLSSMode::Off;
    switch (DLSSQuality)
    {
    case EDLSQuality::Auto:           DLSSMode = UNGXDLSSMode::Auto; break;
    case EDLSQuality::UltraQuality:   DLSSMode = UNGXDLSSMode::UltraQuality; break;
    case EDLSQuality::Quality:        DLSSMode = UNGXDLSSMode::Quality; break;
    case EDLSQuality::Balanced:       DLSSMode = UNGXDLSSMode::Balanced; break;
    case EDLSQuality::Performance:    DLSSMode = UNGXDLSSMode::Performance; break;
    case EDLSQuality::UltraPerformance: DLSSMode = UNGXDLSSMode::UltraPerformance; break;
    }
    
    DLSSSettings->SetDLSSMode(DLSSMode);
    
    // 设置锐化
    if (bDLSSSharpness)
    {
        DLSSSettings->SetSharpness(DLSSSharpnessValue);
    }
    else
    {
        DLSSSettings->SetSharpness(0.0f);
    }
    
    // 设置帧生成
    if (IsDLSSFrameGenerationSupported())
    {
        DLSSSettings->SetFrameGenerationEnabled(bDLSSFrameGeneration);
    }
    
    // 设置Reflex
    ApplyReflexSettings();
}
#endif

void UFPSGameSettings::ApplyReflexSettings()
{
#if WITH_DLSS
    UNGXDLSSSettings* DLSSSettings = UNGXDLSSSettings::GetDLSSSettings();
    if (!DLSSSettings || !IsReflexSupported()) return;
    
    UNGXReflexMode Reflex = UNGXReflexMode::Off;
    switch (ReflexMode)
    {
    case EReflexMode::Disabled:      Reflex = UNGXReflexMode::Off; break;
    case EReflexMode::On:            Reflex = UNGXReflexMode::On; break;
    case EReflexMode::OnPlusBoost:   Reflex = UNGXReflexMode::OnPlusBoost; break;
    }
    
    DLSSSettings->SetReflexMode(Reflex);
#endif
}

void UFPSGameSettings::ApplyHDRSettings()
{
    // HDR设置通常需要重启或切换显示模式
    // 这里保存设置，实际应用可能在重启后
    
    if (bEnableHDR && IsHDRSupported())
    {
        // 启用HDR输出
        IConsoleManager::Get().FindConsoleVariable(TEXT("r.HDR.Display.OutputDevice"))
            ->Set(5); // HDR10
        
        // 设置亮度参数
        IConsoleManager::Get().FindConsoleVariable(TEXT("r.HDR.Display.MiddleGrey"))
            ->Set(HDRPaperWhite);
    }
    else
    {
        IConsoleManager::Get().FindConsoleVariable(TEXT("r.HDR.Display.OutputDevice"))
            ->Set(0); // SDR
    }
}

void UFPSGameSettings::ApplyRayTracingSettings()
{
    if (!IsRayTracingSupported()) return;
    
    IConsoleVariable* RTEnabled = IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing"));
    if (RTEnabled)
    {
        RTEnabled->Set(bRayTracing ? 1 : 0);
    }
    
    if (bRayTracing)
    {
        // 反射
        IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Reflections"))
            ->Set(bRayTracedReflections ? 1 : 0);
        
        // 阴影
        IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Shadows"))
            ->Set(bRayTracedShadows ? 1 : 0);
        
        // GI
        IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.GlobalIllumination"))
            ->Set(bRayTracedGI ? 1 : 0);
    }
}
```

---

## 6. 动态UI显示逻辑

```cpp
void UFPSGraphicsSettingsWidget::RefreshFeatureAvailability()
{
    // 检测GPU支持的特性
    bool bDLSSAvailable = UFPSGraphicsSettingsLibrary::IsDLSSSupported();
    bool bFSRAvailable = UFPSGraphicsSettingsLibrary::IsFSRSupported();
    bool bXeSSAvailable = UFPSGraphicsSettingsLibrary::IsXeSSSupported();
    bool bHDRAvailable = UFPSGraphicsSettingsLibrary::IsHDRSupported();
    bool bRTAvailable = UFPSGraphicsSettingsLibrary::IsRayTracingSupported();
    
    // 构建可用技术列表
    TArray<FText> UpscaleOptions;
    UpscaleOptions.Add(FText::FromString(TEXT("关闭")));
    UpscaleOptions.Add(FText::FromString(TEXT("TSR (虚幻内置)")));
    
    if (bDLSSAvailable)
        UpscaleOptions.Add(FText::FromString(TEXT("DLSS (NVIDIA)")));
    if (bFSRAvailable)
        UpscaleOptions.Add(FText::FromString(TEXT("FSR (AMD)")));
    if (bXeSSAvailable)
        UpscaleOptions.Add(FText::FromString(TEXT("XeSS (Intel)")));
    
    UpscaleOptions.Add(FText::FromString(TEXT("自动选择")));
    
    // 更新下拉框选项
    UpscaleMethodCombo->ClearOptions();
    for (const FText& Option : UpscaleOptions)
    {
        UpscaleMethodCombo->AddOption(Option.ToString());
    }
    
    // 显示/隐藏对应设置区域
    OnUpscaleMethodChanged(UpscaleMethodCombo->GetSelectedIndex());
    
    // HDR设置
    HDRSettingsBox->SetVisibility(bHDRAvailable ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    
    // 光追设置
    RayTracingBox->SetVisibility(bRTAvailable ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    
    // 更新GPU信息文本
    FString GPUInfo = FString::Printf(TEXT("GPU: %s\n显存: %d MB"),
        *UFPSGraphicsSettingsLibrary::GetGPUName(),
        UFPSGraphicsSettingsLibrary::GetGPUMemoryMB());
    GPUInfoText->SetText(FText::FromString(GPUInfo));
    
    // 更新支持特性文本
    FString Features;
    if (bDLSSAvailable) Features += TEXT("✓ DLSS ");
    if (bFSRAvailable) Features += TEXT("✓ FSR ");
    if (bXeSSAvailable) Features += TEXT("✓ XeSS ");
    if (bHDRAvailable) Features += TEXT("✓ HDR ");
    if (bRTAvailable) Features += TEXT("✓ 光追");
    SupportedFeaturesText->SetText(FText::FromString(Features));
}

void UFPSGraphicsSettingsWidget::OnUpscaleMethodChanged(int32 MethodIndex)
{
    // 隐藏所有技术特定设置
    DLSSEntryBox->SetVisibility(ESlateVisibility::Collapsed);
    FSREntryBox->SetVisibility(ESlateVisibility::Collapsed);
    XeSSEntryBox->SetVisibility(ESlateVisibility::Collapsed);
    
    // 根据选择显示对应设置
    EUpscaleMethod SelectedMethod = StaticCast<EUpscaleMethod>(MethodIndex);
    
    switch (SelectedMethod)
    {
    case EUpscaleMethod::DLSS:
        DLSSEntryBox->SetVisibility(ESlateVisibility::Visible);
        break;
    case EUpscaleMethod::FSR:
        FSREntryBox->SetVisibility(ESlateVisibility::Visible);
        break;
    case EUpscaleMethod::XeSS:
        XeSSEntryBox->SetVisibility(ESlateVisibility::Visible);
        break;
    default:
        break;
    }
}
```

---

## 7. 自动优化功能

```cpp
void UFPSGraphicsSettingsWidget::OnAutoOptimizeClicked()
{
    // 获取GPU性能等级
    int32 GPUMemory = UFPSGraphicsSettingsLibrary::GetGPUMemoryMB();
    FString GPUName = UFPSGraphicsSettingsLibrary::GetGPUName();
    
    // 简单的性能分级逻辑
    bool bIsHighEnd = GPUMemory >= 12000; // 12GB+ 显存
    bool bIsMidEnd = GPUMemory >= 8000;   // 8GB+ 显存
    bool bIsNVIDIA = GPUName.Contains(TEXT("NVIDIA")) || GPUName.Contains(TEXT("GeForce"));
    bool bIsAMD = GPUName.Contains(TEXT("AMD")) || GPUName.Contains(TEXT("Radeon"));
    
    // 推荐设置
    if (bIsHighEnd)
    {
        // 高端显卡：开启光追 + DLSS质量/FSR质量
        if (bIsNVIDIA && UFPSGraphicsSettingsLibrary::IsDLSSSupported())
        {
            UpscaleMethodCombo->SetSelectedIndex(StaticCast<int32>(EUpscaleMethod::DLSS));
            DLSSQualityCombo->SetSelectedIndex(StaticCast<int32>(EDLSQuality::Quality));
            DLSSFGToggle->SetIsChecked(true);
        }
        else if (bIsAMD && UFPSGraphicsSettingsLibrary::IsFSRSupported())
        {
            UpscaleMethodCombo->SetSelectedIndex(StaticCast<int32>(EUpscaleMethod::FSR));
            FSRQualityCombo->SetSelectedIndex(StaticCast<int32>(EFSRQuality::Quality));
        }
        
        // 开启光追
        if (UFPSGraphicsSettingsLibrary::IsRayTracingSupported())
        {
            RayTracingToggle->SetIsChecked(true);
            RayTracingQualityCombo->SetSelectedIndex(StaticCast<int32>(EGraphicsQuality::High));
        }
    }
    else if (bIsMidEnd)
    {
        // 中端显卡：DLSS性能/FSR平衡
        if (bIsNVIDIA && UFPSGraphicsSettingsLibrary::IsDLSSSupported())
        {
            UpscaleMethodCombo->SetSelectedIndex(StaticCast<int32>(EUpscaleMethod::DLSS));
            DLSSQualityCombo->SetSelectedIndex(StaticCast<int32>(EDLSQuality::Performance));
        }
        else
        {
            UpscaleMethodCombo->SetSelectedIndex(StaticCast<int32>(EUpscaleMethod::FSR));
            FSRQualityCombo->SetSelectedIndex(StaticCast<int32>(EFSRQuality::Balanced));
        }
    }
    else
    {
        // 低端显卡：FSR性能模式或TSR
        UpscaleMethodCombo->SetSelectedIndex(StaticCast<int32>(EUpscaleMethod::FSR));
        FSRQualityCombo->SetSelectedIndex(StaticCast<int32>(EFSRQuality::Performance));
    }
    
    // 刷新UI
    OnUpscaleMethodChanged(UpscaleMethodCombo->GetSelectedIndex());
    
    // 显示确认弹窗
    GetMenuManager()->ShowMessagePopup(
        FText::FromString(TEXT("自动优化完成")),
        FText::FromString(TEXT("已根据您的硬件配置推荐最佳设置。点击\"应用\"以生效。"))
    );
}
```

---

## 8. 蓝图结构

### WBP_GraphicsSettings 布局更新

```
ScrollBox
└── VerticalBox
    ├── GPUInfoPanel (Border)
    │   ├── GPUInfoText
    │   └── SupportedFeaturesText
    │
    ├── BasicSettings (Category)
    │   ├── OverallQualityCombo
    │   ├── ResolutionCombo
    │   └── WindowModeCombo
    │
    ├── UpscaleSettings (Category)
    │   ├── UpscaleMethodCombo
    │   ├── DLSSEntryBox (VerticalBox)
    │   │   ├── DLSSQualityCombo
    │   │   ├── DLSSFGToggle
    │   │   ├── DLSSSharpnessToggle
    │   │   └── DLSSSharpnessSlider
    │   ├── FSREntryBox (VerticalBox)
    │   │   ├── FSRQualityCombo
    │   │   ├── FSRFGToggle
    │   │   └── FSRSharpnessSlider
    │   └── XeSSEntryBox (VerticalBox)
    │       ├── XeSSQualityCombo
    │       └── XeSSFGToggle
    │
    ├── PerformanceSettings (Category)
    │   ├── ReflexCombo
    │   ├── DynamicResToggle
    │   ├── DynamicResMinSlider
    │   ├── DynamicResMaxSlider
    │   └── DynamicResTargetFPSSlider
    │
    ├── HDRSettingsBox (VerticalBox)
    │   ├── HDRToggle
    │   ├── HDRBrightnessSlider
    │   └── HDRPaperWhiteSlider
    │
    └── RayTracingBox (VerticalBox)
        ├── RayTracingToggle
        ├── RayTracingQualityCombo
        ├── RTReflectionsToggle
        ├── RTShadowsToggle
        └── RTGIToggle
```

---

## 9. 条件编译配置

### Build.cs 配置

```csharp
// FPSGame.Build.cs
public class FPSGame : ModuleRules
{
    public FPSGame(ReadOnlyTargetRules Target) : base(Target)
    {
        // ... 其他依赖 ...
        
        // DLSS支持 (需要NVIDIA授权)
        PrivateDependencyModuleNames.AddRange(new string[] {
            "DLSS",
            "DLSSBlueprint"
        });
        
        // FSR支持
        PrivateDependencyModuleNames.AddRange(new string[] {
            "FSR2MovieRenderPipeline",
            "FFXFSR2Api",
            "FFXFSR2TemporalUpscaling"
        });
        
        // XeSS支持
        PrivateDependencyModuleNames.AddRange(new string[] {
            "XeSS"
        });
        
        // 定义条件编译宏
        PublicDefinitions.Add("WITH_DLSS=1");
        PublicDefinitions.Add("WITH_FSR=1");
        PublicDefinitions.Add("WITH_XESS=1");
    }
}
```

---

## 10. 性能监控HUD

```cpp
// FPSPerformanceHUD.h

UCLASS()
class FPSGAME_API UFPSPerformanceHUD : public UUserWidget
{
    GENERATED_BODY()
    
public:
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    
protected:
    UPROPERTY(meta = (BindWidget))
    UCommonTextBlock* FPSText;
    
    UPROPERTY(meta = (BindWidget))
    UCommonTextBlock* FrameTimeText;
    
    UPROPERTY(meta = (BindWidget))
    UCommonTextBlock* UpscaleMethodText;
    
    UPROPERTY(meta = (BindWidget))
    UCommonTextBlock* FrameGenerationText;
    
    UPROPERTY(meta = (BindWidget))
    UCommonTextBlock* GPUMemoryText;
    
    float UpdateInterval = 0.5f;
    float TimeSinceLastUpdate = 0.0f;
};

// 实现
void UFPSPerformanceHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    TimeSinceLastUpdate += InDeltaTime;
    if (TimeSinceLastUpdate < UpdateInterval) return;
    TimeSinceLastUpdate = 0.0f;
    
    // 更新FPS
    float FPS = 1.0f / InDeltaTime;
    FPSText->SetText(FText::FromString(FString::Printf(TEXT("FPS: %.1f"), FPS)));
    
    // 更新帧时间
    float FrameTime = InDeltaTime * 1000.0f;
    FrameTimeText->SetText(FText::FromString(FString::Printf(TEXT("%.2f ms"), FrameTime)));
    
    // 更新上采样信息
    #if WITH_DLSS
    if (UNGXDLSSSettings* DLSSSettings = UNGXDLSSSettings::GetDLSSSettings())
    {
        UpscaleMethodText->SetText(FText::FromString(TEXT("DLSS")));
        FrameGenerationText->SetText(FText::FromString(
            DLSSSettings->IsFrameGenerationEnabled() ? TEXT("FG: 开") : TEXT("FG: 关")));
    }
    #endif
}
```

---

此设计提供了完整的现代图形技术设置支持，包括自动检测、智能推荐、实时应用等功能。
