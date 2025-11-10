# Room 类

## 概述

`URoom` 类表示地牢中的房间实例。它持有每个房间实例的特定数据，例如位置、方向、玩家是否在房间内、房间自定义数据等。它继承自 `UReplicableObject`，并实现了 `IReadOnlyRoom`、`IDungeonCustomSerialization` 和 `IDungeonSaveInterface` 接口。

## 结构体

### FCustomDataPair

用于存储自定义数据的键值对。

- `DataClass` (UClass*): 数据类。
- `Data` (URoomCustomData*): 数据实例。

## 成员变量

- `Instance` (ULevelStreamingDynamic*): 房间的关卡流实例。
- `Position` (FIntVector): 房间的位置（以房间单元为单位）。
- `Direction` (EDoorDirection): 房间的方向。
- `RoomData` (TSoftObjectPtr<URoomData>): 房间数据资产的软引用。
- `CustomData` (TArray<FCustomDataPair>): 房间的自定义数据。
- `Connections` (TArray<TWeakObjectPtr<class URoomConnection>>): 房间的连接。
- `GeneratorOwner` (TWeakObjectPtr<ADungeonGeneratorBase>): 生成器所有者。
- `Id` (int64): 房间的唯一 ID。
- `bPlayerInside` (bool): 玩家是否在房间内。
- `bIsVisible` (bool): 房间是否可见。
- `bForceVisible` (bool): 是否强制房间可见。
- `bIsLocked` (bool): 房间是否被锁定。

## 成员函数

### IReadOnlyRoom 接口

- `GetRoomData()`: 返回此房间实例的房间数据资产。
- `GetRoomID()`: 返回房间的唯一 ID。
- `GetPosition()`: 返回房间的位置。
- `GetDirection()`: 返回房间的方向。
- `AreAllDoorsConnected()`: 如果此房间的所有门都连接到其他房间，则返回 true。
- `CountConnectedDoors()`: 返回此房间中连接到另一个房间的门的数量。
- `GetBoundsCenter()`: 返回房间的世界中心位置。
- `GetBoundsExtent()`: 返回房间的世界范围（半尺寸）。

### IDungeonCustomSerialization 接口

- `SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading)`: 序列化对象。
- `FixupReferences(UObject* Context)`: 修复引用。

### IDungeonSaveInterface 接口

- `PreSaveDungeon_Implementation()`: 保存地牢前调用。
- `PostLoadDungeon_Implementation()`: 加载地牢后调用。

### 房间状态

- `Generator()`: 返回生成器。
- `SetPlayerInside(bool PlayerInside)`: 设置玩家是否在房间内。
- `SetVisible(bool Visible)`: 设置房间是否可见。
- `IsReady()`: 房间是否已就绪。
- `IsPlayerInside()`: 玩家是否在房间内。
- `IsVisible()`: 房间是否可见。
- `ForceVisibility(bool bForce)`: 强制房间可见。
- `IsLocked()`: 房间是否被锁定。
- `Lock(bool lock)`: 锁定或解锁房间实例。

### 自定义数据

- `GetCustomData_BP(TSubclassOf<URoomCustomData> DataType, URoomCustomData*& Data)`: 访问房间的自定义数据。
- `HasCustomData_BP(const TSubclassOf<URoomCustomData>& DataType)`: 检查房间实例是否包含特定类型的自定义数据。
- `CreateCustomData(const TSubclassOf<URoomCustomData>& DataType)`: 创建自定义数据。
- `CreateAllCustomData()`: 创建所有自定义数据。
- `GetCustomData(const TSubclassOf<URoomCustomData>& DataType, URoomCustomData*& Data)`: 获取自定义数据。
- `HasCustomData(const TSubclassOf<URoomCustomData>& DataType)`: 检查是否有自定义数据。

### 随机数

- `GetRandomStream()`: 返回来自地牢生成器的 RandomStream。**已弃用**：请改用 Actor 上的 DeterministicRandom 组件。

### 门操作

- `GetDoor(int32 DoorIndex)`: 获取特定索引的门 Actor。
- `GetAllDoors(UPARAM(ref) TArray<ADoor*>& OutDoors)`: 用所有连接到房间的门 Actor 填充数组。
- `IsDoorConnected(int DoorIndex)`: 检查 DoorIndex 处的门是否连接到另一个房间。
- `GetConnectedRoomAt(int DoorIndex)`: 返回 DoorIndex 处连接的房间实例。
- `GetAllConnectedRooms(TArray<URoom*>& ConnectedRooms)`: 返回与此房间连接的所有房间实例。
- `GetConnectedRoomIndex(const URoom* OtherRoom)`: 返回提供的房间的索引，如果房间未连接则返回 -1。
- `GetDoorsWith(const URoom* OtherRoom, TArray<ADoor*>& Doors)`: 返回与此房间共享的门 Actor。

### 受保护函数

- `ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)`: 复制子对象。
- `RegisterReplicableSubobjects(bool bRegister)`: 注册可复制子对象。
- `SetPosition(const FIntVector& NewPosition)`: 设置位置。
- `SetDirection(EDoorDirection NewDirection)`: 设置方向。
- `UpdateVisibility()`: 更新可见性。
- `OnRep_RoomData()`: 房间数据复制时调用。
- `OnRep_Id()`: ID 复制时调用。
- `OnRep_Connections()`: 连接复制时调用。
- `OnRep_IsLocked()`: 锁定状态复制时调用。
- `OnInstanceLoaded()`: 实例加载时调用。

### 公共函数

- `Init(URoomData* RoomData, ADungeonGeneratorBase* Generator, int32 RoomId)`: 初始化房间。
- `Instantiate(UWorld* World)`: 实例化房间。
- `Destroy()`: 销毁房间。
- `GetLevelScript()`: 获取关卡脚本。
- `IsInstanceLoaded()`: 实例是否已加载。
- `IsInstanceUnloaded()`: 实例是否已卸载。
- `IsInstanceInitialized()`: 实例是否已初始化。
- `CreateLevelComponents(ARoomLevel* LevelActor)`: 创建关卡组件。
- `GetDoorWorldOrientation(int DoorIndex)`: 获取门的世界方向。
- `GetDoorWorldPosition(int DoorIndex)`: 获取门的世界位置。
- `GetConnectionCount()`: 获取连接数。
- `IsConnected(int32 DoorIndex)`: 检查门是否已连接。
- `SetConnection(int32 DoorIndex, URoomConnection* Conn)`: 设置连接。
- `GetConnectedRoom(int32 DoorIndex)`: 获取连接的房间。
- `GetFirstEmptyConnection()`: 获取第一个空连接。
- `GetAllEmptyConnections(TArray<int32>& EmptyConnections)`: 获取所有空连接。
- `IsDoorIndexValid(int32 DoorIndex)`: 检查门索引是否有效。
- `GetDoorIndexAt(FIntVector WorldPos, EDoorDirection WorldRot)`: 获取指定世界位置和方向的门索引。
- `GetOtherDoorIndex(int32 DoorIndex)`: 获取另一个门索引。
- `GetDoorDef(int32 DoorIndex)`: 获取门定义。
- `GetDoorDefAt(FIntVector WorldPos, EDoorDirection WorldRot)`: 获取指定世界位置和方向的门定义。
- `WorldToRoom(...)`: 将世界坐标转换为房间坐标。
- `RoomToWorld(...)`: 将房间坐标转换为世界坐标。
- `SetRotationFromDoor(int DoorIndex, EDoorDirection WorldRot)`: 根据门设置旋转。
- `SetPositionFromDoor(int DoorIndex, FIntVector WorldPos)`: 根据门设置位置。
- `SetPositionAndRotationFromDoor(int DoorIndex, FIntVector WorldPos, EDoorDirection WorldRot)`: 根据门设置位置和旋转。
- `IsOccupied(FIntVector Cell)`: 检查单元格是否被占用。
- `GetTransform()`: 获取变换。
- `GetBounds()`: 获取边界。
- `GetLocalBounds()`: 获取本地边界。
- `GetIntBounds()`: 获取整数边界。
- `GetVoxelBounds()`: 获取体素边界。
- `Overlap(...)`: 检查房间是否重叠。
- `GetRoomAt(FIntVector RoomCell, const TArray<URoom*>& RoomList)`: 获取指定房间单元的房间。

### 私有函数

- `LoadInstance(...)`: 加载关卡实例。
- `UnloadInstance(ULevelStreamingDynamic* Instance)`: 卸载关卡实例。
- `SerializeLevelActors(FSaveData& Data, bool bIsLoading)`: 序列化关卡 Actor。
- `DispatchCallbackToSavedLevelActors(TFunction<void(AActor*)> Callback)`: 向保存的关卡 Actor 分发回调。


好的，我们来详细看看 URoom::Instantiate 和 ADungeonGeneratorBase::FinalizeDungeon 是如何协同工作，最终加载关卡并调用 URoomData::InitializeRoom 的。

  1. `URoom::Instantiate`: 启动关卡加载

   * 调用者: 这个函数通常由 ADungeonGeneratorBase 在生成流程的后期调用，特别是在 FinalizeDungeon 阶段。
   * 核心职责: Instantiate 的主要任务是启动一个 URoom 实例对应关卡的异步加载过程。
   * 关键步骤:
       * 获取关卡路径: 它首先需要知道要加载哪个关卡。这个信息存储在 URoom 关联的 URoomData 对象的 Level 成员（一个 TSoftObjectPtr<UWorld>）中。
       * 创建流送实例: 它会调用 ULevelStreamingDynamic::LoadLevelInstance (或类似逻辑)。这个函数会：
           1. 检查目标关卡是否已经存在于内存中。
           2. 如果不存在，则开始异步加载该关卡资产（.umap 文件）。
           3. 创建一个 ULevelStreamingDynamic 对象，这个对象负责管理该关卡实例的加载、卸载和可见性。
           4. 将这个 ULevelStreamingDynamic 实例赋值给 URoom::Instance 成员变量。
       * 设置变换: 根据 URoom 的 Position 和 Direction，计算出关卡在世界中的正确 FTransform（位置和旋转）。这个变换会应用到 ULevelStreamingDynamic
         实例上，确保关卡被放置在地牢中的正确位置和朝向。
       * 绑定加载完成回调: 非常关键的一步，Instantiate 会将 ULevelStreamingDynamic 的 OnLevelLoaded 事件（或类似机制）绑定到 URoom 自身的一个回调函数上，通常是
         URoom::OnInstanceLoaded。这意味着当关卡完全加载并初始化完毕后，URoom::OnInstanceLoaded 会被自动调用。

   * 注意: Instantiate 本身不等待关卡加载完成。它只是启动了加载过程，然后立即返回。这是因为关卡加载是异步的，避免阻塞主线程。

  2. `ADungeonGeneratorBase::FinalizeDungeon`: 触发批量实例化

   * 调用者: 这是 ADungeonGenerator (继承自 ADungeonGeneratorBase) 在其 CreateDungeon_Implementation 函数的最后阶段调用的。
   * 核心职责: FinalizeDungeon 的主要任务是，在所有房间的逻辑位置、方向和连接都确定之后，批量触发所有房间的关卡加载，并将整个地牢图标记为已初始化。
   * 关键步骤:
       1. 遍历所有房间: 它会遍历 UDungeonGraph::Rooms 数组中的每一个 URoom 实例。
       2. 调用 `Instantiate`: 对每一个 URoom 实例，调用其 Instantiate 方法。这会为每个房间启动其关卡的异步加载。
       3. 等待加载完成 (隐式): FinalizeDungeon 本身并不显式等待所有关卡加载完成。它依赖于 URoom::OnInstanceLoaded 回调机制来处理后续步骤。
       4. 调用 `InitializeDungeon`: 在触发了所有 Instantiate 调用后，FinalizeDungeon 会调用蓝图可重写事件 InitializeDungeon(UDungeonGraph* 
          Rooms)。这是一个让开发者有机会在所有房间开始加载后、但可能还未完全加载完成前执行一些全局初始化逻辑的地方（例如，设置一些全局变量或触发某些事件）。

  3. `URoom::OnInstanceLoaded`: 完成加载并调用 `InitializeRoom`

   * 触发者: 当 URoom::Instantiate 启动的关卡异步加载过程完全完成后，由 ULevelStreamingDynamic 系统自动调用。
   * 核心职责: 这是真正完成加载后初始化工作的关键点。在这里，房间的关卡已经加载到内存并实例化，可以安全地访问其内部的 Actor 了。
   * 关键步骤:
       1. 获取关卡脚本 Actor: 通过 Instance->GetLevelScriptActor() 获取刚刚加载的关卡实例中的 ALevelScriptActor（在 ProceduralDungeon 中通常是 ARoomLevel 类型）。这个
          Actor 是关卡蓝图的实例。
       2. 初始化 `ARoomLevel`: 调用 ARoomLevel::Init(this)，将 URoom 实例自身传递给 ARoomLevel，建立起它们之间的双向链接。ARoomLevel 现在知道了它代表的是哪个 URoom 实例。
       3. 调用 `URoomData::InitializeRoom`: 这是最关键的一步。URoom::OnInstanceLoaded 会获取关联的 URoomData 对象，然后调用 RoomData->InitializeRoom(this, Graph)。
           * this: 传递当前的 URoom 实例。
           * Graph: 传递管理整个地牢的 UDungeonGraph 实例。
           * 这使得 URoomData（通常在蓝图中实现）有机会在其对应的关卡实例加载完成后，执行特定的初始化逻辑。例如，在关卡中生成特定的道具、修改某些 Actor
             的属性、或者根据房间在地牢中的位置和连接情况设置一些动态内容。
       4. 创建自定义数据组件: 遍历 URoom::CustomData 数组，为每个自定义数据项调用 URoomCustomData::CreateLevelComponent，在 ARoomLevel 上创建并附加相应的 UActorComponent。
       5. 标记为已初始化: 将 URoom 的内部状态标记为已初始化 (IsInstanceInitialized() 将返回 true)。

  总结流程:

   1. ADungeonGeneratorBase::FinalizeDungeon 遍历所有已逻辑生成的 URoom。
   2. 对每个 URoom，调用 URoom::Instantiate。
   3. URoom::Instantiate 启动对应关卡的异步加载，并设置加载完成后的回调 URoom::OnInstanceLoaded。
   4. ADungeonGeneratorBase::FinalizeDungeon 调用蓝图事件 InitializeDungeon。
   5. 当某个房间的关卡异步加载完成后，URoom::OnInstanceLoaded 被自动调用。
   6. URoom::OnInstanceLoaded 完成 ARoomLevel 的初始化，并最终调用 URoomData::InitializeRoom，允许用户自定义的蓝图逻辑在此时执行。
   7. 这个过程对所有房间并行进行，直到所有房间都加载并初始化完毕。