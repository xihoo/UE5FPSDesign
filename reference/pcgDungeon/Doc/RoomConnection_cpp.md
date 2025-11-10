# RoomConnection.cpp

该文件包含了 `URoomConnection` 类的实现，该类用于表示两个房间之间的连接。

## 主要功能

- **连接管理**：存储和管理两个房间之间的连接信息。
- **网络复制**：处理连接属性的网络复制。
- **门实例化**：创建和管理连接处的门Actor。
- **序列化**：支持连接数据的保存和加载。

## 详细实现

### 网络复制

- `GetLifetimeReplicatedProps`：指定需要复制的属性，包括 `ID`、`RoomA`、`RoomADoorId`、`RoomB`、`RoomBDoorId` 和 `DoorInstance`。
- `OnRep_*`：处理属性复制后的回调函数。

### 序列化

- `SerializeObject`：序列化或反序列化连接数据，包括房间ID和门的属性。
- `FixupReferences`：修复序列化后的引用，将房间ID转换为实际的房间对象。
- `PreSaveDungeon`：保存地牢前的回调函数，调用门的 `PreSaveDungeon`。
- `PostLoadDungeon`：加载地牢后的回调函数，调用门的 `PostLoadDungeon` 并重置保存数据。

### 连接信息

- `GetID`：获取连接的唯一ID。
- `GetRoomA`、`GetRoomB`：获取连接的两个房间。
- `GetRoomADoorId`、`GetRoomBDoorId`：获取连接的两个门的索引。
- `GetOtherRoom`：获取相对于指定房间的另一个房间。
- `GetOtherDoorId`：获取相对于指定房间的另一个门的索引。

### 门管理

- `IsDoorInstanced`：检查门是否已实例化。
- `GetDoorInstance`：获取门的实例。
- `GetDoorLocation`：获取门的位置。
- `GetDoorRotation`：获取门的旋转。
- `SetDoorClass`：设置门的类和是否翻转。
- `InstantiateDoor`：实例化门Actor。

### 静态工具函数

- `GetOtherRoom`：静态函数，获取连接中相对于指定房间的另一个房间。
- `GetOtherDoorId`：静态函数，获取连接中相对于指定房间的另一个门的索引。
- `GetDoorInstance`：静态函数，获取连接的门实例。
- `GetDoorType`：静态函数，获取连接处门的类型。
- `CreateConnection`：静态函数，创建一个新的连接对象并设置其属性。

这个类是地牢生成系统中管理房间连接和门的核心组件。