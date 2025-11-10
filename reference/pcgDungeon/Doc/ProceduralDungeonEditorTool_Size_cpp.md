# ProceduralDungeonEditorTool_Size.cpp

## 概述

`ProceduralDungeonEditorTool_Size.cpp` 是 `FProceduralDungeonEditorTool_Size` 类及其辅助结构 `FRoomPoint` 的实现文件。它包含了调整地牢房间大小的交互逻辑。

## 依赖项

- `ProceduralDungeonEditorTool_Size.h`: 类和结构体的声明。
- `EditorMode/ProceduralDungeonEdMode.h`: 访问编辑器模式。
- `EditorModeManager.h`: 管理编辑器模式和变换控件。
- `Engine/Selection.h`: 管理编辑器中的 Actor 选择。
- `ProceduralDungeonEdLog.h`: 编辑器日志宏。
- `ProceduralDungeonUtils.h`: 工具函数，如 `Dungeon::SnapPoint`, `Dungeon::ToWorldLocation`, `Dungeon::ToRoomLocation`。
- `Room.h`, `RoomLevel.h`, `RoomData.h`: 相关的地牢数据结构和类。

## 实现细节

### HRoomPointProxy

- `IMPLEMENT_HIT_PROXY(HRoomPointProxy, HHitProxy);`: 宏，用于实现命中代理。

### FRoomPoint 方法实现

#### AddLinkedPoint

- `AddLinkedPoint(...)`: 将当前点和传入的 `Point` 互相添加到各自的 `LinkedPoints` 列表中，建立双向链接。

#### SetLocation

- `SetLocation(...)`: 设置点的新位置，调用 `SetDirty()` 标记自身和链接点为脏，然后调用 `UpdateWithPropagation()` 启动更新传播。

#### SetDirty

- `SetDirty()`: 如果点已经是脏的，则直接返回。否则，将其标记为所有轴都脏，并递归地调用所有链接点的 `SetDirty()`。

#### UpdateWithPropagation

- `UpdateWithPropagation()`: 使用广度优先搜索（BFS）遍历所有标记为脏的点。它使用一个队列 `TQueue<FRoomPoint*>` 来管理待处理的节点，确保更新按顺序传播。

#### UpdateLinkedPoints

- `UpdateLinkedPoints(...)`: 遍历所有链接点。如果链接点是脏的，则调用 `UpdateFrom` 方法从当前点更新它，然后将该链接点加入待处理队列 `PendingNodes`。

#### UpdateFrom

- `UpdateFrom(...)`: 根据传入的 `From` 点和指定的 `Axis`，更新当前点的坐标。它会检查 `bDirty` 标志和 `Axis` 来确定哪些轴需要更新，并在更新后清除相应轴的 `bDirty` 标志。

### FProceduralDungeonEditorTool_Size 方法实现

#### EnterTool

- `EnterTool()`: 初始化工具。
  - 清空并创建 8 个 `FRoomPoint`。
  - 根据房间立方体的几何关系，使用 `AddLinkedPoint` 建立点之间的链接。例如，点 0 (最小角) 与点 2 (共享 XY 平面), 点 3 (共享 XZ 平面), 点 4 (共享 YZ 平面) 链接。
  - 调用 `OnDataChanged()` 用当前 `URoomData` 的数据初始化点的位置。
  - 调用 `ResetSelectedPoint()` 确保初始状态没有点被选中。

#### ExitTool

- `ExitTool()`: 记录退出工具的日志。

#### Render

- `Render(...)`: 渲染逻辑。
  - 获取当前关卡和 `URoomData`。
  - 遍历 8 个 `Points`。
  - 为每个点设置一个 `HRoomPointProxy` 命中代理。
  - 使用 `PDI->DrawPoint` 绘制点。选中的点使用不同颜色。
  - 绘制完点后，清除命中代理 (`PDI->SetHitProxy(nullptr)`)。

#### HandleClick

- `HandleClick(...)`: 处理点击事件。
  - 调用 `ResetSelectedPoint()` 取消当前选择。
  - 检查 `HitProxy` 是否存在且是 `HRoomPointProxy` 类型。
  - 获取 `HRoomPointProxy` 并检查其索引有效性。
  - 调用 `SetSelectedPoint()` 选中被点击的点。
  - 调用 `GEditor->GetSelectedActors()->DeselectAll()` 取消选择所有 Actor。
  - 获取 `FEditorModeTools` 并调用 `SetWidgetMode(WM_Translate)` 强制将编辑器的变换控件设置为移动模式。

#### InputKey

- `InputKey(...)`: 处理按键事件。
  - 如果是左键释放 (`EKeys::LeftMouseButton`, `IE_Released`)，则调用 `ResetDragPoint()`。

#### InputDelta

- `InputDelta(...)`: 处理拖拽变换。
  - 检查当前变换轴是否为 `None` 或没有有效选择，如果是则返回 `false`。
  - 如果 `InDrag`（拖拽位移）不为零：
    - 将 `InDrag` 累加到 `DragPoint`。
    - 获取选中点的旧位置。
    - 调用 `Points[SelectedPoint].SetLocation(Dungeon::SnapPoint(DragPoint))` 设置新位置（包含吸附）。
    - 如果位置确实发生了变化，则调用 `UpdateDataAsset()` 更新 `URoomData`。

#### UsesTransformWidget

- `UsesTransformWidget() const`: 如果有选中的点或有选中的 Actor，则返回 `true`。

#### UsesTransformWidget (WidgetMode)

- `UsesTransformWidget(WidgetMode CheckMode) const`: 如果有点被选中，则只在 `CheckMode` 为 `WM_Translate` 时返回 `true`。否则，调用基类 `FEdMode` 的实现。

#### GetWidgetLocation

- `GetWidgetLocation() const`: 如果有点被选中，则返回 `DragPoint`。否则，调用基类 `FEdMode` 的实现。

#### PostUndo / PostRedo

- `PostUndo(...)`: 撤销后调用 `OnDataChanged()` 刷新点位置。
- `PostRedo(...)`: 重做后调用 `OnDataChanged()` 刷新点位置。

#### OnDataChanged

- `OnDataChanged(...)`: 数据变更时的处理。
  - 获取当前关卡和 `URoomData`。
  - 使用 `Dungeon::ToWorldLocation` 将 `URoomData` 的 `FirstPoint` 和 `SecondPoint` 转换为世界坐标，并设置给 `Points[0]` 和 `Points[1]`。
  - 调用 `ResetDragPoint()` 更新拖拽点位置。

#### HasValidSelection / ResetDragPoint / UpdateDataAsset / SetSelectedPoint / ResetSelectedPoint

- `HasValidSelection()`: 检查 `SelectedPoint` 索引是否有效。
- `ResetDragPoint()`: 将 `DragPoint` 设置为当前选中点的位置。
- `UpdateDataAsset()`: 更新 `URoomData`。
  - 获取关卡和 `URoomData`。
  - 调用 `Data->Modify()` 标记数据为已修改。
  - 使用 `Dungeon::ToRoomLocation` 将 `Points[0]` 和 `Points[1]` 的世界坐标转换回房间坐标，并更新 `Data->FirstPoint` 和 `Data->SecondPoint`。
- `SetSelectedPoint(int32 Index)`: 设置 `SelectedPoint`，记录日志，并调用 `ResetDragPoint()`。
- `ResetSelectedPoint()`: 调用 `SetSelectedPoint(-1)` 取消选择。