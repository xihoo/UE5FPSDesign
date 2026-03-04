# Menu 系统架构图

## 1. 整体架构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          GameInstance                                        │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    UFPSMenuManager (Subsystem)                       │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌────────────┐  │   │
│  │  │ MenuLayer   │  │ PopupLayer  │  │ GameSettings│  │ InputRouter│  │   │
│  │  │ (Widget     │  │ (Widget     │  │ (SaveGame)  │  │ (CommonUI) │  │   │
│  │  │  Container) │  │  Container) │  │             │  │            │  │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘  └────────────┘  │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      │ Push/Pop
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                            Menu Stack                                        │
│                                                                             │
│   ┌──────────────┐     ┌──────────────┐     ┌──────────────┐               │
│   │ UFPSMenuBase │◄────│ UFPSMenuBase │◄────│ UFPSMenuBase │               │
│   │ (当前显示)    │     │ (已暂停)      │     │ (已暂停)      │               │
│   └──────────────┘     └──────────────┘     └──────────────┘               │
│          │                                                                  │
│          │ Activatable Widget Lifecycle                                      │
│          ▼                                                                  │
│   ┌─────────────────────────────────────────────────────────────────┐      │
│   │                    CommonActivatableWidget                       │      │
│   │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐  │      │
│   │  │ NativeOnActivate()                                          │  │      │
│   │  │ NativeOnDeactivate()                                        │  │      │
│   │  │ NativeGetDesiredFocusTarget()                               │  │      │
│   │  └─────────────┘  └─────────────┘  └─────────────────────────┘  │      │
│   └─────────────────────────────────────────────────────────────────┘      │
└─────────────────────────────────────────────────────────────────────────────┘
                                      │
                    ┌─────────────────┼─────────────────┐
                    │                 │                 │
                    ▼                 ▼                 ▼
         ┌──────────────┐  ┌──────────────┐  ┌──────────────┐
         │  UFPSMainMenu │  │UFPSPauseMenu │  │UFPSSettingsMenu│
         └──────────────┘  └──────────────┘  └──────────────┘
                                                      │
                        ┌─────────────────────────────┼─────────────────────────────┐
                        │                             │                             │
                        ▼                             ▼                             ▼
              ┌──────────────────┐        ┌──────────────────┐        ┌──────────────────┐
              │GraphicsSettings  │        │ AudioSettings    │        │ ControlSettings  │
              └──────────────────┘        └──────────────────┘        └──────────────────┘
```

---

## 2. 设置数据流

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           设置数据流                                         │
└─────────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────┐
  │  用户操作UI      │
  │ (Slider/Combo等) │
  └────────┬────────┘
           │ 修改值
           ▼
  ┌─────────────────┐
  │ UFPSSettingRow  │
  │ (OnValueChanged) │
  └────────┬────────┘
           │ 委托通知
           ▼
  ┌─────────────────┐
  │  Settings页面    │
  │ (收集所有值)      │
  └────────┬────────┘
           │ 点击应用
           ▼
  ┌─────────────────┐     ┌─────────────────┐
  │UFPSGameSettings │────►│  ApplySettings() │
  │  (SaveGame)     │     │  (即时生效)       │
  └────────┬────────┘     └─────────────────┘
           │
           │ SaveGameToSlot()
           ▼
  ┌─────────────────┐
  │   SaveGame文件   │
  │  (LocalSlot)    │
  └─────────────────┘
```

---

## 3. 键位绑定流程

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         键位绑定流程                                         │
└─────────────────────────────────────────────────────────────────────────────┘

  ┌──────────────────────────────────────────────────────────────┐
  │ 1. 用户点击键位绑定按钮                                       │
  │    UFPSSettingKeyBind::StartRebinding()                       │
  │    - bWaitingForInput = true                                  │
  │    - 显示"等待输入..."                                        │
  └───────────────────────────┬──────────────────────────────────┘
                              │
                              ▼
  ┌──────────────────────────────────────────────────────────────┐
  │ 2. 捕获输入 (Widget级别)                                      │
  │    NativeOnKeyDown() / NativeOnMouseButtonDown()             │
  │    - 忽略无效键 (Escape, MouseMove等)                         │
  │    - 检查键位冲突                                              │
  └───────────────────────────┬──────────────────────────────────┘
                              │
                              ▼
  ┌──────────────────────────────────────────────────────────────┐
  │ 3. 验证与冲突处理                                              │
  │    CheckKeyConflict()                                        │
  │    ├── 无冲突 ──► FinishRebinding()                          │
  │    └── 有冲突 ──► ShowKeyConflictPopup()                     │
  │         ├── 用户选择覆盖 ──► FinishRebinding()               │
  │         └── 用户选择取消 ──► CancelRebinding()               │
  └───────────────────────────┬──────────────────────────────────┘
                              │
                              ▼
  ┌──────────────────────────────────────────────────────────────┐
  │ 4. 保存并应用                                                  │
  │    - 更新 UFPSGameSettings::CustomKeyBindings                │
  │    - 调用 ApplyControlSettings()                             │
  │    - Enhanced Input Subsystem 更新绑定                        │
  │    - SaveSettings() 持久化                                    │
  └──────────────────────────────────────────────────────────────┘
```

---

## 4. CommonUI 输入路由

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                     CommonUI 输入路由系统                                    │
└─────────────────────────────────────────────────────────────────────────────┘

                    Input Device
                         │
                         ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                    UCommonInputSubsystem                                    │
│  - 检测设备类型 (Mouse/Keyboard/Gamepad/Touch)                               │
│  - 管理输入图标                                                              │
└───────────────────────────┬─────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                   UCommonUIActionRouter                                     │
│  - 路由输入到当前活跃的ActivatableWidget                                    │
│  - 处理导航 (Up/Down/Left/Right)                                            │
│  - 处理确认/取消 (Confirm/Cancel)                                           │
└───────────────────────────┬─────────────────────────────────────────────────┘
                            │
            ┌───────────────┼───────────────┐
            │               │               │
            ▼               ▼               ▼
┌─────────────────┐ ┌─────────────┐ ┌─────────────────┐
│  UFPSMenuBase   │ │ UFPSPopup   │ │ UFPSSettingRow  │
│  (BackAction)   │ │ (Confirm)   │ │ (ValueChange)   │
└─────────────────┘ └─────────────┘ └─────────────────┘
```

---

## 5. 与原有系统的关系

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      Menu系统与原有系统的集成                                 │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│                              Menu系统                                        │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐             │
│  │ 图形/音频设置    │  │   键位绑定       │  │  游戏难度设置    │             │
│  │ (即时生效)       │  │ (绑定到InputAction│  │ (保存到存档)     │             │
│  └────────┬────────┘  └────────┬────────┘  └────────┬────────┘             │
└───────────┼────────────────────┼────────────────────┼──────────────────────┘
            │                    │                    │
            │                    │                    │
            ▼                    │                    ▼
┌──────────────────────┐         │          ┌──────────────────────┐
│   UE5 Engine APIs    │         │          │    SaveGame系统       │
│  - Scalability       │         │          │  - GameDifficulty     │
│  - Audio Device      │         │          │  - Progress           │
│  - Window Mode       │         │          └──────────────────────┘
└──────────────────────┘         │
                                 │
                                 ▼
            ┌─────────────────────────────────────────────────────┐
            │              原有Input系统设计                       │
            │  ┌─────────────────────────────────────────────────┐│
            │  │  InputAction ──► InputTag ──► GameplayAbility   ││
            │  │                                                  ││
            │  │  IA_Fire ──────► Input.Fire.Primary             ││
            │  │                  ──► Ability.Weapon.Fire.Auto   ││
            │  └─────────────────────────────────────────────────┘│
            └─────────────────────────────────────────────────────┘
```

---

## 6. 类图

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                               类图                                          │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────┐
│  UGameInstanceSubsystem │
└───────────┬─────────────┘
            │
            ▼
┌─────────────────────────┐      ┌─────────────────────────┐
│    UFPSMenuManager      │──────│ UCommonActivatableWidget│
│  - MenuLayer            │      └───────────┬─────────────┘
│  - PopupLayer           │                  │
│  - GameSettings         │                  ▼
│  - PushMenu()           │      ┌─────────────────────────┐
│  - PopMenu()            │      │     UFPSMenuBase        │◄──────────────┐
│  - SaveAllSettings()    │      │  (Abstract)             │               │
└─────────────────────────┘      │  # GetDefaultFocus()    │               │
                                 │  # OnBackPressed()      │               │
                                 │  - bIsPauseMenu         │               │
                                 └───────────┬─────────────┘               │
                                             │                              │
           ┌─────────────────────────────────┼─────────────────────┐       │
           │                                 │                     │       │
           ▼                                 ▼                     ▼       │
┌──────────────────────┐      ┌──────────────────────┐     ┌──────────────┴──────┐
│    UFPSMainMenu      │      │   UFPSPauseMenu      │     │   UFPSSettingsMenu  │
└──────────────────────┘      └──────────────────────┘     └──────────┬──────────┘
                                                                      │
                                          ┌───────────────────────────┼───────────┐
                                          │                           │           │
                                          ▼                           ▼           ▼
                           ┌──────────────────────┐  ┌──────────────────────┐  ┌──────────────────────┐
                           │GraphicsSettingsWidget│  │ AudioSettingsWidget  │  │ControlSettingsWidget │
                           └──────────────────────┘  └──────────────────────┘  └──────────────────────┘

┌─────────────────────────┐      ┌─────────────────────────┐
│  UCommonButtonBase      │      │    USaveGame            │
└───────────┬─────────────┘      └───────────┬─────────────┘
            │                                │
            ▼                                ▼
┌─────────────────────────┐      ┌─────────────────────────┐
│   UFPSCommonButton      │      │   UFPSGameSettings      │
│  - SetButtonText()      │      │  - Graphics Settings    │
│  - SetButtonIcon()      │      │  - Audio Settings       │
│  - Hover/Click Sounds   │      │  - Control Settings     │
└─────────────────────────┘      │  - Apply/Save/Load      │
                                 └─────────────────────────┘

┌─────────────────────────┐
│  UCommonUserWidget      │
└───────────┬─────────────┘
            │
            ▼
┌─────────────────────────┐      ┌─────────────────────────┐
│    UFPSSettingRow       │◄─────│    UFPSSettingKeyBind   │
│  (Abstract)             │      │  - StartRebinding()     │
│  # OnValueChanged       │      │  - CancelRebinding()    │
│  - bIsDirty             │      │  - bWaitingForInput     │
└───────────┬─────────────┘      └─────────────────────────┘
            │
   ┌────────┼────────┐
   │        │        │
   ▼        ▼        ▼
┌────────┐ ┌────────┐ ┌────────┐
│ Slider │ │ Combo  │ │ Toggle │
└────────┘ └────────┘ └────────┘
```

---

## 7. 文件结构

```
Source/FPSGame/
├── Public/
│   └── UI/
│       └── Menu/
│           ├── FPSMenuManager.h
│           ├── FPSMenuBase.h
│           ├── FPSCommonButton.h
│           ├── FPSSettingRow.h
│           ├── FPSSettingKeyBind.h
│           ├── FPSGraphicsSettingsWidget.h
│           ├── FPSAudioSettingsWidget.h
│           ├── FPSControlSettingsWidget.h
│           ├── FPSGameplaySettingsWidget.h
│           └── FPSGameSettings.h
│
├── Private/
│   └── UI/
│       └── Menu/
│           ├── FPSMenuManager.cpp
│           ├── FPSMenuBase.cpp
│           ├── FPSCommonButton.cpp
│           ├── FPSSettingRow.cpp
│           ├── FPSSettingKeyBind.cpp
│           ├── FPSGraphicsSettingsWidget.cpp
│           ├── FPSAudioSettingsWidget.cpp
│           ├── FPSControlSettingsWidget.cpp
│           ├── FPSGameplaySettingsWidget.cpp
│           └── FPSGameSettings.cpp
│
└── FPSGame.Build.cs (添加CommonUI依赖)

Content/UI/Menu/
├── Core/
│   ├── WBP_MenuBase.uasset (抽象基类)
│   ├── WBP_PopupBase.uasset
│   ├── WBP_MenuManager.uasset
│   └── DT_MenuInputActions.uasset
│
├── Components/
│   ├── WBP_CommonButton.uasset
│   ├── WBP_MenuButton.uasset
│   ├── WBP_SettingRow.uasset
│   ├── WBP_SettingSlider.uasset
│   ├── WBP_SettingCombo.uasset
│   ├── WBP_SettingToggle.uasset
│   └── WBP_SettingKeyBind.uasset
│
├── Menus/
│   ├── WBP_MainMenu.uasset
│   ├── WBP_PauseMenu.uasset
│   └── WBP_SettingsMenu.uasset
│
├── Settings/
│   ├── WBP_GraphicsSettings.uasset
│   ├── WBP_AudioSettings.uasset
│   ├── WBP_ControlSettings.uasset
│   └── WBP_GameplaySettings.uasset
│
└── Popups/
    ├── WBP_ConfirmPopup.uasset
    ├── WBP_MessagePopup.uasset
    └── WBP_KeyConflictPopup.uasset
```

---

## 8. 配置检查清单

### 8.1 插件启用

在 `.uproject` 文件中确保以下插件已启用：

```json
{
  "Plugins": [
    { "Name": "CommonUI", "Enabled": true },
    { "Name": "CommonInput", "Enabled": true },
    { "Name": "CommonGame", "Enabled": true },
    { "Name": "GameplayAbilities", "Enabled": true }
  ]
}
```

### 8.2 项目设置

1. **Engine - Input**: 确保 `DefaultViewportMouseCaptureMode` 设置为 `CapturePermanently`
2. **CommonInput**: 配置默认输入类型和图标集
3. **GameViewportClient**: 设置为 `CommonGameViewportClient` 的子类

### 8.3 Build.cs 依赖

```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "Core", "CoreUObject", "Engine", "InputCore",
    "CommonUI", "CommonInput", "CommonGame",
    "EnhancedInput", "GameplayAbilities"
});
```
