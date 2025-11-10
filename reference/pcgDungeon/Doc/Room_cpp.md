# Room.cpp

该文件包含了 `URoom` 类的实现，该类代表地牢中的一个房间实例。

## 主要功能

- **房间初始化**：初始化房间数据、位置、方向和连接。
- **网络复制**：处理房间属性和自定义数据的网络复制。
- **房间实例化**：加载和卸载房间关卡。
- **连接管理**：管理房间之间的连接（门）。
- **可见性管理**：控制房间的可见性。
- **自定义数据**：创建和管理房间的自定义数据。
- **坐标变换**：提供世界坐标和房间坐标之间的转换。
- **序列化**：支持房间数据的保存和加载。

## 详细实现

### 网络复制

- `GetLifetimeReplicatedProps`：指定需要复制的属性，包括 `bIsLocked`、`CustomData`、`RoomData`、`Position`、`Direction`、`Connections`、`GeneratorOwner` 和 `Id`。
- `ReplicateSubobjects`：复制自定义数据的子对象。
- `RegisterReplicableSubobjects`：注册或注销自定义数据的可复制子对象。
- `OnRep_*`：处理属性复制后的回调函数。

### 房间初始化

- `Init`：初始化房间数据、生成器和房间ID，设置位置和方向，并创建所有自定义数据。

### 连接管理

- `IsConnected`：检查指定门是否已连接。
- `SetConnection`：设置指定门的连接。
- `GetConnectedRoom`：获取指定门连接的房间。
- `GetFirstEmptyConnection`：获取第一个空的连接。
- `GetAllEmptyConnections`：获取所有空的连接。

### 房间实例化

- `Instantiate`：加载房间关卡。
- `Destroy`：卸载房间关卡。
- `OnInstanceLoaded`：房间关卡加载完成后的回调函数。
- `LoadInstance`：加载关卡实例。
- `UnloadInstance`：卸载关卡实例。

### 可见性管理

- `ForceVisibility`：强制设置房间可见性。
- `SetVisible`：设置房间可见性。
- `UpdateVisibility`：更新房间可见性。
- `SetPlayerInside`：设置玩家是否在房间内。

### 自定义数据

- `CreateCustomData`：创建自定义数据。
- `CreateAllCustomData`：创建所有自定义数据。
- `GetCustomData`：获取自定义数据。
- `HasCustomData`：检查是否存在自定义数据。
- `GetDataPair`：获取自定义数据对。

### 坐标变换

- `WorldToRoom`：将世界坐标转换为房间坐标。
- `RoomToWorld`：将房间坐标转换为世界坐标。

### 序列化

- `SerializeObject`：序列化或反序列化房间数据。
- `FixupReferences`：修复序列化后的引用。
- `PreSaveDungeon`：保存地牢前的回调函数。
- `PostLoadDungeon`：加载地牢后的回调函数。
- `SerializeLevelActors`：序列化关卡中的Actor。
- `DispatchCallbackToSavedLevelActors`：向保存的关卡Actor分发回调。

### 工具函数

- `Lock`：锁定或解锁房间。
- `SetPosition`：设置房间位置。
- `SetDirection`：设置房间方向。
- `GetLevelScript`：获取关卡蓝图脚本。
- `IsInstanceLoaded`：检查房间实例是否已加载。
- `IsInstanceUnloaded`：检查房间实例是否已卸载。
- `IsInstanceInitialized`：检查房间实例是否已初始化。
- `CreateLevelComponents`：创建关卡组件。
- `GetDoorWorldOrientation`：获取门的世界方向。
- `GetDoorWorldPosition`：获取门的世界位置。
- `IsDoorIndexValid`：检查门索引是否有效。
- `GetDoorIndexAt`：获取指定位置和方向的门索引。
- `GetOtherDoorIndex`：获取连接到另一个房间的门索引。
- `GetDoorDef`：获取门定义。
- `GetDoorDefAt`：获取指定位置和方向的门定义。
- `SetRotationFromDoor`：根据门方向设置房间旋转。
- `SetPositionFromDoor`：根据门位置设置房间位置。
- `SetPositionAndRotationFromDoor`：根据门位置和方向设置房间位置和旋转。
- `IsOccupied`：检查单元格是否被房间占据。
- `GetBounds`：获取房间边界。
- `GetLocalBounds`：获取房间本地边界。
- `GetIntBounds`：获取房间整数边界。
- `GetVoxelBounds`：获取房间体素边界。
- `GetTransform`：获取房间变换。
- `GetRandomStream`：获取随机流。
- `GetDoor`：获取门Actor。
- `GetAllDoors`：获取所有门Actor。
- `IsDoorConnected`：检查门是否已连接。
- `AreAllDoorsConnected`：检查所有门是否已连接。
- `CountConnectedDoors`：计算已连接的门数量。
- `GetConnectedRoomAt`：获取指定门连接的房间。
- `GetAllConnectedRooms`：获取所有连接的房间。
- `GetConnectedRoomIndex`：获取连接到另一个房间的门索引。
- `GetDoorsWith`：获取与另一个房间连接的门。
- `GetBoundsCenter`：获取房间边界中心。
- `GetBoundsExtent`：获取房间边界范围。
- `Overlap`：检查房间是否重叠。
- `GetRoomAt`：获取指定位置的房间。

这个类是地牢生成系统的核心组件之一，负责管理单个房间的实例化、连接、可见性和数据。