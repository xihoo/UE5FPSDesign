# CommonUI Menu 系统设计文档

## 概述

本文档定义基于 UE5 **CommonUI Plugin** 的完整菜单系统架构。CommonUI 提供了一套面向游戏的UI框架，支持跨平台输入导航、统一的视觉风格、以及完善的输入路由系统。

### 为何选择 CommonUI？

| 特性 | UMG原生 | CommonUI |
|------|---------|----------|
| 游戏手柄支持 | 需自行实现 | 内置完善支持 |
| 输入路由 | 手动处理 | CommonUIActionRouter自动管理 |
| 激活状态管理 | 手动控制 | CommonActivatableWidget生命周期 |
| 平台适配 | 需自行判断 | CommonInputSubsystem自动识别 |
| 导航音效 | 需自行实现 | 内置Focus/Click音效 |

---

## 设计原则

1. **分层架构** - 主菜单 → 子菜单 → 具体设置页面
2. **数据持久化** - 使用UE5 SaveGame系统保存设置
3. **实时预览** - 图形/音频设置即时生效
4. **输入无关性** - 键鼠/手柄/触屏统一处理
5. **模块化设置** - 各设置项独立为可复用组件
6. **与GAS集成** - 键位绑定与Ability系统联动
7. **现代图形技术支持** - DLSS/FSR/XeSS、帧生成、光追等

---

## 核心架构

### 1. 菜单层级结构

```
UFPSMenuManager (管理器，放在GameInstance)
├── UFPSMainMenu (主菜单)
│   ├── 继续游戏 (Continue)
│   ├── 新游戏 (New Game)
│   ├── 设置 (Settings)
│   │   └── UFPSMenuStack.Push(UFPSSettingsMenu)
│   ├── 成就 (Achievements)
│   └── 退出 (Quit)
│
├── UFPSPauseMenu (暂停菜单，游戏中ESC)
│   ├── 继续游戏 (Resume)
│   ├── 设置 (Settings)
│   ├── 返回主菜单 (Main Menu)
│   └── 退出游戏 (Quit)
│
├── UFPSSettingsMenu (设置菜单)
│   ├── 图形 (Graphics) → UFPSGraphicsSettingsWidget
│   │   ├── 基础设置 (分辨率、画质、窗口模式)
│   │   ├── 高级图形 → [DLSS/FSR/XeSS设置](./AdvancedGraphics.md)
│   │   ├── HDR设置
│   │   └── 光线追踪设置
│   ├── 音频 (Audio) → UFPSAudioSettingsWidget
│   ├── 控制 (Controls) → UFPSControlSettingsWidget
│   ├── 游戏 (Gameplay) → UFPSGameplaySettingsWidget
│   └── 返回 (Back)
│
└── UFPSHUDLayoutMenu (HUD布局调整)
    └── 可拖拽的HUD元素调整
```

### 2. 核心类继承层次

```cpp
// CommonUI基类
UCommonActivatableWidget (引擎)
├── UFPSMenuBase (所有菜单基类)
│   ├── UFPSMainMenu
│   ├── UFPSPauseMenu
│   ├── UFPSSettingsMenu
│   └── UFPSPopupBase (弹窗基类)
│       ├── UFPSConfirmPopup
│       ├── UFPSMessagePopup
│       └── UFPSKeyBindingPopup

UCommonButtonBase (引擎)
└── UFPSCommonButton (项目统一按钮)
    ├── UFPSMenuButton (菜单导航按钮)
    ├── UFPSSettingRow (设置项行)
    └── UFPSKeyBindButton (键位绑定按钮)

UCommonUserWidget (引擎)
├── UFPSKeyBindingWidget (键位绑定组件)
├── UFPSSettingSlider (滑块设置项)
├── UFPSSettingCombo (下拉框设置项)
├── UFPSSettingToggle (开关设置项)
└── UFPSGraphicsPreview (图形预览窗口)
```

---

## 核心类详细设计

### 1. 菜单管理器 UFPSMenuManager

负责菜单的生命周期管理和栈式导航。

```cpp
// 头文件: FPSMenuManager.h
UCLASS()
class FPSGAME_API UFPSMenuManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    
    // ========== 主菜单 ==========
    
    // 打开主菜单（从游戏开始界面）
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void OpenMainMenu();
    
    // 关闭主菜单，开始游戏
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void CloseMainMenuAndStartGame();
    
    // ========== 暂停菜单 ==========
    
    // 切换暂停菜单
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void TogglePauseMenu();
    
    UFUNCTION(BlueprintCallable, Category = "Menu")
    bool IsPauseMenuOpen() const;
    
    // ========== 设置菜单 ==========
    
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void OpenSettingsMenu();
    
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void CloseSettingsMenu();
    
    // ========== 导航栈管理 ==========
    
    // 推入新菜单到栈
    UFUNCTION(BlueprintCallable, Category = "Menu|Stack")
    void PushMenu(TSubclassOf<UFPSMenuBase> MenuClass);
    
    // 从栈弹出当前菜单
    UFUNCTION(BlueprintCallable, Category = "Menu|Stack")
    void PopMenu();
    
    // 弹出所有菜单直到主层
    UFUNCTION(BlueprintCallable, Category = "Menu|Stack")
    void PopToRoot();
    
    // ========== 弹窗系统 ==========
    
    UFUNCTION(BlueprintCallable, Category = "Menu|Popup", meta = (AutoCreateRefTerm = "OnConfirmed,OnCancelled"))
    void ShowConfirmPopup(
        const FText& Title,
        const FText& Message,
        const FText& ConfirmText,
        const FText& CancelText,
        FOnMenuPopupConfirmed OnConfirmed,
        FOnMenuPopupCancelled OnCancelled
    );
    
    UFUNCTION(BlueprintCallable, Category = "Menu|Popup")
    void ShowMessagePopup(const FText& Title, const FText& Message);
    
    // ========== 保存/加载设置 ==========
    
    UFUNCTION(BlueprintCallable, Category = "Menu|Settings")
    void SaveAllSettings();
    
    UFUNCTION(BlueprintCallable, Category = "Menu|Settings")
    void LoadAllSettings();
    
    // 应用图形设置（需要重启的标记）
    UFUNCTION(BlueprintCallable, Category = "Menu|Settings")
    void ApplyGraphicsSettings(bool bRequireRestart);
    
    // 获取设置数据
    UFUNCTION(BlueprintPure, Category = "Menu|Settings")
    UFPSGameSettings* GetGameSettings() const { return GameSettings; }
    
protected:
    // 菜单层级容器
    UPROPERTY()
    UCommonActivatableWidgetContainerBase* MenuLayer;
    
    // 弹窗层级容器
    UPROPERTY()
    UCommonActivatableWidgetContainerBase* PopupLayer;
    
    // 设置数据
    UPROPERTY()
    UFPSGameSettings* GameSettings;
    
    // 原始输入模式（暂停菜单关闭时恢复）
    FInputModeDataBase CachedInputMode;
    bool bCachedMouseVisibility;
    
    // 初始化菜单层
    void SetupMenuLayers();
    
    // 切换游戏暂停状态
    void SetGamePaused(bool bPaused);
    
    // 设置输入模式
    void SetInputModeForMenu(bool bMenuOpen);
};
```

### 2. 菜单基类 UFPSMenuBase

所有菜单的基类，处理通用逻辑。

```cpp
// FPSMenuBase.h
UCLASS(Abstract)
class FPSGAME_API UFPSMenuBase : public UCommonActivatableWidget
{
    GENERATED_BODY()
    
public:
    // CommonActivatableWidget 接口
    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual UWidget* NativeGetDesiredFocusTarget() const override;
    
protected:
    // ========== 子类必须实现的接口 ==========
    
    // 返回默认聚焦的按钮
    UFUNCTION(BlueprintNativeEvent, Category = "Menu")
    UWidget* GetDefaultFocusWidget() const;
    virtual UWidget* GetDefaultFocusWidget_Implementation() const { return nullptr; }
    
    // 播放打开动画
    UFUNCTION(BlueprintNativeEvent, Category = "Menu")
    void PlayOpenAnimation();
    virtual void PlayOpenAnimation_Implementation() {}
    
    // 播放关闭动画
    UFUNCTION(BlueprintNativeEvent, Category = "Menu")
    void PlayCloseAnimation();
    virtual void PlayCloseAnimation_Implementation() {}
    
    // ========== 导航处理 ==========
    
    // 返回按钮处理
    UFUNCTION(BlueprintCallable, Category = "Menu")
    virtual void OnBackPressed();
    
    // 绑定到返回按钮的Action
    UPROPERTY(EditDefaultsOnly, Category = "Menu|Input")
    FDataTableRowHandle BackInputAction;
    
    // ========== 子组件引用 ==========
    
    // 标题文本
    UPROPERTY(meta = (BindWidget))
    UCommonTextBlock* MenuTitle;
    
    // 背景图片
    UPROPERTY(meta = (BindWidgetOptional))
    UImage* BackgroundImage;
    
    // 内容容器
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* ContentContainer;
    
    // 返回按钮
    UPROPERTY(meta = (BindWidgetOptional))
    UFPSCommonButton* BackButton;
    
    // ========== 数据访问 ==========
    
    UFUNCTION(BlueprintPure, Category = "Menu")
    UFPSMenuManager* GetMenuManager() const;
    
    UFUNCTION(BlueprintPure, Category = "Menu")
    UFPSGameSettings* GetGameSettings() const;
    
    // 是否是暂停菜单模式
    UPROPERTY(EditDefaultsOnly, Category = "Menu")
    bool bIsPauseMenu = false;
};
```

### 3. 统一按钮 UFPSCommonButton

项目中所有按钮使用此基类，保持一致风格。

```cpp
// FPSCommonButton.h
UCLASS(Abstract, meta = (DisableNativeTick))
class FPSGAME_API UFPSCommonButton : public UCommonButtonBase
{
    GENERATED_BODY()
    
public:
    UFPSCommonButton(const FObjectInitializer& ObjectInitializer);
    
    // 设置按钮文本
    UFUNCTION(BlueprintCallable, Category = "Button")
    void SetButtonText(const FText& InText);
    
    // 设置按钮图标
    UFUNCTION(BlueprintCallable, Category = "Button")
    void SetButtonIcon(UTexture2D* Icon);
    
protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;
    
    // 聚焦/悬停音效
    virtual void NativeOnHovered() override;
    virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
    virtual void NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
    
    // 点击音效
    virtual void NativeOnClicked() override;
    
    // ========== 视觉状态 ==========
    
    UPROPERTY(meta = (BindWidget))
    UCommonTextBlock* ButtonText;
    
    UPROPERTY(meta = (BindWidgetOptional))
    UImage* ButtonIcon;
    
    UPROPERTY(meta = (BindWidgetOptional))
    USizeBox* IconSizeBox;
    
    // 样式
    UPROPERTY(EditDefaultsOnly, Category = "Button|Style")
    FSlateBrush NormalBrush;
    
    UPROPERTY(EditDefaultsOnly, Category = "Button|Style")
    FSlateBrush HoveredBrush;
    
    UPROPERTY(EditDefaultsOnly, Category = "Button|Style")
    FSlateBrush PressedBrush;
    
    UPROPERTY(EditDefaultsOnly, Category = "Button|Style")
    FSlateBrush DisabledBrush;
    
    // 音效
    UPROPERTY(EditDefaultsOnly, Category = "Button|Audio")
    USoundBase* HoverSound;
    
    UPROPERTY(EditDefaultsOnly, Category = "Button|Audio")
    USoundBase* ClickSound;
    
    // 文字样式
    UPROPERTY(EditDefaultsOnly, Category = "Button|Style")
    TSubclassOf<UCommonTextStyle> NormalTextStyle;
    
    UPROPERTY(EditDefaultsOnly, Category = "Button|Style")
    TSubclassOf<UCommonTextStyle> SelectedTextStyle;
};
```

---

## 设置系统详细设计

### 1. 设置数据结构 UFPSGameSettings

保存所有可配置项的SaveGame类。

```cpp
// FPSGameSettings.h
UCLASS()
class FPSGAME_API UFPSGameSettings : public USaveGame
{
    GENERATED_BODY()
    
public:
    // 保存文件名
    static const FString SaveSlotName;
    static const uint32 SaveUserIndex;
    
    // ========== 图形设置 ==========
    
    // 窗口模式
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EWindowMode::Type WindowMode = EWindowMode::Fullscreen;
    
    // 分辨率
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    FIntPoint Resolution = FIntPoint(1920, 1080);
    
    // 帧率限制
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    int32 FrameRateLimit = 120;
    
    // 垂直同步
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    bool bVSync = true;
    
    // 整体画质等级
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EGraphicsQuality OverallQuality = EGraphicsQuality::High;
    
    // 具体画质设置
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EGraphicsQuality AntiAliasingQuality = EGraphicsQuality::High;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EGraphicsQuality PostProcessQuality = EGraphicsQuality::High;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EGraphicsQuality ShadowQuality = EGraphicsQuality::High;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EGraphicsQuality TextureQuality = EGraphicsQuality::High;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EGraphicsQuality EffectsQuality = EGraphicsQuality::High;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EGraphicsQuality ViewDistanceQuality = EGraphicsQuality::High;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EGraphicsQuality FoliageQuality = EGraphicsQuality::High;
    
    // 视野角度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    float FieldOfView = 90.0f;
    
    // 显示FPS
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    bool bShowFPS = false;
    
    // 动态模糊
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    bool bMotionBlur = true;
    
    // ========== 音频设置 ==========
    
    // 主音量
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float MasterVolume = 1.0f;
    
    // 音乐音量
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float MusicVolume = 0.8f;
    
    // 音效音量
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float SFXVolume = 1.0f;
    
    // 语音音量
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float VoiceVolume = 1.0f;
    
    // 环境音量
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float AmbientVolume = 0.7f;
    
    // 语音聊天音量
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float VoiceChatVolume = 1.0f;
    
    // 静音
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    bool bMuteMaster = false;
    
    // 启用语音聊天
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    bool bEnableVoiceChat = true;
    
    // 字幕
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    bool bSubtitles = true;
    
    // ========== 控制设置 ==========
    
    // 鼠标灵敏度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    float MouseSensitivity = 1.0f;
    
    // 瞄准灵敏度倍数
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    float AimSensitivityMultiplier = 0.5f;
    
    // 鼠标Y轴反转
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    bool bInvertMouseY = false;
    
    // 鼠标X轴反转
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    bool bInvertMouseX = false;
    
    // 摇杆灵敏度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    float GamepadSensitivity = 1.0f;
    
    // 摇杆死区
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    float GamepadDeadZone = 0.2f;
    
    // 瞄准辅助
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    bool bAimAssist = true;
    
    // 冲刺切换/长按
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    bool bToggleSprint = false;
    
    // 瞄准切换/长按
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    bool bToggleAim = false;
    
    // 蹲伏切换/长按
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    bool bToggleCrouch = true;
    
    // 自定义键位绑定 (InputAction -> Key)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    TMap<FName, FKey> CustomKeyBindings;
    
    // ========== 游戏设置 ==========
    
    // 难度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    EGameDifficulty Difficulty = EGameDifficulty::Normal;
    
    // 显示伤害数字
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    bool bShowDamageNumbers = true;
    
    // 显示血条
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    bool bShowEnemyHealthBar = true;
    
    // 自动拾取
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    bool bAutoLoot = false;
    
    // 小地图旋转
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    bool bRotateMinimap = true;
    
    // 语言设置 (格式: zh-Hans, en, ja, ko 等)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    FString Language = TEXT("zh-Hans");
    
    // 语言枚举 (便于蓝图使用，运行时从Language字符串解析)
    UPROPERTY(Transient, BlueprintReadOnly, Category = "Gameplay")
    EGameLanguage LanguageEnum = EGameLanguage::ChineseSimplified;
    
    // ========== 方法 ==========
    
    // 应用所有设置
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ApplyAllSettings();
    
    // 应用图形设置
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ApplyGraphicsSettings();
    
    // 应用音频设置
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ApplyAudioSettings();
    
    // 应用控制设置
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ApplyControlSettings();
    
    // 应用语言设置
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ApplyLanguageSettings();
    
    // 获取键位显示名称
    UFUNCTION(BlueprintCallable, Category = "Settings")
    FText GetKeyDisplayName(FName InputActionName) const;
    
    // 保存和加载
    static UFPSGameSettings* LoadOrCreateSettings();
    void SaveSettings();
};
```

### 2. 设置行组件 UFPSSettingRow

设置菜单中的单行设置项。

```cpp
// FPSSettingRow.h
UCLASS(Abstract, meta = (DisableNativeTick))
class FPSGAME_API UFPSSettingRow : public UCommonUserWidget
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable, Category = "Setting")
    void SetSettingLabel(const FText& Label);
    
    UFUNCTION(BlueprintCallable, Category = "Setting")
    void SetSettingDescription(const FText& Description);
    
protected:
    virtual void NativeOnInitialized() override;
    
    // 设置标签
    UPROPERTY(meta = (BindWidget))
    UCommonTextBlock* SettingLabel;
    
    // 设置描述（悬停显示）
    UPROPERTY(meta = (BindWidgetOptional))
    UCommonTextBlock* SettingDescription;
    
    // 值容器（子类添加具体控件）
    UPROPERTY(meta = (BindWidget))
    UPanelWidget* ValueContainer;
    
    // 重置按钮
    UPROPERTY(meta = (BindWidgetOptional))
    UFPSCommonButton* ResetButton;
    
    // 是否已修改标记
    UPROPERTY(BlueprintReadOnly, Category = "Setting")
    bool bIsDirty = false;
    
    // 默认值
    UPROPERTY(EditDefaultsOnly, Category = "Setting")
    FString DefaultValueString;
    
    // 委托
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSettingValueChanged, UFPSSettingRow*, const FString&);
    FOnSettingValueChanged OnSettingValueChanged;
};

// ========== 具体设置行类型 ==========

// 滑块设置
UCLASS()
class FPSGAME_API UFPSSettingSlider : public UFPSSettingRow
{
    GENERATED_BODY()
    
public:
    void SetupSlider(float Min, float Max, float Step, float InitialValue);
    float GetValue() const;
    
protected:
    UPROPERTY(meta = (BindWidget))
    USlider* Slider;
    
    UPROPERTY(meta = (BindWidget))
    UCommonTextBlock* ValueText;
    
    UFUNCTION()
    void OnSliderValueChanged(float Value);
};

// 下拉框设置
UCLASS()
class FPSGAME_API UFPSSettingCombo : public UFPSSettingRow
{
    GENERATED_BODY()
    
public:
    void SetupOptions(const TArray<FText>& Options, int32 InitialIndex);
    int32 GetSelectedIndex() const;
    
protected:
    UPROPERTY(meta = (BindWidget))
    UComboBoxString* ComboBox;
    
    UFUNCTION()
    void OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
};

// 开关设置
UCLASS()
class FPSGAME_API UFPSSettingToggle : public UFPSSettingRow
{
    GENERATED_BODY()
    
public:
    void SetupToggle(bool bInitialValue);
    bool GetValue() const;
    
protected:
    UPROPERTY(meta = (BindWidget))
    UCheckBox* CheckBox;
    
    UFUNCTION()
    void OnCheckStateChanged(bool bIsChecked);
};

// 键位绑定设置
UCLASS()
class FPSGAME_API UFPSSettingKeyBind : public UFPSSettingRow
{
    GENERATED_BODY()
    
public:
    void SetupKeyBind(FName InputAction, FKey CurrentKey);
    
    UFUNCTION(BlueprintCallable, Category = "KeyBind")
    void StartRebinding();
    
    UFUNCTION(BlueprintCallable, Category = "KeyBind")
    void CancelRebinding();
    
    bool IsWaitingForInput() const { return bWaitingForInput; }
    
protected:
    virtual void NativeOnInitialized() override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
    virtual FReply NativeOnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& InAnalogInputEvent) override;
    
    UPROPERTY(meta = (BindWidget))
    UFPSCommonButton* KeyBindButton;
    
    UPROPERTY(meta = (BindWidget))
    UCommonTextBlock* CurrentKeyText;
    
    // 绑定的InputAction名称
    UPROPERTY(EditAnywhere, Category = "KeyBind")
    FName InputActionName;
    
    // 当前绑定的键
    UPROPERTY(BlueprintReadOnly, Category = "KeyBind")
    FKey BoundKey;
    
    // 是否正在等待输入
    UPROPERTY(BlueprintReadOnly, Category = "KeyBind")
    bool bWaitingForInput = false;
    
    UFUNCTION()
    void OnKeyBindButtonClicked();
    
    void FinishRebinding(FKey NewKey);
};
```

---

## 设置页面详细设计

### 1. 图形设置页面 UFPSGraphicsSettingsWidget

```cpp
// FPSGraphicsSettingsWidget.h
UCLASS()
class FPSGAME_API UFPSGraphicsSettingsWidget : public UFPSMenuBase
{
    GENERATED_BODY()
    
protected:
    virtual void NativeOnInitialized() override;
    virtual UWidget* GetDefaultFocusWidget_Implementation() const override;
    
    // 设置列表
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* SettingsList;
    
    // 预设画质下拉框
    UPROPERTY(meta = (BindWidget))
    UFPSSettingCombo* OverallQualityCombo;
    
    // 分辨率下拉框
    UPROPERTY(meta = (BindWidget))
    UFPSSettingCombo* ResolutionCombo;
    
    // 窗口模式下拉框
    UPROPERTY(meta = (BindWidget))
    UFPSSettingCombo* WindowModeCombo;
    
    // 帧率限制滑块
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* FrameRateSlider;
    
    // 视野角度滑块
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* FOVSlider;
    
    // 垂直同步开关
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* VSyncToggle;
    
    // 各项画质设置
    UPROPERTY(meta = (BindWidget))
    UFPSSettingCombo* AntiAliasingCombo;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingCombo* ShadowQualityCombo;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingCombo* TextureQualityCombo;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingCombo* EffectsQualityCombo;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingCombo* ViewDistanceCombo;
    
    // 预览按钮
    UPROPERTY(meta = (BindWidget))
    UFPSCommonButton* PreviewButton;
    
    // 应用按钮
    UPROPERTY(meta = (BindWidget))
    UFPSCommonButton* ApplyButton;
    
    // 需要重启的标记
    UPROPERTY(BlueprintReadOnly, Category = "Graphics")
    bool bRequiresRestart = false;
    
    // 初始化可用分辨率列表
    void InitializeResolutionOptions();
    
    // 当画质预设改变时，自动调整子项
    UFUNCTION()
    void OnOverallQualityChanged(int32 QualityIndex);
    
    // 应用设置
    UFUNCTION()
    void OnApplyClicked();
    
    // 预览设置（临时应用）
    UFUNCTION()
    void OnPreviewClicked();
    
    // 检查是否需要重启
    bool CheckRequiresRestart();
};
```

### 2. 音频设置页面 UFPSAudioSettingsWidget

```cpp
// FPSAudioSettingsWidget.h
UCLASS()
class FPSGAME_API UFPSAudioSettingsWidget : public UFPSMenuBase
{
    GENERATED_BODY()
    
protected:
    virtual void NativeOnInitialized() override;
    
    // 主音量
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* MasterVolumeSlider;
    
    // 各分类音量
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* MusicVolumeSlider;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* SFXVolumeSlider;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* VoiceVolumeSlider;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* AmbientVolumeSlider;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* VoiceChatVolumeSlider;
    
    // 开关
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* MuteToggle;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* VoiceChatToggle;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* SubtitlesToggle;
    
    // 测试音效按钮
    UPROPERTY(meta = (BindWidget))
    UFPSCommonButton* TestSFXButton;
    
    // 测试音乐按钮
    UPROPERTY(meta = (BindWidget))
    UFPSCommonButton* TestMusicButton;
    
    UFUNCTION()
    void OnVolumeChanged(UFPSSettingRow* Row, const FString& Value);
    
    UFUNCTION()
    void OnTestSFXClicked();
    
    UFUNCTION()
    void OnTestMusicClicked();
    
    // 实时应用音量
    void ApplyVolumeSettings();
};
```

### 3. 键位绑定设置页面 UFPSControlSettingsWidget

```cpp
// FPSControlSettingsWidget.h
UCLASS()
class FPSGAME_API UFPSControlSettingsWidget : public UFPSMenuBase
{
    GENERATED_BODY()
    
public:
    // 定义可绑定的输入动作列表
    USTRUCT(BlueprintType)
    struct FInputActionGroup {
        GENERATED_BODY()
        
        UPROPERTY(EditAnywhere)
        FText GroupName;
        
        UPROPERTY(EditAnywhere)
        TArray<FName> InputActions;
    };
    
protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;
    
    // 分类列表
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* CategoryList;
    
    // 键位绑定列表
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* KeyBindList;
    
    // 灵敏度设置区域
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* SensitivityContainer;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* MouseSensitivitySlider;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* AimSensitivitySlider;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* GamepadSensitivitySlider;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingSlider* GamepadDeadZoneSlider;
    
    // 切换设置区域
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* ToggleContainer;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* ToggleSprintToggle;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* ToggleAimToggle;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* ToggleCrouchToggle;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* InvertYToggle;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* InvertXToggle;
    
    UPROPERTY(meta = (BindWidget))
    UFPSSettingToggle* AimAssistToggle;
    
    // 输入动作分组配置
    UPROPERTY(EditDefaultsOnly, Category = "KeyBinds")
    TArray<FInputActionGroup> InputActionGroups;
    
    // 重置按钮
    UPROPERTY(meta = (BindWidget))
    UFPSCommonButton* ResetToDefaultsButton;
    
    // 清除所有绑定按钮
    UPROPERTY(meta = (BindWidget))
    UFPSCommonButton* ClearAllButton;
    
    // 当前选中的分类
    UPROPERTY()
    int32 SelectedCategoryIndex = 0;
    
    // 键位绑定项数组
    UPROPERTY()
    TArray<UFPSSettingKeyBind*> KeyBindRows;
    
    // 当前正在等待输入的绑定项
    UPROPERTY()
    UFPSSettingKeyBind* CurrentlyRebinding = nullptr;
    
    // 初始化分类
    void InitializeCategories();
    
    // 显示指定分类的键位
    void ShowKeyBindsForCategory(int32 CategoryIndex);
    
    // 创建键位绑定行
    void CreateKeyBindRow(FName InputActionName, const FText& DisplayName);
    
    // 键位绑定回调
    UFUNCTION()
    void OnKeyBindStarted(UFPSSettingKeyBind* KeyBindRow);
    
    UFUNCTION()
    void OnKeyBindFinished(UFPSSettingKeyBind* KeyBindRow, FKey NewKey);
    
    // 输入监听（用于捕获新键位）
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
    virtual FReply NativeOnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& InAnalogInputEvent) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    
    // 重置默认
    UFUNCTION()
    void OnResetToDefaultsClicked();
    
    // 检查键位冲突
    bool CheckKeyConflict(FKey Key, FName ExcludedAction);
    
    // 显示冲突警告
    void ShowKeyConflictWarning(FName Action1, FName Action2);
};
```

---

## CommonUI 输入路由配置

### Input Action Data Table

创建 `DT_MenuInputActions` 数据表，定义菜单导航的输入动作。

| Row Name | InputAction | HoldTriggersAction | 
|----------|-------------|-------------------|
| MenuUp | IA_MenuUp | false |
| MenuDown | IA_MenuDown | false |
| MenuLeft | IA_MenuLeft | false |
| MenuRight | IA_MenuRight | false |
| MenuConfirm | IA_MenuConfirm | false |
| MenuCancel | IA_MenuCancel | false |
| MenuBack | IA_MenuBack | false |
| MenuNextTab | IA_MenuNextTab | false |
| MenuPrevTab | IA_MenuPrevTab | false |

### IA_MenuCancel 处理

```cpp
// 在UFPSMenuBase中处理返回
void UFPSMenuBase::OnBackPressed()
{
    // 播放关闭动画
    PlayCloseAnimation();
    
    // 返回上一个菜单
    if (UFPSMenuManager* Manager = GetMenuManager())
    {
        Manager->PopMenu();
    }
}
```

---

## 蓝图结构

### 1. WBP_MenuBase (抽象基类)

```
CanvasPanel (Root)
├── BackgroundImage (Image)
├── SafeZone
│   └── ContentContainer (VerticalBox)
│       ├── Header
│       │   ├── BackButton (FPSCommonButton)
│       │   └── MenuTitle (CommonTextBlock)
│       └── Content (SizeBox)
│           └── [子类覆盖内容]
```

### 2. WBP_MainMenu

```
[继承WBP_MenuBase]
Content
├── Logo (Image)
├── ButtonContainer (VerticalBox)
│   ├── ContinueButton (FPSMenuButton)
│   ├── NewGameButton (FPSMenuButton)
│   ├── SettingsButton (FPSMenuButton)
│   ├── AchievementsButton (FPSMenuButton)
│   └── QuitButton (FPSMenuButton)
└── VersionText (CommonTextBlock)
```

### 3. WBP_SettingsMenu

```
[继承WBP_MenuBase]
Content
├── TabContainer (HorizontalBox)
│   ├── GraphicsTab (FPSCommonButton)
│   ├── AudioTab (FPSCommonButton)
│   ├── ControlsTab (FPSCommonButton)
│   └── GameplayTab (FPSCommonButton)
├── SettingsContent (WidgetSwitcher)
│   ├── GraphicsSettings (WBP_GraphicsSettings)
│   ├── AudioSettings (WBP_AudioSettings)
│   ├── ControlSettings (WBP_ControlSettings)
│   └── GameplaySettings (WBP_GameplaySettings)
└── BottomBar (HorizontalBox)
    ├── ApplyButton (FPSCommonButton)
    ├── ResetButton (FPSCommonButton)
    └── ChangedIndicator (Image)
```

---

## 目录结构

```
Content/
├── UI/
│   └── Menu/
│       ├── Core/
│       │   ├── WBP_MenuBase.uasset
│       │   ├── WBP_PopupBase.uasset
│       │   ├── WBP_MenuManager.uasset
│       │   └── DT_MenuInputActions.uasset
│       │
│       ├── Components/
│       │   ├── WBP_CommonButton.uasset
│       │   ├── WBP_MenuButton.uasset
│       │   ├── WBP_SettingRow.uasset
│       │   ├── WBP_SettingSlider.uasset
│       │   ├── WBP_SettingCombo.uasset
│       │   ├── WBP_SettingToggle.uasset
│       │   └── WBP_SettingKeyBind.uasset
│       │
│       ├── Menus/
│       │   ├── WBP_MainMenu.uasset
│       │   ├── WBP_PauseMenu.uasset
│       │   └── WBP_SettingsMenu.uasset
│       │
│       ├── Settings/
│       │   ├── WBP_GraphicsSettings.uasset
│       │   ├── WBP_AudioSettings.uasset
│       │   ├── WBP_ControlSettings.uasset
│       │   └── WBP_GameplaySettings.uasset
│       │
│       └── Popups/
│           ├── WBP_ConfirmPopup.uasset
│           ├── WBP_MessagePopup.uasset
│           └── WBP_KeyConflictPopup.uasset
│
└── Data/
    └── Settings/
        └── DA_DefaultGameSettings.uasset

Source/FPSGame/Private/UI/Menu/
├── FPSMenuManager.cpp
├── FPSMenuBase.cpp
├── FPSCommonButton.cpp
├── FPSSettingRow.cpp
├── FPSGraphicsSettingsWidget.cpp
├── FPSAudioSettingsWidget.cpp
├── FPSControlSettingsWidget.cpp
└── FPSGameSettings.cpp
```

---

## 集成流程

### 1. 项目配置

在 `.uproject` 或 `.Build.cs` 启用插件：

```csharp
// [ProjectName].Build.cs
PublicDependencyModuleNames.AddRange(new string[] {
    "CommonUI",
    "CommonInput",
    "CommonGame",
    "EnhancedInput"
});
```

### 2. 初始化菜单管理器

```cpp
// 在GameInstance中
void UFPSTGameInstance::Init()
{
    Super::Init();
    
    // 获取菜单管理器并初始化
    MenuManager = GetSubsystem<UFPSMenuManager>();
    if (MenuManager)
    {
        MenuManager->LoadAllSettings();
    }
}
```

### 3. 输入优先级配置

```cpp
// 游戏中打开暂停菜单时的输入处理
void UFPSMenuManager::TogglePauseMenu()
{
    if (IsPauseMenuOpen())
    {
        // 关闭菜单，恢复游戏IMC
        PopMenu();
        RemoveInputMappingContext(UIMappingContext);
        AddInputMappingContext(GameplayMappingContext, 0);
        SetGamePaused(false);
    }
    else
    {
        // 打开菜单，添加UIMappingContext（高优先级）
        AddInputMappingContext(UIMappingContext, 10);
        PushMenu(PauseMenuClass);
        SetGamePaused(true);
    }
}
```

---

## 与原有Input系统的集成

### 键位绑定同步

当玩家在菜单中修改键位时，需要同步到Enhanced Input系统：

```cpp
void UFPSGameSettings::ApplyControlSettings()
{
    // 获取本地玩家
    if (ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstLocalPlayerController()->GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = 
            LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            // 遍历所有自定义绑定
            for (const auto& Pair : CustomKeyBindings)
            {
                FName ActionName = Pair.Key;
                FKey NewKey = Pair.Value;
                
                // 获取InputAction
                if (UInputAction* IA = InputActionMap.FindRef(ActionName))
                {
                    // 移除旧绑定
                    InputSubsystem->RemoveAllPlayerMappedKeysForAction(IA);
                    
                    // 添加新绑定
                    FModifyContextOptions Options;
                    Options.bIgnoreAllPressedKeysUntilRelease = false;
                    InputSubsystem->AddPlayerMappedKeyInSlot(
                        IA, 
                        NewKey, 
                        EInputMappingRebuildType::None,
                        Options
                    );
                }
            }
            
            // 重建输入上下文
            InputSubsystem->RequestRebuildControlMappings(
                EInputMappingRebuildType::RebuildWithFlush,
                true
            );
        }
    }
    
    // 保存设置
    SaveSettings();
}
```

---

## 多平台适配

### 平台检测

```cpp
// 根据平台显示不同提示
FText UFPSControlSettingsWidget::GetPlatformSpecificConfirmText() const
{
    if (UFPSCommonInputSubsystem* InputSubsystem = UFPSCommonInputSubsystem::Get(GetOwningLocalPlayer()))
    {
        ECommonInputType CurrentType = InputSubsystem->GetCurrentInputType();
        
        switch (CurrentType)
        {
        case ECommonInputType::Gamepad:
            return FText::FromString(TEXT("按 A 确认"));
        case ECommonInputType::Touch:
            return FText::FromString(TEXT("点击确认"));
        default:
            return FText::FromString(TEXT("按 Enter 确认"));
        }
    }
    return FText::GetEmpty();
}
```

### 动态输入图标

使用CommonInputSubsystem自动显示当前输入设备对应的图标：

```cpp
// 在按钮上显示对应平台的图标
UFUNCTION(BlueprintCallable)
void UFPSCommonButton::UpdateInputIcon()
{
    if (UFPSCommonInputSubsystem* InputSubsystem = ...)
    {
        FKey Key = InputSubsystem->GetKeyForAction(InputActionRowHandle);
        UTexture2D* Icon = InputSubsystem->GetIconForKey(Key);
        SetButtonIcon(Icon);
    }
}
```

---

## 总结

本Menu系统设计基于UE5 CommonUI框架，提供了：

1. **完整的菜单层级** - 主菜单 → 暂停菜单 → 设置菜单（图形/音频/控制/游戏）
2. **统一的设计风格** - 通过CommonButton和MenuBase确保视觉一致性
3. **完善的数据持久化** - SaveGame保存所有设置
4. **与GAS/Enhanced Input集成** - 键位绑定直接同步到输入系统
5. **跨平台支持** - 键鼠/手柄自动适配
6. **实时预览** - 图形/音频设置即时生效


---

## 相关文档

| 文档 | 说明 |
|------|------|
| [MenuArchitecture.md](./MenuArchitecture.md) | 架构图、类图、数据流图 |
| [QuickStart.md](./QuickStart.md) | 10步快速开始指南 |
| [Implementation.md](./Implementation.md) | C++实现代码示例 |
| [AdvancedGraphics.md](./AdvancedGraphics.md) | DLSS/FSR/XeSS/帧生成/光追等高级图形设置 |

---

## 总结

本Menu系统设计基于UE5 CommonUI框架，提供了：

1. **完整的菜单层级** - 主菜单 → 暂停菜单 → 设置菜单（图形/音频/控制/游戏）
2. **统一的设计风格** - 通过CommonButton和MenuBase确保视觉一致性
3. **完善的数据持久化** - SaveGame保存所有设置
4. **与GAS/Enhanced Input集成** - 键位绑定直接同步到输入系统
5. **跨平台支持** - 键鼠/手柄自动适配
6. **实时预览** - 图形/音频设置即时生效
7. **现代图形技术支持** - DLSS、FSR、XeSS、帧生成、光线追踪等
