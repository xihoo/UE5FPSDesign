# ProceduralDungeonEdModeToolkit.h

## 概述

`ProceduralDungeonEdModeToolkit.h` 定义了 `FProceduralDungeonEdModeToolkit` 类。这个类继承自 `FModeToolkit`，是“地牢房间编辑模式”的用户界面（UI）工具包。它负责创建和管理模式的工具栏、属性面板以及与编辑器模式核心逻辑的交互。

## 类定义

### FProceduralDungeonEdModeToolkit

此类是编辑器模式 UI 的入口点。

#### 重写的 FModeToolkit 接口

- `virtual void Init(...) override`: 初始化工具包，设置命令绑定和创建主 UI Widget。
- `virtual void GetToolPaletteNames(...) override`: 获取工具调色板的名称列表。此实现添加了 "DefaultPalette"。
- `virtual FText GetToolPaletteDisplayName(...) override`: 获取工具调色板的显示名称。此实现将 "DefaultPalette" 显示为 "Default"。
- `virtual void BuildToolPalette(...) override`: 构建指定调色板的工具栏。此实现构建了包含工具按钮和属性编辑器的默认工具栏。

#### 重写的 IToolkit 接口

- `virtual FName GetToolkitFName() const override`: 返回工具包的唯一名称 "ProceduralDungeonEdMode"。
- `virtual FText GetBaseToolkitName() const override`: 返回工具包的本地化显示名称 "ProceduralDungeonEdMode Tool"。
- `virtual FEdMode* GetEditorMode() const override`: 获取与此工具包关联的 `FEdMode` 实例（即 `FProceduralDungeonEdMode`）。
- `virtual TSharedPtr<class SWidget> GetInlineContent() const override`: 返回嵌入到模式 UI 中的主要 Slate Widget（即 `EdModeWidget`）。

#### 公共方法

- `class FProceduralDungeonEdMode* GetDungeonEditorMode() const`: 便捷方法，将 `GetEditorMode()` 的返回值转换为 `FProceduralDungeonEdMode*`。
- `void OnChangeTool(FName ToolName) const`: 当用户通过 UI 选择一个工具时调用，通知 `FProceduralDungeonEdMode` 切换激活的工具。
- `bool IsToolEnabled(FName ToolName) const`: 查询一个工具是否可用。
- `bool IsToolActive(FName ToolName) const`: 查询一个工具是否是当前激活的工具。
- `bool IsDoorTypeEnabled() const`: 查询 DoorType 属性编辑器是否应启用（当 Door 工具可用时启用）。
- `void OnLevelChanged()`: 当编辑的关卡发生变化时调用，通知主 UI Widget 更新。

#### 私有成员

- `TSharedPtr<class SProceduralDungeonEdModeWidget> EdModeWidget`: 指向主 UI Widget 的共享指针。