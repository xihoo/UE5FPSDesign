# DungeonGraph.cpp

该文件包含了 `UDungeonGraph` 类的实现，该类用于管理地牢中所有房间及其连接关系。

## 主要功能

- **房间管理**：添加、初始化、加载和卸载房间。
- **连接管理**：创建和管理房间之间的连接（门）。
- **序列化**：支持地牢数据的保存和加载。
- **路径查找**：提供在房间之间查找路径的功能。
- **房间查询**：根据房间数据或自定义数据查找房间。
- **网络复制**：处理房间和连接的网络复制。

## 详细实现

### 网络复制

- `GetLifetimeReplicatedProps`：指定需要复制的属性（`ReplicatedRooms` 和 `RoomConnections`）。
- `ReplicateSubobjects`：复制房间和连接的子对象。
- `RegisterReplicableSubobjects`：注册或注销可复制的子对象。
- `OnRep_Rooms`：当复制的房间列表发生变化时调用，标记图表已更改。

### 房间管理

- `AddRoom`：将房间添加到图表中并更新边界。
- `InitRooms`：初始化所有房间，包括创建空连接和调用房间数据的初始化函数。
- `Clear`：清理所有房间和连接，调用房间数据的清理函数。
- `RetrieveRoomsFromLoadedData`：从加载的数据中恢复房间和连接。
- `SynchronizeRooms`：同步服务器和客户端的房间列表。
- `LoadAllRooms`：加载所有房间的关卡。
- `UnloadAllRooms`：卸载所有房间的关卡并销毁门实例。

### 连接管理

- `Connect`：创建两个房间之间的连接。
- `TryConnectDoor`：尝试连接指定房间的门到相邻房间的门。
- `TryConnectToExistingDoors`：尝试连接房间的所有门到现有房间。
- `SpawnAllDoors`：为所有连接实例化门Actor。

### 序列化

- `SerializeObject`：序列化或反序列化房间和连接数据。

### 路径查找

- `FindPath`：使用双向广度优先搜索（BFS）查找两个房间之间的路径。
- `HasValidPath`：检查两个房间之间是否存在有效路径。
- `NumberOfRoomBetween`：计算两个房间之间的房间数量。
- `GetPathBetween`：获取两个房间之间的路径。

### 房间查询

- `GetAllRoomsFromData`：获取所有使用指定房间数据的房间。
- `GetAllRoomsFromDataList`：获取所有使用指定房间数据列表中任意数据的房间。
- `GetFirstRoomFromData`：获取第一个使用指定房间数据的房间。
- `GetAllRoomsWithCustomData`：获取所有包含指定自定义数据的房间。
- `GetAllRoomsWithAllCustomData`：获取所有包含指定自定义数据列表中所有数据的房间。
- `GetAllRoomsWithAnyCustomData`：获取所有包含指定自定义数据列表中任意数据的房间。
- `GetRandomRoom`：从房间列表中随机获取一个房间。
- `HasAlreadyRoomData`：检查是否已存在使用指定房间数据的房间。
- `HasAlreadyOneRoomDataFrom`：检查是否已存在使用指定房间数据列表中任意数据的房间。
- `CountRoomData`：计算使用指定房间数据的房间数量。
- `CountTotalRoomData`：计算使用指定房间数据列表中任意数据的房间总数量。
- `HasAlreadyRoomType`：检查是否已存在指定类型的房间。
- `HasAlreadyOneRoomTypeFrom`：检查是否已存在指定类型列表中任意类型的房间。
- `CountRoomType`：计算指定类型房间的数量。
- `CountTotalRoomType`：计算指定类型列表中所有类型房间的总数量。

### 工具函数

- `GetConnectionByIndex`：根据索引获取连接。
- `GetRoomAt`：根据单元格位置获取房间。
- `GetRoomByIndex`：根据房间ID获取房间。
- `GetDungeonBoundsCenter`：获取地牢边界中心。
- `GetDungeonBoundsExtent`：获取地牢边界范围。
- `GetIntBounds`：获取整数边界。
- `FilterAndSortRooms`：根据门定义过滤和排序房间候选列表。
- `TraverseRooms`：遍历房间图。
- `UpdateBounds`：更新地牢边界。
- `RebuildBounds`：重新构建地牢边界。
- `MarkDirty`：标记图表已更改。
- `IsDirty`：检查图表是否已更改。
- `HasAuthority`：检查是否具有权威性。
- `GetWorld`：获取世界对象。