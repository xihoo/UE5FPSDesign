# ProceduralDungeonEdModeToolkit.cpp

## 概述

`ProceduralDungeonEdModeToolkit.cpp` 是 `FProceduralDungeonEdModeToolkit` 类的实现文件。它负责初始化工具包、构建工具栏 UI、处理命令绑定以及协调 UI 与编辑器模式核心逻辑之间的交互。

## 依赖项

- `ProceduralDungeonEdModeToolkit.h`: 类的声明。
- `ProceduralDungeonEdMode.h`: 访问编辑器模式。
- `SProceduralDungeonEdModeWidget.h`: 主 UI Widget。
- `EditorModeManager.h`: 管理编辑器模式。
- `ISinglePropertyView.h`: 用于创建单个属性的编辑器控件。
- `ProceduralDungeonEditorCommands.h`: 访问 UI 命令。
- `ProceduralDungeonEditorObject.h`: 访问编辑器设置。
- `ProceduralDungeonEdLog.h`: 编辑器日志宏。
- `Tools/ProceduralDungeonEditorTool.h`: 访问工具基类。

## 实现细节

### Init

- `Init(...)`: 初始化工具包。
  - 获取 `FProceduralDungeonEditorCommands` 的 `NameToCommandMap`。
  - 获取工具包的命令列表 `CommandList`。
  - 使用 `CommandList->MapAction` 将 "Tool_Size" 和 "Tool_Door" 命令与相应的执行、可用性和激活状态检查函数 (`OnChangeTool`, `IsToolEnabled`, `IsToolActive`) 绑定。
  - 使用 `SAssignNew` 创建 `SProceduralDungeonEdModeWidget` 实例并赋值给 `EdModeWidget`。
  - 调用基类 `FModeToolkit::Init` 完成初始化。

### GetToolPaletteNames / GetToolPaletteDisplayName

- `GetToolPaletteNames(...)`: 将 "DefaultPalette" 添加到输出数组。
- `GetToolPaletteDisplayName(...)`: 返回 "DefaultPalette" 对应的本地化文本 "Default"。

### BuildToolPalette

- `BuildToolPalette(...)`: 构建工具栏。
  - 获取 `FProceduralDungeonEditorCommands` 实例。
  - 获取 `FProceduralDungeonEditorObject` (即 `Settings`)。
  - 使用 `FPropertyEditorModule::CreateSingleProperty` 为 `Settings->DoorType` 属性创建一个独立的属性编辑器控件。
  - 将该属性编辑器控件包装在一个 `SHorizontalBox` 中，并设置其启用状态由 `IsDoorTypeEnabled` 决定。
  - 使用 `ToolbarBuilder` 开始一个名为 "Default" 的区域。
  - 添加 "SizeTool" 按钮。
  - 添加分隔符。
  - 添加 "DoorTool" 按钮。
  - 添加之前创建的 DoorType 属性编辑器 Widget。
  - 结束该区域。

### GetEditorMode / GetDungeonEditorMode / GetInlineContent

- `GetEditorMode()`: 通过 `GLevelEditorModeTools().GetActiveMode(...)` 获取 `FProceduralDungeonEdMode` 实例。
- `GetDungeonEditorMode()`: 将 `GetEditorMode()` 的返回值强制转换为 `FProceduralDungeonEdMode*`。
- `GetInlineContent()`: 返回 `EdModeWidget`。

### UI 交互方法

- `OnChangeTool(...)`: 当工具按钮被点击时调用。
  - 获取 `FProceduralDungeonEdMode` 实例。
  - 记录日志。
  - 调用 `EdMode->SetActiveTool(ToolName)` 切换工具。
- `IsToolEnabled(...)`: 调用 `EdMode->IsToolEnabled(ToolName)` 查询工具可用性。
- `IsToolActive(...)`: 获取工具实例，并将其与当前激活的工具进行比较。
- `IsDoorTypeEnabled()`: 返回 `IsToolEnabled("Tool_Door")`，即当 Door 工具可用时，DoorType 属性编辑器也启用。
- `OnLevelChanged()`: 调用 `EdModeWidget->OnLevelChanged()`，通知主 UI Widget 关卡已更改。