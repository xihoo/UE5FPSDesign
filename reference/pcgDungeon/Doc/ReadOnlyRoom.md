# ReadOnlyRoom 类

## 概述

`IReadOnlyRoom` 是一个接口，用于在生成过程中访问房间实例的一些数据。它提供了一种只读的方式来获取房间的信息，而无需修改房间实例本身。

## 接口

### UReadOnlyRoom

这是一个 Unreal Engine 接口类，标记为 `MinimalAPI`、`BlueprintType` 和 `NotBlueprintable`。它不能在蓝图中实现。

### IReadOnlyRoom

这是 `IReadOnlyRoom` 接口的实际实现。

#### 成员函数

- `GetRoomData()`: 返回此房间实例的房间数据资产。
- `GetRoomID()`: 返回房间的唯一 ID（每个地牢）。第一个房间的 ID 为 0，然后按照放置房间的顺序递增。
- `GetPosition()`: 返回房间的位置（以房间单元为单位）。
- `GetDirection()`: 返回房间的方向。
- `AreAllDoorsConnected()`: 如果此房间的所有门都连接到其他房间，则返回 true。
- `CountConnectedDoors()`: 返回此房间中连接到另一个房间的门的数量。
- `GetBoundsCenter()`: 返回房间的世界中心位置。
- `GetBoundsExtent()`: 返回房间的世界范围（半尺寸）。