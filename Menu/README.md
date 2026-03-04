# UE5 CommonUI Menu 系统

本目录包含基于 **UE5 CommonUI Plugin** 的完整菜单系统设计方案。

## 📁 文档结构

| 文档 | 说明 |
|------|------|
| **[Menu.md](./Menu.md)** | 完整的设计文档，包含架构设计、核心类定义、设置系统详细设计 |
| **[MenuArchitecture.md](./MenuArchitecture.md)** | 架构图、类图、数据流图 |
| **[QuickStart.md](./QuickStart.md)** | 快速开始指南，10步完成Menu系统集成 |
| **[Implementation.md](./Implementation.md)** | C++实现代码示例 |
| **[AdvancedGraphics.md](./AdvancedGraphics.md)** | DLSS、FSR、XeSS、帧生成、光追等高级图形设置 |
| **[UMGWorkflow.md](./UMGWorkflow.md)** | 基于C++基类的UMG制作详细流程 |
| **[UMGExamples.md](./UMGExamples.md)** | UMG制作实例，含蓝图节点详解 |
| **[CppToUMGFlow.md](./CppToUMGFlow.md)** | C++到UMG绑定流程图和排查指南 |

## 🎯 核心特性

### 支持的菜单功能

- ✅ **主菜单** - 继续游戏、新游戏、设置、成就、退出
- ✅ **暂停菜单** - 继续、设置、返回主菜单、退出
- ✅ **设置菜单** - 4个分类页面
  - **图形设置** - 基础设置 + 高级图形技术
    - 基础：分辨率、画质、FOV、窗口模式等
    - 超采样：DLSS、FSR、XeSS、TSR，含帧生成
    - 性能：NVIDIA Reflex、动态分辨率
    - HDR：亮度、纸白设置
    - 光追：RT反射、阴影、全局光照
  - **音频设置** - 主音量、分类音量、字幕等
  - **控制设置** - 鼠标/手柄灵敏度、键位绑定等
  - **游戏设置** - 难度、HUD选项等

### 技术亮点

| 特性 | 说明 |
|------|------|
| **CommonUI框架** | 使用UE5官方CommonUI插件，支持跨平台输入 |
| **数据持久化** | 基于SaveGame系统，自动保存/加载设置 |
| **实时预览** | 图形/音频设置即时生效 |
| **键位绑定** | 完整键位重绑定，冲突检测，与Enhanced Input集成 |
| **输入无关** | 键鼠/手柄/触屏统一处理，动态图标 |
| **GAS集成** | 键位绑定与Gameplay Ability System无缝集成 |
| **DLSS/FSR/XeSS** | 超分辨率技术支持，自动检测可用性 |
| **帧生成** | DLSS 3 / FSR 3 帧生成功能 |
| **光线追踪** | RT反射、阴影、全局光照设置 |
| **NVIDIA Reflex** | 低延迟模式设置 |
| **自动优化** | 根据GPU性能自动推荐最佳设置 |

## 🏗️ 系统架构

```
┌─────────────────────────────────────────────────────────────┐
│                      Menu System                             │
├─────────────────────────────────────────────────────────────┤
│  UFPSMenuManager (GameInstanceSubsystem)                     │
│  ├── MenuLayer (ActivatableWidgetContainer)                  │
│  ├── PopupLayer (ActivatableWidgetContainer)                 │
│  └── GameSettings (SaveGame)                                 │
├─────────────────────────────────────────────────────────────┤
│  UFPSMenuBase (CommonActivatableWidget)                      │
│  ├── UFPSMainMenu                                            │
│  ├── UFPSPauseMenu                                           │
│  └── UFPSSettingsMenu                                        │
│      ├── UFPSGraphicsSettingsWidget                          │
│      │   ├── 基础图形设置                                     │
│      │   ├── 高级图形 (DLSS/FSR/XeSS/FG)                     │
│      │   ├── HDR设置                                         │
│      │   └── 光线追踪设置                                     │
│      ├── UFPSAudioSettingsWidget                             │
│      ├── UFPSControlSettingsWidget                           │
│      └── UFPSGameplaySettingsWidget                          │
├─────────────────────────────────────────────────────────────┤
│  Settings Components                                         │
│  ├── UFPSSettingRow (Base)                                   │
│  ├── UFPSSettingSlider                                       │
│  ├── UFPSSettingCombo                                        │
│  ├── UFPSSettingToggle                                       │
│  └── UFPSSettingKeyBind                                      │
└─────────────────────────────────────────────────────────────┘
```

## 🚀 快速开始

```cpp
// 1. 在GameInstance中初始化
void UMyGameInstance::Init() {
    if (UFPSMenuManager* MenuManager = GetSubsystem<UFPSMenuManager>()) {
        MenuManager->LoadAllSettings();
    }
}

// 2. 打开主菜单
MenuManager->OpenMainMenu();

// 3. 切换暂停菜单
MenuManager->TogglePauseMenu();

// 4. 显示确认弹窗
MenuManager->ShowConfirmPopup(
    FText::FromString(TEXT("确认退出")),
    FText::FromString(TEXT("确定要退出游戏吗？")),
    FText::FromString(TEXT("确认")),
    FText::FromString(TEXT("取消")),
    FOnMenuPopupConfirmed::CreateLambda([]() { FGenericPlatformMisc::RequestExit(false); }),
    FOnMenuPopupCancelled::CreateLambda([]() { /* Do nothing */ })
);
```

## 📋 实施清单

### 必需插件
- ✅ CommonUI Plugin
- ✅ CommonInput Plugin
- ✅ EnhancedInput Plugin
- ✅ GameplayAbilities Plugin

### 必需C++类
1. `UFPSGameSettings` - SaveGame类，存储所有设置
2. `UFPSMenuManager` - GameInstanceSubsystem，管理菜单生命周期
3. `UFPSMenuBase` - 菜单基类
4. `UFPSCommonButton` - 统一按钮样式
5. `UFPSSettingRow` - 设置行基类
6. `UFPSSettingKeyBind` - 键位绑定组件

### 必需蓝图
1. `WBP_MenuBase` - 菜单基类蓝图
2. `WBP_CommonButton` - 按钮基类蓝图
3. `WBP_MainMenu` - 主菜单
4. `WBP_PauseMenu` - 暂停菜单
5. `WBP_SettingsMenu` - 设置菜单
6. `DT_MenuInputActions` - 菜单输入动作数据表

## 🔗 与原有系统集成

本Menu系统设计时已考虑与项目已有系统的集成：

### 与Input系统的集成
- 菜单中的键位绑定直接修改Enhanced Input的映射
- 使用数据驱动的方式绑定 `InputAction → GameplayTag → Ability`

### 与GAS的集成
- 键位绑定变更后自动通知AbilitySystemComponent
- 支持通过数据资产热更新键位映射

### 与Save系统的集成
- 设置自动保存到SaveGame Slot
- 支持跨会话持久化

## 📚 参考文档

- [UE5 CommonUI Documentation](https://docs.unrealengine.com/5.0/en-US/common-ui-plugin-in-unreal-engine/)
- [Enhanced Input](https://docs.unrealengine.com/5.0/en-US/enhanced-input-in-unreal-engine/)
- [Gameplay Ability System](https://docs.unrealengine.com/5.0/en-US/gameplay-ability-system-for-unreal-engine/)

## 📝 注意事项

1. **项目配置**：确保在`.uproject`中启用CommonUI相关插件
2. **Build.cs**：添加`CommonUI`、`CommonInput`、`CommonGame`模块依赖
3. **GameViewportClient**：替换为`UCommonGameViewportClient`子类
4. **中文支持**：所有UI文本使用`FText`和本地化系统

---

**设计日期**: 2026-03-04  
**版本**: 1.0  
**作者**: AI Assistant
