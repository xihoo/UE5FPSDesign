# DungeonGraph 类

## 概述

`UDungeonGraph` 类持有生成的地牢。你可以使用许多函数来访问房间。它继承自 `UReplicableObject`，并实现了 `IRoomContainer`、`IGeneratorProvider`、`IDungeonCustomSerialization` 和 `IDungeonSaveInterface` 接口。

## 结构体

### FRoomCandidate

- `Data` (URoomData*): 房间数据。
- `DoorIndex` (int32): 门索引。
- `Score` (int32): 分数。

## 成员变量

- `Rooms` (TArray<URoom*>): 房间数组。
- `RoomConnections` (TArray<URoomConnection*>): 房间连接数组。
- `ReplicatedRooms` (TArray<URoom*>): 与服务器同步的房间数组。
- `bIsDirty` (bool): 是否脏。
- `Generator` (TWeakObjectPtr<ADungeonGeneratorBase>): 生成器。
- `Bounds` (FVoxelBounds): 地牢的计算边界。

## 成员函数

### IRoomContainer 接口

- `GetRoomByIndex(int64 Index)`: 返回具有提供索引的房间实例。如果不存在具有提供索引的房间，则返回 null。
- `GetConnectionByIndex(int32 Index)`: 返回具有提供索引的房间连接。

### IDungeonCustomSerialization 接口

- `SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading)`: 序列化对象。

### IDungeonSaveInterface 接口

- `PostLoadDungeon_Implementation()`: 加载地牢后调用。

### IGeneratorProvider 接口

- `GetGenerator()`: 返回生成器。

### 房间管理

- `AddRoom(URoom* Room)`: 添加房间。
- `InitRooms()`: 初始化房间。
- `Clear()`: 清除。
- `TryConnectDoor(URoom* Room, int32 DoorIndex)`: 尝试连接门。
- `TryConnectToExistingDoors(URoom* Room)`: 尝试连接到现有门。
- `HasRooms()`: 是否有房间。
- `IsDirty()`: 是否脏。
- `GetAllRooms()`: 返回所有房间。
- `GetAllConnections()`: 返回所有房间连接。
- `GetAllRoomsFromData(const URoomData* Data, TArray<URoom*>& Rooms)`: 返回提供的房间数据的所有房间实例。
- `GetAllRoomsFromDataList(const TArray<URoomData*>& Data, TArray<URoom*>& Rooms)`: 返回提供的房间数据列表的任何房间数据的所有房间实例。
- `GetFirstRoomFromData(const URoomData* Data)`: 返回提供的房间数据的第一个找到的房间实例。
- `GetAllRoomsWithCustomData(const TSubclassOf<URoomCustomData>& CustomData, TArray<URoom*>& Rooms)`: 返回具有提供的自定义数据的所有房间实例。
- `GetAllRoomsWithAllCustomData(const TArray<TSubclassOf<URoomCustomData>>& CustomData, TArray<URoom*>& Rooms)`: 返回具有所有提供的自定义数据的所有房间实例。
- `GetAllRoomsWithAnyCustomData(const TArray<TSubclassOf<URoomCustomData>>& CustomData, TArray<URoom*>& Rooms)`: 返回具有至少一个提供的自定义数据的所有房间实例。
- `GetRandomRoom(const TArray<URoom*>& RoomList)`: 从房间数组中返回一个随机房间。
- `Count()`: 返回房间总数。
- `HasAlreadyRoomData(const URoomData* RoomData)`: 如果地牢中已存在特定的 RoomData，则返回 true。
- `HasAlreadyOneRoomDataFrom(const TArray<URoomData*>& RoomDataList)`: 如果地牢中已存在提供的列表中的至少一个 RoomData，则返回 true。
- `CountRoomData(const URoomData* RoomData)`: 返回地牢中特定 RoomData 的数量。
- `CountTotalRoomData(const TArray<URoomData*>& RoomDataList)`: 返回地牢中提供的列表中的 RoomData 的总数。
- `HasAlreadyRoomType(const TSubclassOf<URoomData>& RoomType)`: 如果地牢中已存在特定的 RoomData 类型，则返回 true。
- `HasAlreadyOneRoomTypeFrom(const TArray<TSubclassOf<URoomData>>& RoomTypeList)`: 如果地牢中已存在提供的列表中的至少一个 RoomData 类型，则返回 true。
- `CountRoomType(const TSubclassOf<URoomData>& RoomType)`: 返回地牢中特定 RoomData 类型的数量。
- `CountTotalRoomType(const TArray<TSubclassOf<URoomData>>& RoomTypeList)`: 返回地牢中提供的列表中的 RoomData 类型的总数。
- `HasValidPath(const URoom* From, const URoom* To, bool IgnoreLockedRooms = false)`: 返回两个房间之间是否存在有效路径（没有锁定的房间阻挡）。
- `NumberOfRoomBetween(const URoom* A, const URoom* B, bool IgnoreLockedRooms = false)`: 返回 A 和 B 之间的最小连接房间数。
- `NumberOfRoomBetween_ReadOnly(TScriptInterface<IReadOnlyRoom> A, TScriptInterface<IReadOnlyRoom> B)`: 返回 A 和 B 之间的最小连接房间数（使用 ReadOnlyRoom）。
- `GetPathBetween(const URoom* A, const URoom* B, TArray<URoom*>& ResultPath, bool IgnoreLockedRooms = false)`: 返回 A 和 B 之间的路径。
- `GetRoomAt(FIntVector RoomCell)`: 返回位于提供的房间单元格（以房间单元表示，而不是虚幻单元！！！）的房间实例。如果在提供的单元格中没有房间，则返回 null。
- `GetDungeonBoundsCenter()`: 返回地牢边界框的中心。
- `GetDungeonBoundsExtent()`: 返回地牢边界框的范围（半尺寸）。
- `FilterAndSortRooms(const TArray<URoomData*>& RoomList, const FDoorDef& FromDoor, TArray<FRoomCandidate>& SortedRooms, const FScoreCallback& CustomScore)`: 过滤和排序房间。
- `GetDungeonBounds(const FTransform& Transform = FTransform::Identity)`: 返回计算的地牢边界。
- `GetIntBounds()`: 返回整数边界。
- `GetVoxelBounds()`: 返回体素边界。
- `TraverseRooms(const TSet<URoom*>& InRooms, TSet<URoom*>* OutRooms, uint32 Distance, TFunction<void(URoom*)> Func)`: 返回距离中每个 InRooms 的所有房间，并可选择对每个房间应用 Func。
- `FindPath(const URoom* From, const URoom* To, TArray<const URoom*>* OutPath = nullptr, bool IgnoreLocked = false)`: 查找路径。

### 受保护函数

- `CountRoomByPredicate(TFunction<bool(const URoom*)> Predicate)`: 按谓词计算房间。
- `GetRoomsByPredicate(TArray<URoom*>& OutRooms, TFunction<bool(const URoom*)> Predicate)`: 按谓词获取房间。
- `FindFirstRoomByPredicate(TFunction<bool(const URoom*)> Predicate)`: 按谓词查找第一个房间。
- `ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)`: 复制子对象。
- `RegisterReplicableSubobjects(bool bRegister)`: 注册可复制子对象。
- `SynchronizeRooms()`: 同步房间。
- `RetrieveRoomsFromLoadedData()`: 从加载的数据中检索房间。
- `Connect(URoom* RoomA, int32 DoorA, URoom* RoomB, int32 DoorB)`: 创建并存储两个房间之间的新连接。
- `AreRoomsLoaded(int32& NbRoomLoaded)`: 房间是否已加载。
- `AreRoomsUnloaded(int32& NbRoomUnloaded)`: 房间是否已卸载。
- `AreRoomsInitialized(int32& NbRoomInitialized)`: 房间是否已初始化。
- `AreRoomsReady()`: 房间是否已就绪。
- `SpawnAllDoors()`: 生成所有门。
- `LoadAllRooms()`: 加载所有房间。
- `UnloadAllRooms()`: 卸载所有房间。
- `MarkDirty()`: 标记为脏。
- `UpdateBounds(const URoom* Room)`: 扩展边界以包含提供的房间。
- `RebuildBounds()`: 使用整个房间列表重新创建边界。

### 私有函数

- `OnRep_Rooms()`: 房间复制时调用。

好的，我们来深入了解一下 URoomConnection 如何表示房间间的连接，以及 UDungeonGraph 如何利用这些连接来管理整个地牢结构。

  1. `URoomConnection`: 连接的表示

  URoomConnection 类是表示两个 `URoom` 实例之间连接的核心数据结构。它本身是一个 UReplicableObject，意味着它可以像 UObject
  一样被网络复制，这对于多人游戏中同步地牢结构至关重要。

   * 核心成员变量:
       * RoomA (TWeakObjectPtr<URoom>) 和 RoomB (TWeakObjectPtr<URoom>): 这两个弱指针分别指向连接的两个房间实例。使用弱指针可以避免循环引用导致的内存泄漏问题。
       * RoomADoorId (int32) 和 RoomBDoorId (int32): 这两个整数索引指明了连接发生在 RoomA 和 RoomB 各自的 URoomData::Doors 数组中的哪个门定义 (FDoorDef)
         上。例如，RoomADoorId 为 1，意味着这个连接是通过 RoomA 的 RoomData->Doors[1] 这个门定义建立的。
       * DoorClass (TSubclassOf<ADoor>): 指定在物理世界中，这个连接处应该生成哪种具体的门 Actor 类（例如，是普通门 ADoor 还是触发门 ATriggerDoor）。
       * bFlipped (bool): 一个布尔值，用于指示门的朝向是否需要翻转。这在两个房间的门定义方向不完全相对时可能需要调整。
       * DoorInstance (TWeakObjectPtr<ADoor>): 一个指向实际生成的门 Actor 实例的弱指针。这个指针在地牢生成后期，当门被实例化后才会被填充。

   * 作用:
       * URoomConnection 对象精确地记录了“哪个房间的哪个门”连接到了“另一个房间的哪个门”。
       * 它存储了生成门 Actor 所需的信息（类型、是否翻转）。
       * 它持有对生成的门 Actor 实例的引用，方便后续访问和管理。
       * 通过网络复制，确保客户端和服务端对房间连接关系有一致的认知。

  2. `UDungeonGraph`: 地牢结构的管理者

  UDungeonGraph 是一个 UReplicableObject，它扮演着整个已生成地牢的中央数据库和管理器的角色。它持有所有房间和所有连接，并提供各种方法来查询和操作这个网络结构。

   * 核心成员变量:
       * Rooms (TArray<URoom*>): 一个数组，存储了地牢中所有已生成的 URoom 实例的指针。房间的索引（在数组中的位置）通常与其 URoom::Id 相关联。
       * RoomConnections (TArray<URoomConnection*>): 一个数组，存储了地牢中所有已创建的 URoomConnection 实例的指针。

   * 管理职责:
       * 存储: 持有所有房间和连接的引用。
       * 创建连接: 当 ADungeonGenerator 决定在两个房间之间建立连接时，它会调用 UDungeonGraph 的方法（如 Connect 或在 AddRoomToDungeon 内部）来创建一个新的 URoomConnection
         对象，并将其添加到 RoomConnections 数组中。同时，它会更新两个 URoom 实例内部的 Connections 数组，让它们各自知道在哪个门索引上连接了哪个 URoomConnection。
       * 查询:
           * 按索引查找: GetRoomByIndex, GetConnectionByIndex 提供了基于数组索引的快速访问。
           * 按属性查找: 提供了大量方法来根据各种条件查找房间，例如 GetAllRoomsFromData (查找使用特定 URoomData 的所有房间), GetAllRoomsWithCustomData
             (查找包含特定自定义数据的房间), GetRoomAt (查找特定逻辑坐标上的房间)。
           * 路径查找: FindPath, HasValidPath, GetPathBetween 等方法利用图论算法（通常是广度优先搜索 BFS）在房间网络中查找路径。这对于 AI
             导航、寻路或游戏逻辑（如检查两个房间是否连通）非常关键。
           * 连接查询: 可以查询特定房间的连接状态，如 AreAllDoorsConnected, CountConnectedDoors, GetConnectedRoomAt。
       * 边界计算: GetDungeonBounds, GetVoxelBounds 等方法会遍历所有房间，计算出整个地牢在逻辑空间或世界空间中的总体边界。
       * 网络复制: 通过 ReplicateSubobjects 等方法，确保 Rooms 和 RoomConnections 数组能被正确地复制到客户端。
       * 序列化/保存加载: 实现了 IDungeonCustomSerialization 和
         IDungeonSaveInterface，负责在保存游戏时序列化所有房间和连接的信息，并在加载时正确地反序列化和重建它们之间的引用关系。
       * 初始化与清理: InitRooms 在所有房间添加完毕后被调用，用于最终化连接（如为未连接的门选择默认门类）。Clear 用于重置和清理整个图。

  总结:

   * URoomConnection 是连接本身的“化身”，它存储了连接的两个端点（房间和门索引）以及相关的门信息。
   * UDungeonGraph 是连接关系的“管理者”，它维护着一个包含所有房间和所有连接的列表，并提供了丰富的 API
     来查询、遍历和操作这个由房间和连接构成的复杂网络结构。它是理解整个地牢拓扑结构的核心。