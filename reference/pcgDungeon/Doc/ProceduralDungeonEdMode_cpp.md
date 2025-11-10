# ProceduralDungeonEdMode.cpp

## 概述

`ProceduralDungeonEdMode.cpp` 是 `FProceduralDungeonEdMode` 类的实现文件。它包含了编辑器模式的核心逻辑，包括工具管理、生命周期处理、用户输入路由和与关卡数据的交互。

## 依赖项

- `ProceduralDungeonEdMode.h`: 类的声明。
- `ProceduralDungeonEditor.h`: 获取插件模块。
- `Toolkits/ToolkitManager.h`: 管理工具包。
- `EditorModeManager.h`: 管理编辑器模式。
- `Engine/LevelScriptBlueprint.h`: 关卡蓝图类。
- `ProceduralDungeonEdModeToolkit.h`: 模式的 UI 工具包。
- `ProceduralDungeonEdLog.h`: 编辑器日志宏。
- `ProceduralDungeonEditorSettings.h`: 插件设置。
- `ProceduralDungeonEditorObject.h`: 编辑器对象（设置容器）。
- `Tools/...`: 各种编辑器工具。
- `Room.h`, `RoomLevel.h`, `RoomData.h`: 地牢相关类。

## 实现细节

### 宏定义

- `#define ROUTE_TO_TOOL(FuncCall)`: 一个便捷宏，用于将 FEdMode 的输入/查询函数路由到当前激活的 `ActiveTool`。如果 `ActiveTool` 为空，则调用基类 `FEdMode` 的对应方法。

### 静态成员初始化

- `const FEditorModeID FProceduralDungeonEdMode::EM_ProceduralDungeon(...)`: 初始化编辑器模式的唯一 ID。

### 构造函数

- `FProceduralDungeonEdMode::FProceduralDungeonEdMode()`: 构造函数。
  - 创建 `FProceduralDungeonEditorTool_Size` 和 `FProceduralDungeonEditorTool_Door` 的实例，并添加到 `Tools` 数组中。
  - 创建一个 `UProceduralDungeonEditorObject` 实例并赋值给 `Settings`，用于存储编辑器设置。

### AddReferencedObjects

- `AddReferencedObjects(...)`: 调用基类方法，然后使用 `Collector.AddReferencedObject(Settings)` 确保 `Settings` 对象不会被垃圾回收。

### Enter

- `Enter()`: 进入模式。
  - 调用基类 `Enter()`。
  - 如果 `Toolkit` 无效，则创建并初始化 `FProceduralDungeonEdModeToolkit`。
  - 调用 `UpdateLevelBlueprint()` 更新关卡信息。
  - 设置 `ARoomLevel::bIsDungeonEditorMode = true`，通知 `ARoomLevel` 当前处于编辑器模式。

### Exit

- `Exit()`: 退出模式。
  - 注销关卡蓝图编译委托。
  - 关闭并重置 `Toolkit`。
  - 如果有 `ActiveTool`，调用其 `ExitTool()` 方法并置空指针。
  - 重置 `CachedLevelInstance` 和 `CachedLevelBlueprint`。
  - 设置 `ARoomLevel::bIsDungeonEditorMode = false`。
  - 调用基类 `Exit()`。

### Render / Tick

- `Render(...)`: 如果有 `ActiveTool`，则调用其 `Render` 方法，然后调用基类 `Render`。
- `Tick(...)`: 调用基类 `Tick`，然后如果有 `ActiveTool`，则调用其 `Tick` 方法。

### 输入处理 (HandleClick, InputKey, InputAxis, InputDelta, MouseMove)

- 这些方法都使用 `ROUTE_TO_TOOL` 宏将调用转发给 `ActiveTool`。如果 `ActiveTool` 为空，则调用 `FEdMode` 的基类实现。

### Widget 相关方法 (ShowModeWidgets, ShouldDrawWidget, UsesTransformWidget, GetWidgetLocation, AllowWidgetMove, GetPivotForOrbit, GetCursor)

- `ShowModeWidgets()`: 返回 `true`。
- `ShouldDrawWidget()`: 返回 `true`。
- `UsesTransformWidget()`: 使用 `ROUTE_TO_TOOL` 宏。
- `UsesTransformWidget(WidgetMode)`: 使用 `ROUTE_TO_TOOL` 宏。
- `GetWidgetLocation()`: 使用 `ROUTE_TO_TOOL` 宏。
- `AllowWidgetMove()`: 返回 `true`。
- `GetPivotForOrbit(...)`: 根据 `UProceduralDungeonEditorSettings` 中的 `bUseRoomAsOrbitPivot` 设置，决定摄像机环绕的中心点是否为房间中心。
- `GetCursor(...)`: 使用 `ROUTE_TO_TOOL` 宏。

### 工具管理方法

- `GetTool(...)`: 遍历 `Tools` 数组，根据 `ToolName` 查找并返回对应的工具实例。
- `GetActiveTool()`: 返回 `ActiveTool` 指针。
- `SetActiveTool(FName)`: 根据名称查找工具，然后调用 `SetActiveTool(FProceduralDungeonEditorTool*)`。
- `ResetActiveTool()`: 调用 `SetActiveTool(nullptr)`。
- `SetActiveTool(FProceduralDungeonEditorTool*)`: 核心的工具切换逻辑。
  - 如果当前有 `ActiveTool`，调用其 `ExitTool()`。
  - 更新 `ActiveTool` 指针。
  - 如果新的 `NewTool` 不为空，调用其 `EnterTool()`。
- `SetDefaultTool()`: 如果当前没有激活工具且 "Tool_Size" 可用，则激活 "Tool_Size" 工具。
- `IsToolEnabled(...)`: 检查 `GetLevel()` 返回的关卡和其 `Data` 是否有效。

### 关卡数据交互方法

- `GetLevelBlueprint(...)`: 获取当前世界持久关卡的 `ULevelScriptBlueprint`。
- `GetLevel()`: 获取关卡蓝图生成的默认 `ARoomLevel` 对象。
- `UpdateLevelBlueprint()`: 更新对当前关卡蓝图的缓存。
  - 如果新的蓝图与缓存的不同，则先注销旧蓝图的委托。
  - 更新 `CachedLevelBlueprint`。
  - 如果新蓝图有效，则注册其编译完成委托。
  - 调用 `OnLevelBlueprintCompiled()`。
- `OnLevelBlueprintCompiled(...)`: 当关卡蓝图编译完成时的回调。
  - 更新 `CachedLevelInstance` 为当前世界中的 `LevelScriptActor`。
  - 记录日志。
  - 调用 `SetDefaultTool()` 或 `ResetActiveTool()`。
  - 获取 `FProceduralDungeonEdModeToolkit` 实例并调用其 `OnLevelChanged()`。
  - 如果有 `ActiveTool`，调用其 `OnLevelChanged()`。
- `RegisterLevelCompilationDelegate(bool)`: 注册或注销关卡蓝图编译完成的委托。
  - 检查 `CachedLevelBlueprint` 是否有效。
  - 根据 `Register` 参数和 `LevelBlueprintDelegateHandle` 的状态，添加或移除委托。