# UMG 制作实例详解

本文档通过具体示例展示UMG与C++的交互。

---

## 实例1：创建音量滑块（完整流程）

### 目标
创建一个音量滑块，拖动时实时改变游戏音量。

### 步骤

#### 1. C++ 基类准备

```cpp
// FPSSettingSlider.h
UCLASS()
class FPSGAME_API UFPSSettingSlider : public UFPSSettingRow
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable, Category = "Slider")
    void SetupSlider(float Min, float Max, float Step, float InitialValue);
    
    UFUNCTION(BlueprintPure, Category = "Slider")
    float GetValue() const;
    
    // 暴露给蓝图的委托
    UPROPERTY(BlueprintAssignable, Category = "Slider")
    FOnSettingValueChanged OnValueChanged;
    
protected:
    UPROPERTY(meta = (BindWidget))
    USlider* Slider;
    
    UPROPERTY(meta = (BindWidget))
    UCommonTextBlock* ValueText;
    
    UFUNCTION()
    void HandleSliderValueChanged(float Value);
};
```

#### 2. 创建 WBP_SettingSlider

**Designer布局**：
```
HorizontalBox (RowRoot)
├── LabelContainer (SizeBox) [Width: 300]
│   └── SettingLabel (CommonTextBlock)
│       └── Variable Name: SettingLabel
│
└── ValueContainer (SizeBox) [Fill]
    └── HorizontalBox
        ├── Slider (Slider)
        │   └── Variable Name: Slider
        │   └── Size: Fill
        │
        └── ValueText (CommonTextBlock)
            └── Variable Name: ValueText
            └── Min Width: 60
```

**Graph逻辑**：

```
[Event Construct]
    │
    ├──► Slider (获取引用)
    │   └── Bind Event to OnValueChanged
    │       └── Target: Slider
    │       └── Event: HandleSliderValueChanged
    │
    └──► (可选) 设置初始值
        └── Slider: Set Value (0.5)

[Custom Event: HandleSliderValueChanged]
    │ Input: float Value
    │
    ├──► ValueText: Set Text
    │   └── Text: Format "{0}%" (Value * 100)
    │
    └──► (C++会处理) Broadcast OnValueChanged
```

#### 3. 在 WBP_AudioSettings 中使用

**Designer**：
```
ScrollBox
└── VerticalBox
    ├── MasterVolumeRow (WBP_SettingSlider实例)
    │   └── Variable Name: MasterVolumeRow
    │
    └── MusicVolumeRow (WBP_SettingSlider实例)
        └── Variable Name: MusicVolumeRow
```

**Graph**：

```
[Event Construct]
    │
    ├──► GetGameSettings (C++函数)
    │   └── (返回 UFPSGameSettings*)
    │
    ├──► MasterVolumeRow: Setup Slider
    │   ├── Min: 0.0
    │   ├── Max: 1.0
    │   ├── Step: 0.01
    │   └── InitialValue: GetGameSettings.MasterVolume
    │
    └──► MusicVolumeRow: Setup Slider
        └── ... 同上

[Event Construct]
    │
    ├──► MasterVolumeRow
    │   └── Bind Event to OnValueChanged
    │       ├── Target: MasterVolumeRow
    │       └── Event: OnMasterVolumeChanged
    │
    └──► MusicVolumeRow
        └── Bind Event to OnValueChanged
            └── Event: OnMusicVolumeChanged

[Custom Event: OnMasterVolumeChanged]
    │ Input: UFPSSettingRow* ChangedRow, FString NewValue
    │
    ├──► GetGameSettings
    │   └── Set MasterVolume = StringToFloat(NewValue)
    │
    └──► 实时应用音量
        └── Set Sound Mix Class Override
            ├── In Sound Mix: MasterMix
            ├── In Sound Class: MasterClass
            └── Volume: StringToFloat(NewValue)

[ApplyButton: OnClicked]
    │
    ├──► GetGameSettings: Apply Audio Settings
    └──► GetGameSettings: Save Settings
```

---

## 实例2：键位绑定按钮

### 目标
点击按钮，按下新按键后完成绑定。

### C++ 关键代码

```cpp
UCLASS()
class FPSGAME_API UFPSSettingKeyBind : public UFPSSettingRow
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable, Category = "KeyBind")
    void SetupKeyBind(FName InputAction, FKey CurrentKey);
    
    UFUNCTION(BlueprintCallable, Category = "KeyBind")
    void StartRebinding();
    
    UFUNCTION(BlueprintCallable, Category = "KeyBind")
    void CancelRebinding();
    
    // 绑定完成委托
    UPROPERTY(BlueprintAssignable, Category = "KeyBind")
    FOnKeyBindFinished OnKeyBindFinished;
    
protected:
    UPROPERTY(meta = (BindWidget))
    UFPSCommonButton* KeyBindButton;
    
    UPROPERTY(meta = (BindWidget))
    UCommonTextBlock* CurrentKeyText;
    
    UPROPERTY(BlueprintReadOnly, Category = "KeyBind")
    bool bWaitingForInput = false;
    
    // 处理输入
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
```

### 蓝图 WBP_SettingKeyBind

**Designer**：
```
HorizontalBox
├── SettingLabel (CommonTextBlock)
└── KeyBindButton (WBP_CommonButton)
    └── Variable Name: KeyBindButton
    └── CurrentKeyText (CommonTextBlock)
        └── Variable Name: CurrentKeyText
```

**Graph**：

```
[Event Construct]
    │
    └──► KeyBindButton
        └── Bind Event to OnClicked
            └── Event: OnKeyBindButtonClicked

[Custom Event: OnKeyBindButtonClicked]
    │
    ├──► Call "StartRebinding" (C++函数)
    │   └── 内部设置 bWaitingForInput = true
    │
    └──► CurrentKeyText: Set Text "按任意键..."

// 输入处理在C++中，完成后调用委托
// 如果需要在蓝图响应绑定完成：

[Event Construct]
    │
    └──► Bind Event to OnKeyBindFinished
        ├── Target: self
        └── Event: HandleKeyBindFinished

[Custom Event: HandleKeyBindFinished]
    │ Input: FKey NewKey
    │
    ├──► CurrentKeyText: Set Text (NewKey.DisplayName)
    └──► Play Animation "KeyBindSuccess"
```

---

## 实例3：Tab切换页面

### 目标
点击Tab按钮，切换显示不同的设置页面。

### 蓝图 WBP_SettingsMenu

**Designer**：
```
VerticalBox
├── TabRow (HorizontalBox)
│   ├── GraphicsTab (WBP_CommonButton)
│   │   └── Variable Name: GraphicsTabBtn
│   ├── AudioTab (WBP_CommonButton)
│   │   └── Variable Name: AudioTabBtn
│   └── ControlsTab (WBP_CommonButton)
│       └── Variable Name: ControlsTabBtn
│
└── TabSwitcher (WidgetSwitcher) [Variable Name: TabSwitcher]
    ├── GraphicsPage (WBP_GraphicsSettings)
    ├── AudioPage (WBP_AudioSettings)
    └── ControlsPage (WBP_ControlSettings)
```

**Graph - 方式1：直接连接（适合Tab少的情况）**：

```
[GraphicsTabBtn: OnClicked]
    │
    ├──► TabSwitcher: Set Active Widget Index (0)
    │
    ├──► GraphicsTabBtn: Set Style (Selected)
    ├──► AudioTabBtn: Set Style (Normal)
    └──► ControlsTabBtn: Set Style (Normal)

[AudioTabBtn: OnClicked]
    │
    ├──► TabSwitcher: Set Active Widget Index (1)
    │
    ├──► GraphicsTabBtn: Set Style (Normal)
    ├──► AudioTabBtn: Set Style (Selected)
    └──► ControlsTabBtn: Set Style (Normal)
```

**Graph - 方式2：使用函数（推荐）**：

```
[Function: SwitchToTab]
    │ Input: int32 TabIndex
    │
    ├──► TabSwitcher: Set Active Widget Index (TabIndex)
    │
    ├──► GraphicsTabBtn: Set Is Selected (TabIndex == 0)
    ├──► AudioTabBtn: Set Is Selected (TabIndex == 1)
    └──► ControlsTabBtn: Set Is Selected (TabIndex == 2)

[GraphicsTabBtn: OnClicked]
    └──► Call SwitchToTab (0)

[AudioTabBtn: OnClicked]
    └──► Call SwitchToTab (1)

[ControlsTabBtn: OnClicked]
    └──► Call SwitchToTab (2)
```

---

## 实例4：确认弹窗

### 目标
显示确认弹窗，用户选择"确认"或"取消"后执行对应操作。

### C++ 委托定义

```cpp
DECLARE_DYNAMIC_DELEGATE(FOnMenuPopupConfirmed);
DECLARE_DYNAMIC_DELEGATE(FOnMenuPopupCancelled);

UCLASS()
class FPSGAME_API UFPSMenuManager : public UGameInstanceSubsystem
{
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void ShowConfirmPopup(
        const FText& Title,
        const FText& Message,
        const FText& ConfirmText,
        const FText& CancelText,
        FOnMenuPopupConfirmed OnConfirmed,
        FOnMenuPopupCancelled OnCancelled
    );
};
```

### 蓝图使用示例

**场景**：退出按钮点击

```
[QuitButton: OnClicked]
    │
    └──► GetMenuManager
        └── Show Confirm Popup
            ├── Title: "确认退出"
            ├── Message: "确定要退出游戏吗？"
            ├── Confirm Text: "退出"
            ├── Cancel Text: "取消"
            │
            ├── On Confirmed (创建事件)
            │   └──► Quit Game
            │
            └── On Cancelled (创建事件)
                └──► [无操作]
```

**WBP_ConfirmPopup 内部实现**：

```
[Event Construct]
    │
    ├──► TitleText: Set Text (从传入参数)
    ├──► MessageText: Set Text
    ├──► ConfirmButton: Set Button Text
    └──► CancelButton: Set Button Text

[ConfirmButton: OnClicked]
    │
    ├──► Call OnConfirmed (执行传入的委托)
    └──► GetMenuManager: Close Popup

[CancelButton: OnClicked]
    │
    ├──► Call OnCancelled
    └──► GetMenuManager: Close Popup
```

---

## 实例5：动态显示/隐藏设置项

### 目标
根据GPU是否支持DLSS，动态显示DLSS设置区域。

### C++ 检测函数

```cpp
UFUNCTION(BlueprintPure, Category = "Graphics")
static bool IsDLSSSupported();
```

### 蓝图 WBP_GraphicsSettings

**Designer**：
```
VerticalBox
├── UpscaleMethodRow (WBP_SettingCombo)
│   └── Options: ["关闭", "TSR", "DLSS", "FSR"]
│
└── DLSSEntryBox (VerticalBox) [Variable Name: DLSSEntryBox]
    ├── DLSSQualityRow (WBP_SettingCombo)
    └── DLSSFGRow (WBP_SettingToggle)
```

**Graph**：

```
[Event Construct]
    │
    ├──► Call "IsDLSSSupported" (C++静态函数)
    │   └── Return Value
    │       ├── True: 显示DLSS选项
    │       └── False: 从下拉框移除DLSS选项
    │
    └──► RefreshDLSSVisibility

[Custom Event: RefreshDLSSVisibility]
    │
    ├──► UpscaleMethodRow: Get Selected Index
    │   └── 如果选择的是DLSS (Index 2)
    │       └──► DLSSEntryBox: Set Visibility (Visible)
    │   └── 否则
    │       └──► DLSSEntryBox: Set Visibility (Collapsed)

[UpscaleMethodRow.ComboBox: OnSelectionChanged]
    │
    └──► Call RefreshDLSSVisibility
```

---

## 实例6：自动优化按钮

### 目标
点击按钮，根据GPU自动推荐最佳设置。

### C++ 函数

```cpp
UFUNCTION(BlueprintCallable, Category = "Graphics")
void AutoOptimizeSettings();
```

### 蓝图 WBP_GraphicsSettings

```
[AutoOptimizeButton: OnClicked]
    │
    ├──► Play Animation "ButtonClick"
    │
    ├──► GetGameSettings: Auto Optimize Settings
    │
    ├──► 更新所有UI显示
    │   ├── OverallQualityRow: Set Selected Index
    │   ├── DLSSQualityRow: Set Selected Index
    │   └── ...
    │
    └──► Show Message Popup
        ├── Title: "自动优化完成"
        └── Message: "已根据您的硬件配置推荐最佳设置。"
```

---

## 实例7：保存/加载设置

### 保存设置

```
[ApplyButton: OnClicked]
    │
    ├──► 收集所有UI值到GameSettings
    │   ├── GetGameSettings: Set MasterVolume (MasterVolumeRow.GetValue)
    │   ├── GetGameSettings: Set Resolution (ResolutionRow.GetSelectedOption)
    │   └── ...
    │
    ├──► GetGameSettings: Apply All Settings
    │   └── (应用所有设置到引擎)
    │
    ├──► GetGameSettings: Save Settings
    │   └── (保存到SaveGame文件)
    │
    └──► Show Toast Message
        └── "设置已保存"
```

### 加载设置

```
[Event Construct (WBP_GraphicsSettings)]
    │
    ├──► GetGameSettings
    │
    ├──► OverallQualityRow: Set Selected Index (GameSettings.OverallQuality)
    ├──► ResolutionRow: Set Selected Option (GameSettings.Resolution.ToString())
    ├──► WindowModeRow: Set Selected Index (GameSettings.WindowMode)
    └──► ... 其他设置
```

---

## 常用蓝图节点速查

### 获取系统

| 功能 | 节点 |
|------|------|
| 获取MenuManager | `Get Game Instance` → `Get Subsystem` (FPSMenuManager) |
| 获取GameSettings | `Get Menu Manager` → `Get Game Settings` |
| 获取PlayerController | `Get Player Controller` |
| 获取OwningPlayer | `Get Owning Player` (在Widget中) |

### 文本操作

| 功能 | 节点 |
|------|------|
| 创建文本 | `Make Literal Text` |
| 格式化文本 | `Format Text` (使用 {0}, {1} 占位符) |
| 文本转字符串 | `Text To String` |
| 数值转文本 | `As Text` (float/int都有) |

### 委托绑定

| 功能 | 节点 |
|------|------|
| 绑定事件 | `Bind Event to [委托名]` |
| 创建事件 | `Create Event` → 选择自定义事件 |
| 调用委托 | `Call [委托名]` (通常在C++调用) |

### 类型转换

| 功能 | 节点 |
|------|------|
| 字符串转float | `Conv String to Float` |
| float转字符串 | `Conv Float to String` |
| int转枚举 | `Conv Int to Byte` → `Byte to Enum` |

---

## 调试输出示例

```
[任意事件]
    │
    └──► Print String
        ├── In String: "事件触发了！"
        ├── Print to Screen: true
        ├── Print to Log: true
        └── Text Color: (可选)
```

带变量输出：

```
[Slider: OnValueChanged]
    │
    └──► Print String
        └── In String: Format "Slider值: {0}"
            └── {0}: Value
```
