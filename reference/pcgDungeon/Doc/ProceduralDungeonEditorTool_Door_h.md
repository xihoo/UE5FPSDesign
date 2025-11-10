# ProceduralDungeonEditorTool_Door.h

## 概述

`ProceduralDungeonEditorTool_Door.h` 定义了 `FProceduralDungeonEditorTool_Door` 类。这个类是“地牢房间编辑模式”中的一个具体工具，用于在房间的墙壁上添加或移除门（`FDoorDef`）。它继承自抽象基类 `FProceduralDungeonEditorTool`。

## 类定义

### FProceduralDungeonEditorTool_Door

此类实现了门编辑工具的逻辑和行为。

#### 构造函数

- `FProceduralDungeonEditorTool_Door(FProceduralDungeonEdMode* InEdMode)`: 构造函数，调用基类构造函数。

#### 重写的基类方法

- `virtual const TCHAR* GetToolName() override`: 返回工具的内部名称 "Tool_Door"。
- `virtual FText GetDisplayName() override`: 返回工具的显示名称 "Door"。
- `virtual FText GetDisplayMessage() override`: 返回工具激活时的提示信息（此实现为空）。
- `virtual void EnterTool() override`: 当工具激活时调用，记录日志并调用 `UpdateRoomBox`。
- `virtual void ExitTool() override`: 当工具停用时调用，记录日志，销毁 `RoomBox` 并重置缓存的关卡指针。
- `virtual void Render(...) override`: 在视口中渲染辅助线，绘制房间网格线。
- `virtual void Tick(...) override`: 每帧调用，如果 `ShowDoorPreview` 为真，则绘制门的预览。
- `virtual bool HandleClick(...) override`: 处理鼠标点击事件。左键点击添加门，右键点击移除门。使用编辑器事务（Transaction）来支持撤销/重做。
- `virtual bool MouseMove(...) override`: 处理鼠标移动，计算鼠标悬停位置对应的房间单元格和门方向，更新 `DoorPreview` 数据，并根据现有门进行吸附。
- `virtual bool GetCursor(...) const override`: 根据门预览位置的有效性（是否与现有门重叠）设置鼠标光标（手型或禁止符号）。
- `virtual void OnLevelChanged(...) override`: 当编辑的关卡改变时调用，更新 `RoomBox`。
- `virtual void OnDataChanged(...) override`: 当关联的 `URoomData` 改变时调用，更新 `RoomBox`。

#### 受保护方法

- `void UpdateRoomBox()`: 更新或创建一个用于检测鼠标点击的临时 `UBoxComponent`，其大小和位置与当前 `URoomData` 的边界一致。
- `void DestroyRoomBox()`: 销毁临时的 `UBoxComponent`。
- `bool RoomTraceFromMouse(...)`: 从鼠标位置发射一条射线，并与 `RoomBox` 进行碰撞检测。
- `bool RoomTrace(...)`: 执行与 `RoomBox` 的线性碰撞检测。
- `bool GetRoomCellFromHit(...)`: 根据碰撞结果（`FHitResult`）计算出对应的房间单元格坐标 (`FIntVector`) 和门方向 (`EDoorDirection`)。

#### 私有成员

- `TWeakObjectPtr<class ARoomLevel> CachedLevel`: 缓存当前编辑的 `ARoomLevel` 指针。
- `TWeakObjectPtr<class UBoxComponent> RoomBox`: 用于鼠标交互检测的临时碰撞体。
- `FDoorDef DoorPreview`: 当前鼠标悬停位置的门预览数据。
- `bool ShowDoorPreview`: 标记是否显示门预览。