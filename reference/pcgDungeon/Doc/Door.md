# Door 类

## 概述

`ADoor` 类是地牢中所有门 Actor 的基类。即使你想创建一个墙而不是门（例如，当门没有连接到另一个房间时），也应该使用这个类。

## 成员变量

- `bLocked` (bool): 门是否被锁定。
- `bIsOpen` (bool): 门是否打开。
- `bShouldBeLocked` (bool): 门是否应该被锁定（用于保存/加载）。
- `bShouldBeOpen` (bool): 门是否应该打开（用于保存/加载）。
- `RoomA` (URoom*): 连接到此门的第一个房间。
- `RoomB` (URoom*): 连接到此门的第二个房间。
- `bAlwaysVisible` (bool): 门是否始终可见。
- `bAlwaysUnlocked` (bool): 门是否始终未锁定。
- `Type` (UDoorType*): 门的类型。
- `DefaultSceneComponent` (USceneComponent*): 默认场景组件。

## 成员函数

### 构造函数

- `ADoor()`: 构造函数。

### 蓝图可调用函数

- `SetConnectingRooms(URoom* RoomA, URoom* RoomB)`: 设置连接到此门的两个房间。
- `IsLocked()`: 返回门是否被锁定。
- `IsOpen()`: 返回门是否打开。
- `Open(bool open)`: 打开或关闭门（仅在权威端调用）。
- `Lock(bool lock)`: 锁定或解锁门（仅在权威端调用）。
- `GetDoorType()`: 返回门的类型。
- `ShouldBeOpened()`: 返回门是否应该打开。
- `ShouldBeLocked()`: 返回门是否应该被锁定。

### 生命周期函数

- `Tick(float DeltaTime)`: 每帧调用。
- `ShouldTickIfViewportsOnly()`: 如果仅在视口中运行，是否应该调用 Tick。

### 事件处理函数

- `OnDoorLock()`: 当门被锁定时调用（C++）。
- `OnDoorLock_BP()`: 当门被锁定时调用（蓝图）。
- `OnDoorUnlock()`: 当门被解锁时调用（C++）。
- `OnDoorUnlock_BP()`: 当门被解锁时调用（蓝图）。
- `OnDoorOpen()`: 当门打开时调用（C++）。
- `OnDoorOpen_BP()`: 当门打开时调用（蓝图）。
- `OnDoorClose()`: 当门关闭时调用（C++）。
- `OnDoorClose_BP()`: 当门关闭时调用（蓝图）。