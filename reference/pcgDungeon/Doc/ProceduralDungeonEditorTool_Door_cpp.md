# ProceduralDungeonEditorTool_Door.cpp

## 概述

`ProceduralDungeonEditorTool_Door.cpp` 是 `FProceduralDungeonEditorTool_Door` 类的实现文件。它包含了在“地牢房间编辑模式”中添加、移除和预览门的具体逻辑。

## 依赖项

- `ProceduralDungeonEditorTool_Door.h`: 类的声明。
- `Components/BoxComponent.h`: 用于创建临时碰撞体。
- `EditorMode/ProceduralDungeonEdMode.h`: 访问编辑器模式和设置。
- `EditorMode/ProceduralDungeonEditorObject.h`: 访问编辑器对象（如设置中的 DoorType）。
- `ProceduralDungeonEdLog.h`: 编辑器日志宏。
- `ProceduralDungeonUtils.h`: 工具函数，如 `Dungeon::RoomUnit()` 和 `Dungeon::ToWorldLocation`。
- `Room.h`, `RoomLevel.h`, `RoomData.h`, `Door.h`: 相关的地牢数据结构和类。

## 实现细节

### 辅助函数

- `AreDoorsOverlapping(const FDoorDef& A, const FDoorDef& B)`: 检查两个门定义是否在空间上重叠。
- `IsPositionInside(const FDoorDef& Door, const FIntVector& Position)`: 检查一个单元格位置是否在某个门的边界内。
- `IsDoorValid(const URoomData* Data, const FDoorDef& Door)`: 检查一个新门是否与房间数据中已有的门重叠，如果不重叠则认为是有效的。

### FProceduralDungeonEditorTool_Door 方法实现

#### EnterTool / ExitTool

- `EnterTool()`: 记录进入工具的日志，并调用 `UpdateRoomBox()` 来设置碰撞体。
- `ExitTool()`: 记录退出工具的日志，调用 `DestroyRoomBox()` 销毁碰撞体，并重置缓存的关卡指针。

#### Render

- `Render(...)`: 调用基类渲染方法，然后根据当前 `URoomData` 的边界，在视口中绘制一个表示房间网格的线框（垂直和水平线）。

#### Tick

- `Tick(...)`: 调用基类 Tick 方法。如果 `ShowDoorPreview` 为真，则调用 `FDoorDef::DrawDebug` 绘制门的预览。预览的颜色会根据门是否有效（不与现有门重叠）而变化。

#### HandleClick

- `HandleClick(...)`: 处理鼠标点击。
  - 检查是否按下了修饰键（Alt, Ctrl, Shift），如果是则不处理。
  - 检查是否显示门预览。
  - 获取当前关卡和 `URoomData`。
  - **左键点击**: 如果门预览有效（不重叠），则开始一个编辑器事务（`GEditor->BeginTransaction`），标记 `URoomData` 为已修改（`Data->Modify()`），将 `DoorPreview` 添加到 `Data->Doors` 数组中，然后结束事务（`GEditor->EndTransaction`）。
  - **右键点击**: 如果门预览在 `Data->Doors` 数组中存在，则开始事务，标记修改，从数组中移除该门，然后结束事务。

#### MouseMove

- `MouseMove(...)`: 处理鼠标移动。
  - 将 `ShowDoorPreview` 设为 `false`。
  - 调用 `RoomTraceFromMouse` 获取鼠标射线与房间碰撞体的交点。
  - 如果有交点，调用 `GetRoomCellFromHit` 计算出单元格和方向。
  - 更新 `DoorPreview` 的 `Position`, `Direction` 和 `Type`（从 `EdMode->Settings` 获取）。
  - 遍历 `URoomData` 中已有的门，如果鼠标位置在某个现有门的内部，则将 `DoorPreview` 吸附到该门的数据上。
  - 将 `ShowDoorPreview` 设为 `true`。

#### GetCursor

- `GetCursor(...)`: 根据 `ShowDoorPreview` 和 `IsDoorValid` 的结果设置鼠标光标。有效位置显示手型光标，无效位置显示禁止光标。

#### OnLevelChanged / OnDataChanged

- `OnLevelChanged(...)`: 调用 `UpdateRoomBox()`。
- `OnDataChanged(...)`: 调用 `UpdateRoomBox()`。

#### UpdateRoomBox / DestroyRoomBox

- `UpdateRoomBox()`: 管理 `RoomBox` 组件的生命周期。
  - 检查当前关卡是否与缓存的关卡不同，如果不同则销毁旧的 `RoomBox` 并为新关卡创建一个新的。
  - 将新 `RoomBox` 附加到关卡的根组件，并设置其碰撞属性（忽略所有通道）。
  - 根据 `URoomData` 的边界设置 `RoomBox` 的位置和大小。
- `DestroyRoomBox()`: 销毁 `RoomBox` 组件并重置指针。

#### RoomTraceFromMouse / RoomTrace

- `RoomTraceFromMouse(...)`: 计算鼠标在视口中的位置，并生成一条从摄像机出发的射线。然后调用 `RoomTrace`。
- `RoomTrace(...)`: 使用 `UBoxComponent::LineTraceComponent` 执行与 `RoomBox` 的线性碰撞检测。

#### GetRoomCellFromHit

- `GetRoomCellFromHit(...)`: 从 `FHitResult` 中提取信息。
  - 首先检查碰撞法线是否大致垂直（Z轴方向），如果是则认为无效（不能在地面/天花板上放门）。
  - 根据法线与世界坐标轴的点积确定门的朝向（北/南/东/西）。
  - 使用碰撞点坐标、门朝向和 `Dungeon::RoomUnit()` 计算出对应的房间单元格坐标 (`FIntVector`)。