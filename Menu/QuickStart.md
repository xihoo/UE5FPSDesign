# CommonUI Menu 系统快速开始指南

## 第1步：启用插件

1. 打开编辑器，点击 `Edit` → `Plugins`
2. 启用以下插件：
   - ✅ **Common UI Plugin**
   - ✅ **Common Input Plugin**
   - ✅ **Gameplay Abilities** (如果还没启用)
3. 重启编辑器

---

## 第2步：配置项目设置

### 2.1 创建 CommonGameViewportClient

创建 `FPSGameViewportClient` 类继承自 `UCommonGameViewportClient`：

```cpp
// FPSGameViewportClient.h
UCLASS()
class FPSGAME_API UFPSGameViewportClient : public UCommonGameViewportClient
{
    GENERATED_BODY()
};
```

在 `DefaultEngine.ini` 中配置：

```ini
[/Script/Engine.Engine]
GameViewportClientClass=/Script/FPSGame.FPSGameViewportClient
```

### 2.2 配置 Common Input

创建 `DefaultCommonInput.ini`：

```ini
[/Script/CommonInput.CommonInputSettings]
InputData=/Game/UI/Menu/Core/DA_CommonInputData.DA_CommonInputData_C

[/Script/CommonInput.CommonInputPlatformSettings]
+InputDataClassRedirects=(OldClassName="",NewClassName="",FilterPlatform="")
```

---

## 第3步：创建核心C++类

按照 `Menu.md` 中的定义，依次创建以下类：

### 必需的基础类（按顺序实现）

1. `UFPSGameSettings` - SaveGame设置存储
2. `UFPSMenuManager` - GameInstanceSubsystem
3. `UFPSCommonButton` - 按钮基类
4. `UFPSMenuBase` - 菜单基类
5. `UFPSSettingRow` - 设置行基类

### 设置页面类

6. `UFPSGraphicsSettingsWidget`
7. `UFPSAudioSettingsWidget`
8. `UFPSControlSettingsWidget`
9. `UFPSGameplaySettingsWidget`

---

## 第4步：创建基础蓝图

### 4.1 创建 Data Asset

创建 `DA_CommonInputData` (继承自 `UCommonInputBaseControllerData`)：

| 属性 | 值 |
|------|-----|
| DefaultInputType | MouseAndKeyboard |
| +InputDataTable | `DT_MenuInputActions` |

### 4.2 创建输入动作数据表

创建 `DT_MenuInputActions` (行结构 `CommonInputActionDataBase`)：

| 行名 | InputAction | HoldTriggersAction |
|------|-------------|-------------------|
| MenuUp | IA_MenuUp | false |
| MenuDown | IA_MenuDown | false |
| MenuLeft | IA_MenuLeft | false |
| MenuRight | IA_MenuRight | false |
| MenuConfirm | IA_MenuConfirm | false |
| MenuCancel | IA_MenuCancel | false |
| MenuBack | IA_MenuBack | false |

### 4.3 创建基础蓝图类

| 蓝图名 | 父类 | 用途 |
|--------|------|------|
| WBP_MenuBase | `UFPSMenuBase` | 菜单基类 |
| WBP_CommonButton | `UFPSCommonButton` | 按钮基类 |
| WBP_SettingRow | `UFPSSettingRow` | 设置行基类 |

---

## 第5步：菜单管理器初始化

在 `GameInstance` 中初始化菜单管理器：

```cpp
void UFPSTGameInstance::Init()
{
    Super::Init();
    
    // 加载设置
    if (UFPSMenuManager* MenuManager = GetSubsystem<UFPSMenuManager>())
    {
        MenuManager->LoadAllSettings();
    }
}
```

---

## 第6步：创建主菜单

### 6.1 创建蓝图

创建 `WBP_MainMenu` 继承自 `WBP_MenuBase`：

```
CanvasPanel
├── BackgroundImage (Image)
│   └── 设置背景图
├── SafeZone
│   └── VerticalBox (Content)
│       ├── Logo (Image)
│       ├── Spacer
│       └── ButtonContainer (VerticalBox)
│           ├── BTN_Continue (WBP_CommonButton)
│           ├── BTN_NewGame (WBP_CommonButton)
│           ├── BTN_Settings (WBP_CommonButton)
│           ├── BTN_Achievements (WBP_CommonButton)
│           └── BTN_Quit (WBP_CommonButton)
└── VersionText (CommonTextBlock)
```

### 6.2 配置按钮

在 `WBP_CommonButton` 中：
- 设置 Normal/Hovered/Pressed 图片
- 配置文字样式
- 绑定 Hover/Click 音效

### 6.3 实现功能

在 `WBP_MainMenu` 的 Graph 中：

```
Event Construct
│
├─► SettingsButton.OnClicked
│   └── GetMenuManager → OpenSettingsMenu
│
├─► QuitButton.OnClicked
│   └── ShowConfirmPopup
│       ├── Title: "确认退出"
│       ├── Message: "确定要退出游戏吗？"
│       ├── OnConfirmed: QuitGame
│       └── OnCancelled: 关闭弹窗
│
└─► NewGameButton.OnClicked
    └── CloseMainMenuAndStartGame
```

---

## 第7步：创建设置菜单

### 7.1 创建 Tab 结构

创建 `WBP_SettingsMenu`：

```
VerticalBox
├── Header (HorizontalBox)
│   ├── BackButton
│   └── TitleText: "设置"
├── TabContainer (HorizontalBox)
│   ├── BTN_Graphics (CommonButton)
│   ├── BTN_Audio (CommonButton)
│   ├── BTN_Controls (CommonButton)
│   └── BTN_Gameplay (CommonButton)
├── Content (WidgetSwitcher)
│   ├── GraphicsPanel
│   ├── AudioPanel
│   ├── ControlsPanel
│   └── GameplayPanel
└── BottomBar (HorizontalBox)
    ├── ApplyButton
    └── ResetButton
```

### 7.2 Tab 切换逻辑

```
TabButton.OnClicked
└── WidgetSwitcher.SetActiveWidgetIndex(TabIndex)
```

---

## 第8步：创建图形设置

### 8.1 创建设置行

创建 `WBP_GraphicsSettings`：

```
ScrollBox
└── VerticalBox
    ├── OverallQualityRow (WBP_SettingCombo)
    ├── ResolutionRow (WBP_SettingCombo)
    ├── WindowModeRow (WBP_SettingCombo)
    ├── FrameRateLimitRow (WBP_SettingSlider)
    ├── VSyncRow (WBP_SettingToggle)
    ├── FOVRow (WBP_SettingSlider)
    ├── AAQualityRow (WBP_SettingCombo)
    ├── ShadowQualityRow (WBP_SettingCombo)
    ├── TextureQualityRow (WBP_SettingCombo)
    ├── EffectsQualityRow (WBP_SettingCombo)
    └── ViewDistanceRow (WBP_SettingCombo)
```

### 8.2 填充选项

在 `Construct` 事件中：

```
ResolutionCombo.ClearOptions
For Each AvailableResolution
└── ResolutionCombo.AddOption(Resolution.ToString)
```

### 8.3 应用设置

```
ApplyButton.OnClicked
├── GetGameSettings
│   ├── Resolution = ResolutionCombo.GetSelectedValue
│   ├── WindowMode = WindowModeCombo.GetSelectedIndex
│   └── ...
├── GetGameSettings.ApplyGraphicsSettings
└── GetGameSettings.SaveSettings
```

---

## 第9步：创建键位绑定

### 9.1 创建键位绑定行

创建 `WBP_SettingKeyBind`：

```
HorizontalBox
├── LabelText (CommonTextBlock)
├── Spacer (SizeBox)
└── KeyBindButton (CommonButton)
    └── KeyText (CommonTextBlock)
```

### 9.2 实现绑定逻辑

```cpp
// 在蓝图中
void StartRebinding()
{
    bWaitingForInput = true;
    KeyText.SetText("按任意键...");
    
    // 监听输入
    BindEventToOnKeyDown(NativeOnKeyDown);
}

FEventReply OnKeyDown(FKeyEvent KeyEvent)
{
    if (bWaitingForInput)
    {
        FKey PressedKey = KeyEvent.GetKey();
        
        // 忽略无效键
        if (PressedKey == EKeys::Escape)
        {
            CancelRebinding();
            return FEventReply(true);
        }
        
        // 检查冲突
        if (CheckKeyConflict(PressedKey))
        {
            ShowConflictPopup();
            return FEventReply(true);
        }
        
        // 完成绑定
        FinishRebinding(PressedKey);
        return FEventReply(true);
    }
    return FEventReply(false);
}
```

### 9.3 在设置页面中使用

```
ControlSettingsPanel
└── VerticalBox
    ├── MouseSensitivity (WBP_SettingSlider)
    ├── InvertY (WBP_SettingToggle)
    ├── CategoryLabel: "移动"
    ├── MoveForward (WBP_SettingKeyBind) - InputAction: "IA_MoveForward"
    ├── MoveBackward (WBP_SettingKeyBind) - InputAction: "IA_MoveBackward"
    ├── CategoryLabel: "战斗"
    ├── Fire (WBP_SettingKeyBind) - InputAction: "IA_Fire"
    ├── Aim (WBP_SettingKeyBind) - InputAction: "IA_Aim"
    └── ...
```

---

## 第10步：暂停菜单集成

### 10.1 创建暂停菜单

创建 `WBP_PauseMenu` 继承自 `WBP_MenuBase`：

设置 `bIsPauseMenu = true`

### 10.2 绑定ESC键

在 `FPSPlayerController` 中：

```cpp
void AFPSPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    // Enhanced Input绑定
    EnhancedInputComponent->BindAction(IA_Pause, ETriggerEvent::Started, this, &AFPSPlayerController::TogglePauseMenu);
}

void AFPSPlayerController::TogglePauseMenu()
{
    if (UFPSMenuManager* MenuManager = GetGameInstance()->GetSubsystem<UFPSMenuManager>())
    {
        MenuManager->TogglePauseMenu();
    }
}
```

### 10.3 暂停逻辑

```cpp
void UFPSMenuManager::TogglePauseMenu()
{
    if (IsPauseMenuOpen())
    {
        // 关闭菜单
        PopMenu();
        SetGamePaused(false);
        SwitchToGameplayMode();
    }
    else
    {
        // 打开菜单
        SetGamePaused(true);
        SwitchToUIMode();
        PushMenu(PauseMenuClass);
    }
}
```

---

## 第11步：测试清单

### 功能测试

| 测试项 | 预期结果 |
|--------|----------|
| 点击Settings | 打开设置菜单 |
| Tab切换 | 各设置页面正常显示 |
| 修改设置 | 值正确保存到SaveGame |
| 图形设置应用 | 分辨率/窗口模式即时生效 |
| 音量调节 | 实时听到音量变化 |
| 键位绑定 | 按下新键后立即更新 |
| 键位冲突 | 弹出冲突警告 |
| ESC键 | 正确返回上级菜单 |
| 手柄导航 | 方向键/AB键正常工作 |

### 平台测试

| 平台 | 测试内容 |
|------|----------|
| PC (键鼠) | 鼠标悬停效果、点击响应 |
| PC (手柄) | 方向导航、图标显示 |
| 主机 | 平台合规检查 |

---

## 常见问题

### Q: 菜单不接收输入？

检查以下几点：
1. 是否正确设置了 `CommonGameViewportClient`？
2. Input Action Data Table 是否正确配置？
3. Widget 是否正确添加到 `MenuLayer`？
4. 是否有其他Widget遮挡了输入？

### Q: 键位绑定不生效？

1. 确认 `CustomKeyBindings` 已保存
2. 检查 `ApplyControlSettings` 是否正确调用了 Enhanced Input API
3. 确认调用 `RequestRebuildControlMappings`

### Q: 设置不保存？

1. 检查 `SaveGameToSlot` 是否成功
2. 确认 SaveGame 类已正确标记 `UCLASS()`
3. 检查存储路径是否有写权限

### Q: 手柄图标不显示？

1. 确认 `CommonInputSubsystem` 检测到正确输入类型
2. 检查 Input Icon Set 是否配置
3. 确认 `CommonButton` 的 `InputActionRowHandle` 已设置

---

## 下一步

完成基础菜单后，可以添加：

1. **成就系统页面** - 显示游戏成就列表
2. **统计信息页面** - 玩家游戏数据统计
3. **Credits页面** - 制作人员名单
4. **DLC/商店页面** - 游戏内商店
5. **社交功能** - 好友列表、邀请
