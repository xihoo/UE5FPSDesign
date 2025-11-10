# ProceduralDungeonEditorCommands.cpp

## 概述

`ProceduralDungeonEditorCommands.cpp` 是 `FProceduralDungeonEditorCommands` 类的实现文件。它包含了命令的注册逻辑。

## 依赖项

- `ProceduralDungeonEditorCommands.h`: 类的声明。
- `Tools/ProceduralDungeonEditorTool_Size.h`: 用于获取 "Tool_Size" 的名称（虽然头文件被包含，但 cpp 文件中未直接使用其类）。
- `Tools/ProceduralDungeonEditorTool_Door.h`: 用于获取 "Tool_Door" 的名称（同上）。

## 实现细节

### 静态成员初始化

- `FName FProceduralDungeonEditorCommands::ProceduralDungeonContext = TEXT("ProceduralDungeonEditor");`: 初始化静态成员 `ProceduralDungeonContext`，设置为 "ProceduralDungeonEditor"。

### 构造函数

- `FProceduralDungeonEditorCommands::FProceduralDungeonEditorCommands()`: 构造函数。
  - 调用基类 `TCommands<FProceduralDungeonEditorCommands>` 的构造函数。
  - 传入参数：
    - `ProceduralDungeonContext`: 命令上下文的名称。
    - `NSLOCTEXT(...)`: 用于本地化显示的上下文名称。
    - `NAME_None`: 父级上下文，这里没有父级。
    - `FName("ProceduralDungeonStyle")`: 图标样式集的名称，与插件注册的 `FSlateStyleSet` 名称一致。

### RegisterCommands

- `void FProceduralDungeonEditorCommands::RegisterCommands()`: 注册具体的 UI 命令。
  - `UI_COMMAND(SizeTool, ...)`: 使用 `UI_COMMAND` 宏创建 "Size" 工具的命令。
    - `SizeTool`: 要创建的 `TSharedPtr<FUICommandInfo>` 成员变量。
    - `"Size"`: 命令的显示名称。
    - `"Change room size by dragging points in the viewport."`: 命令的提示信息（Tooltip）。
    - `EUserInterfaceActionType::RadioButton`: 动作类型，设置为单选按钮，意味着在一组工具中只能选择一个。
    - `FInputChord()`: 快捷键，这里未设置。
  - `NameToCommandMap.Add("Tool_Size", SizeTool);`: 将工具名称 "Tool_Size" 与创建的 `SizeTool` 命令对象关联起来，存入 `NameToCommandMap`。
  - `UI_COMMAND(DoorTool, ...)`: 使用 `UI_COMMAND` 宏创建 "Door" 工具的命令，参数含义与 `SizeTool` 类似。
  - `NameToCommandMap.Add("Tool_Door", DoorTool);`: 将工具名称 "Tool_Door" 与 `DoorTool` 命令对象关联起来。