# ProceduralDungeonEditorTool_Size.h

## 概述

`ProceduralDungeonEditorTool_Size.h` 定义了 `FProceduralDungeonEditorTool_Size` 类以及相关的辅助结构 `HRoomPointProxy` 和 `FRoomPoint`。这个类是“地牢房间编辑模式”中的一个具体工具，用于调整房间的大小。它通过操作房间的两个对角点（`FirstPoint` 和 `SecondPoint`）来改变房间的边界。它继承自抽象基类 `FProceduralDungeonEditorTool`。

## 结构体定义

### HRoomPointProxy

这是一个 `HHitProxy` 的子类，用于在视口中标识可交互的房间点（即代表 `FirstPoint` 和 `SecondPoint` 的控制点）。当用户点击这些点时，编辑器可以通过这个代理知道是哪个点被点击了。

- `DECLARE_HIT_PROXY()`: 宏，用于声明命中代理。
- `HRoomPointProxy(int32 InIndex)`: 构造函数，接受一个索引 `InIndex`，用于标识是哪个点（0 或 1）。
- `int32 Index`: 存储点的索引。

### FRoomPoint

这是一个辅助结构，用于管理视口中表示房间角落的 3D 点。它不仅存储点的位置，还实现了点之间的链接和联动更新逻辑，以确保在拖拽一个点时，其他相关联的点也会相应移动，从而保持房间形状的完整性（例如，拖拽一个角点时，与之相连的边会同步移动）。

#### 方法

- `void AddLinkedPoint(FRoomPoint& Point, EAxisList::Type Axis)`: 将当前点与另一个点 `Point` 连接起来，并指定它们在哪个轴上联动（X, Y, Z 或它们的组合）。
- `FVector GetLocation() const`: 获取点的当前位置。
- `void SetLocation(FVector InLocation)`: 设置点的新位置，并触发联动更新。
- `FVector* operator->()`: 重载箭头操作符，方便直接访问内部的 `FVector Location`。
- `void SetDirty()`: 标记该点及其链接点为“脏”（需要更新）。
- `void UpdateWithPropagation()`: 启动一个传播更新过程，确保所有相关的点都根据联动规则更新。
- `void UpdateLinkedPoints(TQueue<FRoomPoint*>& PendingNodes)`: 更新与当前点链接的其他点。
- `void UpdateFrom(FRoomPoint& From, EAxisList::Type Axis)`: 根据另一个点 `From` 和指定的轴 `Axis` 来更新当前点的位置。

#### 私有成员

- `EAxisList::Type bDirty`: 标记哪些轴上的数据是“脏”的。
- `FVector Location`: 点的 3D 位置。
- `TArray<FLink> LinkedPoints`: 与当前点链接的其他点的列表。

## 类定义

### FProceduralDungeonEditorTool_Size

此类实现了房间大小调整工具的逻辑和行为。

#### 构造函数

- `FProceduralDungeonEditorTool_Size(FProceduralDungeonEdMode* InEdMode)`: 构造函数，调用基类构造函数。

#### 重写的基类方法

- `virtual const TCHAR* GetToolName() override`: 返回工具的内部名称 "Tool_Size"。
- `virtual FText GetDisplayName() override`: 返回工具的显示名称 "Size"。
- `virtual FText GetDisplayMessage() override`: 返回工具激活时的提示信息（此实现为空）。
- `virtual void EnterTool() override`: 当工具激活时调用，初始化 8 个 `FRoomPoint`（代表房间的 8 个角落），并根据它们的空间关系建立链接，然后调用 `OnDataChanged` 和 `ResetSelectedPoint`。
- `virtual void ExitTool() override`: 当工具停用时调用，记录日志。
- `virtual void Render(...) override`: 在视口中渲染 8 个代表房间角落的点。被选中的点会以不同颜色显示，并设置 `HRoomPointProxy` 以便点击检测。
- `virtual bool HandleClick(...) override`: 处理鼠标点击事件。如果点击的是一个 `HRoomPointProxy`，则选中对应的点，取消选择所有 Actor，并强制将编辑器的变换控件模式设置为“移动”（Translate）。
- `virtual bool InputKey(...) override`: 处理键盘/鼠标按键事件。当左键释放时，调用 `ResetDragPoint`。
- `virtual bool InputDelta(...) override`: 处理视口中的变换操作（拖拽）。如果有点被选中且有拖拽位移，则更新该点的位置（经过吸附），并调用 `UpdateDataAsset`。
- `virtual bool UsesTransformWidget() const override`: 指示工具是否使用标准的变换控件。如果有选中的点或有选中的 Actor，则使用。
- `virtual bool UsesTransformWidget(WidgetMode CheckMode) const override`: 检查特定的变换控件模式是否被使用。如果有点被选中，则只使用“移动”模式。
- `virtual FVector GetWidgetLocation() const override`: 返回变换控件的位置。如果有选中的点，则返回该点的拖拽位置 `DragPoint`。
- `virtual void PostUndo(...) override`: 撤销操作后调用，刷新点的位置。
- `virtual void PostRedo(...) override`: 重做操作后调用，刷新点的位置。
- `virtual void OnDataChanged(...) override`: 当关联的 `URoomData` 改变时调用，更新 `Points[0]` 和 `Points[1]` 的位置，并重置拖拽点。

#### 公共方法

- `bool HasValidSelection() const`: 检查当前是否有有效的点被选中。
- `void ResetDragPoint()`: 将 `DragPoint` 重置为当前选中点的位置。
- `void UpdateDataAsset() const`: 将 `Points[0]` 和 `Points[1]` 的世界坐标转换回房间坐标，并更新关联的 `URoomData` 的 `FirstPoint` 和 `SecondPoint`。
- `void SetSelectedPoint(int32 Index)`: 设置当前选中的点索引，并重置拖拽点。
- `void ResetSelectedPoint()`: 取消选择任何点。

#### 私有成员

- `int32 SelectedPoint`: 当前选中的点的索引（-1 表示未选中）。
- `TArray<FRoomPoint> Points`: 存储代表房间 8 个角落的点。
- `FVector DragPoint`: 当前拖拽操作的目标位置。