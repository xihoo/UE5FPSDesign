# UMG Widget 制作流程详解

本文档详细介绍如何基于已有的C++基类，在UMG中创建功能完整的Menu界面。

---

## 1. 前置准备

### 1.1 确保C++编译通过

在开始制作UMG前，确保以下C++类已编译成功：

```
✅ UFPSMenuManager
✅ UFPSMenuBase
✅ UFPSCommonButton
✅ UFPSSettingRow
✅ UFPSGameSettings
```

### 1.2 项目设置检查

打开 `Edit > Project Settings`：

| 设置项 | 推荐值 |
|--------|--------|
| Default GameMode | BP_FPSGameMode (继承自AFPSGameModeBase) |
| Game Instance | BP_FPSGameInstance |
| Game Viewport Client | BP_FPSGameViewportClient (继承UCommonGameViewportClient) |

---

## 2. 创建基础Widget

### 2.1 创建菜单基类 WBP_MenuBase

#### 步骤1：创建蓝图

1. 在Content Browser中右键 → `User Interface` → `Widget Blueprint`
2. 命名为 `WBP_MenuBase`
3. 打开蓝图，点击 `Class Settings`
4. 在 `Parent Class` 下拉框中选择 `FPSMenuBase` (你的C++类)

#### 步骤2：设计布局结构

在Designer面板中构建以下层级：

```
CanvasPanel (Root) [命名: RootCanvas]
├── BackgroundLayer (CanvasPanel) [可选，用于背景图/模糊效果]
│   └── BackgroundImage (Image) [命名: BackgroundImage]
│       └── Brush: 设置半透黑色或背景图
│
├── SafeZone (SafeZone) [确保内容在安全区域内]
│   └── MainContainer (VerticalBox) [命名: ContentContainer]
│       ├── HeaderRow (HorizontalBox)
│       │   ├── BackButton (CommonButton → 替换为WBP_CommonButton)
│       │   │   └── 命名: BackButton
│       │   └── Spacer
│       │
│       ├── TitleText (CommonTextBlock) [命名: MenuTitle]
│       │   ├── Text: "菜单标题"
│       │   ├── Style: H1样式
│       │   └── Margin: 0, 20, 0, 30
│       │
│       └── ContentSlot (SizeBox) [内容插槽，子类覆盖]
│           └── [留空，子类添加具体内容]
│
└── PopupLayer (CanvasPanel) [命名: PopupLayer，用于弹窗]
```

#### 步骤3：关键设置

| 组件 | 设置项 | 值 | 说明 |
|------|--------|-----|------|
| RootCanvas | Clipping | Clip to Bounds | 防止内容溢出 |
| BackgroundImage | Anchors | Fill All | 填满屏幕 |
| ContentContainer | Alignment | H: Center, V: Top | 内容居中对齐 |
| BackButton | Visibility | Collapsed (默认隐藏) | 子类按需显示 |

#### 步骤4：绑定C++变量（关键步骤）

选中Widget，在Details面板设置 `Variable Name`，**必须与C++中的 `meta = (BindWidget)` 名称一致**：

```cpp
// C++中声明
UPROPERTY(meta = (BindWidget))
UCommonTextBlock* MenuTitle;

UPROPERTY(meta = (BindWidget))
UFPSCommonButton* BackButton;

UPROPERTY(meta = (BindWidget))
UPanelWidget* ContentContainer;

UPROPERTY(meta = (BindWidgetOptional))
UImage* BackgroundImage;
```

**Blueprint中对应命名**：
- `MenuTitle` → Variable Name: `MenuTitle`
- `BackButton` → Variable Name: `BackButton`
- `ContentContainer` → Variable Name: `ContentContainer`
- `BackgroundImage` → Variable Name: `BackgroundImage`

勾选 `Is Variable` 选项。

#### 步骤5：初始化Back按钮

在Graph中，创建以下逻辑：

```
Event Construct
│
└──► BackButton (获取引用)
    └── IsValid ──► True
        └── Bind Event to OnClicked
            └── Target: self
            └── Event: Custom Event "OnBackClicked"

Custom Event: OnBackClicked
└──► Call OnBackPressed (调用C++函数)
```

**注意**：`OnBackPressed` 是C++中定义的BlueprintCallable函数。

---

### 2.2 创建通用按钮 WBP_CommonButton

#### 步骤1：创建蓝图

1. 创建 `WBP_CommonButton`，父类选择 `FPSCommonButton`

#### 步骤2：设计按钮结构

```
[Root] CommonButtonBase (这是引擎自动创建的)
└── Content (HorizontalBox) [命名: Content]
    ├── IconSlot (SizeBox) [命名: IconSizeBox，可选]
    │   └── IconImage (Image) [命名: ButtonIcon]
    │       └── Size: 32x32
    │
    └── TextSlot (SizeBox)
        └── ButtonText (CommonTextBlock) [命名: ButtonText]
            ├── Text: "按钮"
            ├── Style: 创建Text Style (T_CommonButtonText)
            └── Justification: Center
```

#### 步骤3：设置按钮样式

选中Root (CommonButtonBase)：

| 属性 | 值 |
|------|-----|
| Style | 创建新的 `CommonButtonStyle` |
| Normal | 设置正常状态图片/颜色 |
| Hovered | 设置悬停状态 (比Normal亮10%) |
| Pressed | 设置按下状态 (比Normal暗10%) |
| Disabled | 设置禁用状态 (灰度) |

#### 步骤4：绑定变量

确保以下Variable Name匹配C++：
- `ButtonText`
- `ButtonIcon` (Optional)
- `IconSizeBox` (Optional)

#### 步骤5：添加动画（可选）

创建两个动画：
1. `HoverAnim` - 按钮悬停时的缩放动画 (Scale 1.0 → 1.05)
2. `ClickAnim` - 点击时的按下动画 (Scale 1.0 → 0.95)

在Graph中：

```
Event OnMouseEnter
└── Play Animation (HoverAnim)

Event OnMouseLeave
└── Play Animation (HoverAnim, Reverse)

Event OnPressed
└── Play Animation (ClickAnim)

Event OnReleased
└── Play Animation (ClickAnim, Reverse)
```

---

## 3. 创建设置行组件

### 3.1 创建 WBP_SettingRow

父类：`FPSSettingRow`

```
HorizontalBox [命名: RowRoot]
├── LabelContainer (SizeBox) [Width: 300]
│   └── SettingLabel (CommonTextBlock) [命名: SettingLabel]
│       ├── Text: "设置名称"
│       └── Style: T_SettingLabel
│
├── ValueContainer (SizeBox) [命名: ValueContainer, Fill]
│   └── [留空，子类覆盖]
│
└── ResetButton (CommonButton) [命名: ResetButton]
    ├── Size: 32x32
    └── Icon: 重置图标
```

**关键绑定**：
- `SettingLabel` (BindWidget)
- `ValueContainer` (BindWidget)
- `ResetButton` (BindWidgetOptional)

### 3.2 创建 WBP_SettingCombo (继承WBP_SettingRow)

父类：`FPSSettingCombo`

```
[在ValueContainer内]
ComboBoxString [命名: ComboBox]
├── Option 0: "选项1"
├── Option 1: "选项2"
└── Font: 使用项目字体
```

**绑定事件**：

```
ComboBox: On Selection Changed
│
└──► Call "OnSelectionChanged" (C++函数)
    └── 参数: SelectedItem, SelectionType
```

### 3.3 创建 WBP_SettingSlider (继承WBP_SettingRow)

```
[在ValueContainer内]
HorizontalBox
├── Slider (Slider) [命名: Slider]
│   ├── Value: 0.5
│   ├── Min: 0.0
│   ├── Max: 1.0
│   └── Step Size: 0.01
│
└── ValueText (CommonTextBlock) [命名: ValueText]
    ├── Text: "50%"
    └── Min Width: 60
```

**绑定事件**：

```
Slider: On Value Changed
│
└──► 设置 ValueText.Text = Format("{0}%", Value * 100)
│
└──► Call "OnSliderValueChanged" (C++函数)
```

### 3.4 创建 WBP_SettingToggle (继承WBP_SettingRow)

```
[在ValueContainer内]
CheckBox [命名: CheckBox]
├── Checked State: Checked
└── Horizontal Alignment: Right
```

### 3.5 创建 WBP_SettingKeyBind (继承WBP_SettingRow)

父类：`FPSSettingKeyBind`

```
[在ValueContainer内]
FPSCommonButton [命名: KeyBindButton]
└── CurrentKeyText (CommonTextBlock) [命名: CurrentKeyText]
    └── Text: "F"
```

**Graph逻辑**：

```
KeyBindButton: OnClicked
└──► Call "StartRebinding" (C++函数)
    └── CurrentKeyText.Text = "按任意键..."

// 绑定完成委托
Event Construct
└── Bind to OnKeyBindFinished (需要在C++暴露的委托)
    └── CurrentKeyText.Text = NewKey.DisplayName
```

---

## 4. 创建具体菜单

### 4.1 创建 WBP_MainMenu

#### 步骤1：设置继承

父类：`WBP_MenuBase` (不是直接继承C++，而是继承已有的WBP)

#### 步骤2：添加内容

在 `ContentContainer` 的 `ContentSlot` 中添加：

```
ContentSlot (SizeBox) [覆盖父类的插槽]
└── MenuContent (VerticalBox)
    ├── LogoContainer (SizeBox) [Height: 200]
    │   └── LogoImage (Image)
    │       └── Brush: 游戏Logo
    │
    ├── Spacer (SizeBox) [Height: 100]
    │
    └── ButtonList (VerticalBox)
        ├── BTN_Continue (WBP_CommonButton) [命名: ContinueButton]
        ├── BTN_NewGame (WBP_CommonButton) [命名: NewGameButton]
        ├── BTN_Settings (WBP_CommonButton) [命名: SettingsButton]
        ├── BTN_Achievements (WBP_CommonButton) [命名: AchievementsButton]
        └── BTN_Quit (WBP_CommonButton) [命名: QuitButton]
```

#### 步骤3：配置按钮

选中每个按钮，设置：

| 按钮 | ButtonText | OnClicked事件 |
|------|-----------|---------------|
| ContinueButton | "继续游戏" | Call: GetMenuManager → [预留] |
| NewGameButton | "新游戏" | Call: GetMenuManager → CloseMainMenuAndStartGame |
| SettingsButton | "设置" | Call: GetMenuManager → OpenSettingsMenu |
| AchievementsButton | "成就" | [预留] |
| QuitButton | "退出" | 显示确认弹窗 |

#### 步骤4：退出确认弹窗

```
QuitButton: OnClicked
│
└──► GetMenuManager
    └── ShowConfirmPopup
        ├── Title: "确认退出"
        ├── Message: "确定要退出游戏吗？未保存的进度将丢失。"
        ├── ConfirmText: "退出"
        ├── CancelText: "取消"
        ├── OnConfirmed: QuitGame
        └── OnCancelled: [无操作]
```

#### 步骤5：默认焦点

在C++中，`GetDefaultFocusWidget()` 会查找第一个可交互的按钮。在蓝图中可以覆盖：

```
// 在Graph中，如果需要自定义焦点
Event OnInitialized
└──► 如果需要延迟设置焦点
    └── Delay 0.1s
        └── SetUserFocus (NewGameButton)
```

---

### 4.2 创建 WBP_SettingsMenu

#### 步骤1：创建Tab结构

```
ContentContainer (覆盖父类)
└── SettingsContainer (VerticalBox)
    ├── TabRow (HorizontalBox)
    │   ├── GraphicsTab (WBP_CommonButton) [命名: GraphicsTabBtn]
    │   ├── AudioTab (WBP_CommonButton) [命名: AudioTabBtn]
    │   ├── ControlsTab (WBP_CommonButton) [命名: ControlsTabBtn]
    │   └── GameplayTab (WBP_CommonButton) [命名: GameplayTabBtn]
    │
    ├── TabContent (WidgetSwitcher) [命名: TabSwitcher]
    │   ├── GraphicsPage (WBP_GraphicsSettings)
    │   ├── AudioPage (WBP_AudioSettings)
    │   ├── ControlsPage (WBP_ControlSettings)
    │   └── GameplayPage (WBP_GameplaySettings)
    │
    └── BottomBar (HorizontalBox)
        ├── Spacer (Fill)
        ├── ApplyButton (WBP_CommonButton) [命名: ApplyButton]
        └── ResetButton (WBP_CommonButton) [命名: ResetButton]
```

#### 步骤2：Tab切换逻辑

```
GraphicsTabBtn: OnClicked
└──► TabSwitcher: SetActiveWidgetIndex (0)
└──► SetButtonSelected (GraphicsTabBtn, true)
└──► SetButtonSelected (其他TabBtn, false)

AudioTabBtn: OnClicked
└──► TabSwitcher: SetActiveWidgetIndex (1)
└──► [同上，更新选中状态]
```

**更好的做法**：创建一个通用的Tab切换函数

```
Function: SwitchTab (Input: TabIndex)
├── TabSwitcher: SetActiveWidgetIndex (TabIndex)
├── GraphicsTabBtn: SetSelected (TabIndex == 0)
├── AudioTabBtn: SetSelected (TabIndex == 1)
├── ControlsTabBtn: SetSelected (TabIndex == 2)
└── GameplayTabBtn: SetSelected (TabIndex == 3)
```

#### 步骤3：应用按钮

```
ApplyButton: OnClicked
│
└──► GetGameSettings
    ├── ApplyGraphicsSettings
    ├── ApplyAudioSettings
    ├── ApplyControlSettings
    └── SaveSettings
│
└──► ShowMessagePopup
    └── Message: "设置已保存"
```

---

### 4.3 创建 WBP_GraphicsSettings

#### 步骤1：布局设计

```
ScrollBox
└── SettingsList (VerticalBox)
    ├── GPUInfoPanel (Border)
    │   ├── GPUNameText (CommonTextBlock)
    │   └── SupportedFeaturesText (CommonTextBlock)
    │
    ├── SectionLabel: "基础设置"
    ├── OverallQualityRow (WBP_SettingCombo)
    ├── ResolutionRow (WBP_SettingCombo)
    ├── WindowModeRow (WBP_SettingCombo)
    ├── FrameRateRow (WBP_SettingSlider)
    │
    ├── SectionLabel: "超采样技术"
    ├── UpscaleMethodRow (WBP_SettingCombo)
    ├── DLSSEntryBox (VerticalBox)
    │   ├── DLSSQualityRow (WBP_SettingCombo)
    │   ├── DLSSFGRow (WBP_SettingToggle)
    │   └── DLSSSharpnessRow (WBP_SettingSlider)
    │
    ├── SectionLabel: "高级"
    └── ... 其他设置行
```

#### 步骤2：初始化设置值

```
Event Construct
│
└──► GetGameSettings
    ├── OverallQualityRow.ComboBox: SetSelectedIndex (OverallQuality)
    ├── ResolutionRow.ComboBox: SetSelectedOption (Resolution.ToString())
    └── ... 其他设置
│
└──► Call "RefreshFeatureAvailability" (C++函数，检测GPU特性)
```

#### 步骤3：绑定值变化事件

```
OverallQualityRow.ComboBox: OnSelectionChanged
│
└──► GetGameSettings
    └── Set OverallQuality = SelectedIndex
│
└──► 如果 SelectedIndex != Custom
    ├── DLSSQualityRow: SetIsEnabled (false)
    └── 自动设置所有子项质量
```

---

## 5. 事件绑定详解

### 5.1 C++暴露给蓝图的方法

#### 方式1：BlueprintCallable 函数

```cpp
UCLASS()
class FPSGAME_API UFPSMenuManager : public UGameInstanceSubsystem
{
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void OpenSettingsMenu();
    
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void ShowConfirmPopup(
        const FText& Title,
        const FText& Message,
        const FText& ConfirmText,
        const FText& CancelText,
        FOnMenuPopupConfirmed OnConfirmed,  // 委托参数
        FOnMenuPopupCancelled OnCancelled
    );
};
```

**蓝图调用**：

```
RightClick → Call Function → FPSMenuManager → OpenSettingsMenu
```

#### 方式2：BlueprintImplementableEvent (C++声明，蓝图实现)

```cpp
UCLASS()
class FPSGAME_API UFPSMenuBase : public UCommonActivatableWidget
{
    UFUNCTION(BlueprintImplementableEvent, Category = "Menu")
    void PlayOpenAnimation();
    
    UFUNCTION(BlueprintImplementableEvent, Category = "Menu")
    void PlayCloseAnimation();
};
```

**蓝图实现**：

1. 在WBP中右键 → `Add Event` → `Override` → `Play Open Animation`
2. 添加动画轨道，设计打开动画
3. 在C++调用 `PlayOpenAnimation()` 时会触发蓝图事件

#### 方式3：BlueprintNativeEvent (C++默认实现 + 蓝图可覆盖)

```cpp
UCLASS()
class FPSGAME_API UFPSMenuBase : public UCommonActivatableWidget
{
    UFUNCTION(BlueprintNativeEvent, Category = "Menu")
    UWidget* GetDefaultFocusWidget() const;
    virtual UWidget* GetDefaultFocusWidget_Implementation() const;
};
```

**蓝图覆盖**：

1. 在MyBlueprint面板找到 `Get Default Focus Widget`
2. 点击 `Override`
3. 返回需要默认焦点的Widget引用

#### 方式4：动态多播委托 (BlueprintAssignable)

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSettingValueChanged, UFPSSettingRow*, ChangedRow);

UCLASS()
class FPSGAME_API UFPSSettingRow : public UCommonUserWidget
{
    UPROPERTY(BlueprintAssignable, Category = "Setting")
    FOnSettingValueChanged OnSettingValueChanged;
};
```

**蓝图绑定**：

```
Event Construct
└── Bind Event to OnSettingValueChanged
    └── Target: 设置行实例
    └── Event: Custom Event
        └── [处理值变化逻辑]
```

---

### 5.2 委托绑定完整示例

**场景**：当音量滑块变化时，实时更新音量

```cpp
// C++声明
UCLASS()
class FPSGAME_API UFPSSettingSlider : public UFPSSettingRow
{
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSliderValueChanged, float, NewValue, UFPSSettingSlider*, Slider);
    
    UPROPERTY(BlueprintAssignable, Category = "Slider")
    FOnSliderValueChanged OnValueChanged;
    
    UFUNCTION()
    void HandleSliderValueChanged(float Value);
};
```

```cpp
// C++实现
void UFPSSettingSlider::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    
    if (Slider)
    {
        Slider->OnValueChanged.AddDynamic(this, &UFPSSettingSlider::HandleSliderValueChanged);
    }
}

void UFPSSettingSlider::HandleSliderValueChanged(float Value)
{
    // 更新显示文本
    if (ValueText)
    {
        ValueText->SetText(FText::AsPercent(Value));
    }
    
    // 广播委托
    OnValueChanged.Broadcast(Value, this);
}
```

**蓝图绑定**：

```
Event Construct (在WBP_AudioSettings中)
│
└──► MasterVolumeRow (WBP_SettingSlider)
    └── Bind Event to OnValueChanged
        └── Target: MasterVolumeRow
        └── Event: Custom Event "OnMasterVolumeChanged"
            ├── NewValue (float)
            └──► 设置音效音量
                └── Set Sound Mix Class Override
                    ├── In Sound Mix: MasterSoundMix
                    ├── In Sound Class: MasterSoundClass
                    └── Volume: NewValue
```

---

## 6. 数据流完整示例

### 场景：修改DLSS质量设置

```
[用户操作]
    │
    ▼
[WBP_GraphicsSettings]
    │ 用户选择 "DLSS质量: 性能"
    ▼
[ComboBox: OnSelectionChanged]
    │
    ▼
[调用C++ UFPSGraphicsSettingsWidget::OnDLSSQualityChanged]
    │ 更新 GameSettings->DLSSQuality
    ▼
[C++ 调用 ApplyDLSSSettings()]
    │ 应用设置到DLSS插件
    ▼
[NVIDIA NGX API]
    │ 实际改变渲染分辨率
    ▼
[屏幕显示更新]
```

---

## 7. 调试技巧

### 7.1 检查BindWidget是否正确

在C++构造函数中添加检查：

```cpp
void UFPSMenuBase::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    
    // 检查关键Widget是否绑定成功
    if (!MenuTitle)
    {
        UE_LOG(LogMenu, Warning, TEXT("MenuTitle not bound in %s"), *GetName());
    }
    
    if (!BackButton)
    {
        UE_LOG(LogMenu, Warning, TEXT("BackButton not bound in %s"), *GetName());
    }
}
```

### 7.2 蓝图调试

1. 右键节点 → `Add Breakpoint`
2. 运行游戏，触发断点
3. 检查Variables面板中的值
4. 使用 `Print String` 节点输出调试信息

### 7.3 常见问题

| 问题 | 原因 | 解决 |
|------|------|------|
| C++函数在蓝图不可见 | 没加 `BlueprintCallable` | 添加宏 |
| Widget变量为None | BindWidget名称不匹配 | 检查Variable Name |
| 事件不触发 | 委托未绑定 | 检查Bind Event节点 |
| 编译错误 | 父类选择错误 | 确认Parent Class |

---

## 8. 最佳实践

### 8.1 命名规范

| 类型 | 前缀 | 示例 |
|------|------|------|
| Widget Blueprint | WBP_ | WBP_MainMenu |
| 按钮实例 | BTN_ | BTN_Settings |
| 文本实例 | TXT_ | TXT_Title |
| 容器实例 | BOX_ | BOX_SettingsList |
| 图片实例 | IMG_ | IMG_Background |

### 8.2 代码与蓝图分工

| 职责 | C++ | 蓝图 |
|------|-----|------|
| 数据结构 | ✅ 定义 | ❌ |
| 业务逻辑 | ✅ 实现 | 轻量调用 |
| UI布局 | ❌ | ✅ 设计 |
| 动画效果 | ❌ | ✅ 制作 |
| 视觉样式 | 基础定义 | 细化调整 |
| 事件绑定 | 声明委托 | 连接节点 |

### 8.3 性能优化

1. **避免每帧更新UI**：使用事件驱动，而非Tick
2. **对象池**：频繁显示的弹窗可缓存
3. **延迟加载**：设置页面按需初始化
4. **Visibility控制**：隐藏页面设为 `Collapsed` 而非 `Hidden`

---

## 9. 快速检查清单

创建新Widget时：

- [ ] 选择正确的Parent Class
- [ ] Variable Name与C++ `BindWidget` 名称一致
- [ ] 勾选 `Is Variable`
- [ ] 设置正确的Anchors
- [ ] 绑定必要的事件
- [ ] 在C++中添加空指针检查
- [ ] 测试键盘/手柄导航
- [ ] 测试不同分辨率下的显示
