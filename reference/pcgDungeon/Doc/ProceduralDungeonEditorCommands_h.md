# ProceduralDungeonEditorCommands.h

## 概述

`ProceduralDungeonEditorCommands.h` 定义了 `FProceduralDungeonEditorCommands` 类。这个类用于管理和注册“地牢房间编辑模式”中使用的 UI 命令，例如切换“大小调整工具”和“门编辑工具”。它继承自 Unreal Engine 的 `TCommands` 模板类，这是创建和管理编辑器命令的标准方式。

## 类定义

### FProceduralDungeonEditorCommands

此类负责定义和注册与地牢编辑器模式相关的命令。

#### 构造函数

- `FProceduralDungeonEditorCommands()`: 构造函数，调用 `TCommands` 的构造函数，传入上下文名称、本地化显示名称、父级上下文和图标样式集名称。

#### 重写的基类方法

- `virtual void RegisterCommands() override`: 纯虚函数的实现，用于实际注册命令。在这个函数中，会使用 `UI_COMMAND` 宏来创建具体的命令对象。

#### 公共静态成员

- `static FName ProceduralDungeonContext`: 定义命令的上下文名称，用于快速查找和分组。

#### 公共成员

- `TSharedPtr<FUICommandInfo> SizeTool`: 指向“大小调整工具”命令的共享指针。
- `TSharedPtr<FUICommandInfo> DoorTool`: 指向“门编辑工具”命令的共享指针。
- `TMap<FName, TSharedPtr<FUICommandInfo>> NameToCommandMap`: 一个映射，将工具的内部名称（如 "Tool_Size"）关联到其对应的 `FUICommandInfo` 对象。这使得可以通过工具名称字符串来查找和激活相应的命令。