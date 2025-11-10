# DungeonGeneratorBase 类

## 概述

`ADungeonGeneratorBase` 类是插件的主要 Actor 基类。地牢生成器负责生成地牢并通过网络进行复制。这个基类是抽象的，你需要重写 `CreateDungeon` 函数来编写你自己的生成算法。

## 枚举

### EGenerationResult

- `None`
- `Error`
- `Success`

### EGeneratorFlags

- `None`
- `Generating`
- `LoadSavedDungeon`
- `All`

## 结构体

### FDungeonSaveData

- `GeneratorId` (FGuid): 生成器的 GUID。
- `Data` (TArray<uint8>): 保存的数据。

## 成员变量

- `bUseGeneratorTransform` (bool): 如果勾选，房间的位置和旋转将相对于此 Actor 的变换。如果未勾选，房间将相对于世界原点放置。
- `SeedType` (ESeedType): 如何处理每次生成调用时的种子。Random（随机）：生成并使用随机种子。Auto Increment（自动递增）：首次生成时使用 Seed，并在后续每次生成时将其递增 SeedIncrement。Fixed（固定）：每次生成时仅使用 Seed。
- `SeedIncrement` (uint32): 当 SeedType 为 AutoIncrement 时，每次后续地牢生成的增量数。
- `bUseWorldCollisionChecks` (bool): 如果勾选，在地牢生成过程中尝试放置新房间时，将进行框重叠测试，以确保房间不会生成在持久世界中的现有网格内。这是一项繁重的工作，应仅在必要时勾选。仅在生成过程中有影响，对游戏玩法无影响。
- `Seed` (uint32): 种子值。
- `Random` (FRandomStream): 随机流。
- `bUseJsonSave` (bool): 如果为 true，地牢将以人类可读的 json 格式保存。*警告*：这仅在编辑器和开发版本中可用，在打包版本中不会有任何变化。它应仅用于调试目的。
- `bDrawDebugDungeonBounds` (bool): 绘制计算出的地牢边界框。
- `bRebuildNavmesh` (bool): 如果为 true，生成器将管理默认的 UE navmesh 系统，以便在生成结束时重建它。如果为 false，生成器将不对导航系统执行任何操作。
- `Graph` (UDungeonGraph*): 地牢图。
- `Id` (FGuid): GUID。
- `CurrentState` (EGenerationState): 当前状态。
- `Flags` (EGeneratorFlags): 生成器标志。
- `bShouldIncrement` (bool): 设置以避免在手动设置种子后增加种子。
- `Octree` (TUniquePtr<FDungeonOctree>): 遮挡剔除系统。
- `CurrentPlayerRooms` (TSet<URoom*>): 当前玩家房间。
- `bWasOcclusionEnabled` (bool): 仅用于检测遮挡设置是否更改。
- `PreviousOcclusionDistance` (uint32): 仅用于检测遮挡距离是否更改。
- `CachedTmpRoomCount` (int32): 仅用于在生成期间计算未加载/已加载/已初始化的房间。
- `WorldCollisionParams` (FCollisionQueryParams): 仅在 bUseWorldCollisionChecks 为 true 时使用，用于缓存碰撞参数。

## 成员函数

### 构造函数

- `ADungeonGeneratorBase()`: 构造函数。

### 虚函数重写 (AActor 接口)

- `PostInitializeComponents()`: 组件初始化后调用。
- `EndPlay()`: 游戏结束时调用。
- `Tick()`: 每帧调用。
- `ReplicateSubobjects()`: 复制子对象。
- `PostActorCreated()`: Actor 创建后调用。

### 蓝图可调用函数

- `Generate()`: 更新种子并在所有客户端上调用生成。在客户端上调用时无操作。
- `Unload()`: 卸载当前地牢。在客户端上调用时无操作。
- `SaveDungeon(FDungeonSaveData& SaveData)`: 从当前地牢状态创建保存数据。
- `LoadDungeon(const FDungeonSaveData& SaveData)`: 从先前保存的数据加载地牢。
- `SetSeed(int32 NewSeed)`: 设置种子。
- `GetSeed()`: 获取种子。
- `GetGuid()`: 获取 GUID。
- `UseGeneratorTransform()`: 是否使用生成器变换。
- `GetDungeonOffset()`: 获取地牢偏移。
- `GetDungeonRotation()`: 获取地牢旋转。
- `GetDungeonTransform()`: 获取地牢变换。
- `GetRooms()`: 获取房间图。
- `GetCurrentState()`: 获取当前状态。
- `GetRandomRoomData(TArray<URoomData*> RoomDataArray)`: 从提供的数组中返回一个随机的 RoomData。
- `GetRandomRoomDataWeighted(const TMap<URoomData*, int>& RoomDataWeightedMap)`: 从加权映射中返回一个随机的 RoomData。
- `GetCompatibleRoomData(bool& bSuccess, TArray<URoomData*>& CompatibleRooms, const TArray<URoomData*>& RoomDataArray, const FDoorDef& DoorData)`: 返回至少有一个与提供的门数据兼容的门的房间数据数组。
- `GetRandomStream()`: 访问程序地牢的随机流。你应该始终使用它进行程序生成。
- `GetProgress()`: 返回当前生成进度。
- `GetRoomByIndex(int64 Index)`: 通过索引获取房间。
- `SaveAllDungeons(const UObject* WorldContextObject, TArray<FDungeonSaveData>& SavedData)`: 保存所有地牢。
- `LoadAllDungeons(const UObject* WorldContextObject, const TArray<FDungeonSaveData>& SavedData)`: 加载所有地牢。

### 蓝图原生事件 (需在蓝图中重写)

- `ChooseDoor(const URoomData* CurrentRoom, const URoom* CurrentRoomInstance, const URoomData* NextRoom, const URoom* NextRoomInstance, const UDoorType* DoorType, bool& Flipped)`: 返回将在当前房间和下一个房间之间生成的门。
- `InitializeDungeon(const UDungeonGraph* Rooms)`: 在生成步骤中初始化房间实例。
- `GetVisibilityPawn()`: 返回用于房间剔除系统的 pawn。这个 pawn 也会影响房间的 PlayerInside 变量。
- `OnPreGeneration()`: 在生成新地牢之前调用一次。
- `OnPostGeneration()`: 在所有地牢生成后调用一次（即使失败）。
- `OnGenerationInit()`: 在尝试生成新地牢之前以及每次 IsValidDungeon 返回 false 时调用。
- `OnGenerationSuccess()`: 当地牢成功生成时调用（IsValidDungeon 返回 true）。
- `OnGenerationFailed()`: 当所有地牢生成尝试都耗尽时调用（IsValidDungeon 总是返回 false）。没有生成地牢。
- `OnRoomAdded(const URoomData* NewRoom, const TScriptInterface<IReadOnlyRoom>& RoomInstance)`: 每次在地牢中添加房间时调用（但尚未生成）。
- `OnFailedToAddRoom(const URoomData* FromRoom, const FDoorDef& FromDoor)`: 每次无法在门处放置房间时调用（所有房间放置尝试都已耗尽）。

### 蓝图可调用函数 (受保护)

- `CreateDungeon_Implementation()`: 创建虚拟地牢（不加载也不初始化房间关卡）。
- `StartNewDungeon()`: 清除当前图并调用 GenerationInit 事件。
- `FinalizeDungeon()`: 在所有房间都已放置和连接后初始化房间实例（调用 InitializeDungeon）。
- `CreateRoomInstance(URoomData* RoomData)`: 使用提供的房间数据创建并初始化新的房间实例。
- `TryPlaceRoom(URoom* const& Room, int DoorIndex, const FDoorDef& TargetDoor, const UWorld* World = nullptr)`: 设置房间实例的位置和旋转，如果没有任何东西与之碰撞，则返回 true。
- `AddRoomToDungeon(URoom* const& Room, const TArray<int>& DoorsToConnect, bool bFailIfNotConnected = true)`: 通过将房间添加到地牢图来完成房间创建。OnRoomAdded 在此处调用。

### 私有函数

- `ChooseDoorClasses()`: 为所有房间连接选择门类。这必须在 Graph->InitRooms() *之后*发生，以便能够为未连接的门选择门类。
- `UpdateRoomVisibility()`: 根据玩家位置更新房间可见性。
- `Reset()`: 重置特定生成的所有数据。
- `UpdateOctree()`: 从地牢图中清除并重新填充八叉树。
- `UpdateSeed()`: 根据种子类型设置初始化种子。
- `IsGenerating()`: 是否正在生成。
- `IsLoadingSavedDungeon()`: 是否正在加载已保存的地牢。
- `DrawDebug()`: 绘制调试信息。
- `SetState(EGenerationState NewState)`: 设置状态。
- `OnStateBegin(EGenerationState State)`: 状态开始时调用。
- `OnStateTick(EGenerationState State)`: 状态每帧调用。
- `OnStateEnd(EGenerationState State)`: 状态结束时调用。

### 事件

- `OnPreGenerationEvent`: 在生成新地牢之前调用一次。
- `OnPostGenerationEvent`: 在所有地牢生成后调用一次（即使失败）。
- `OnGenerationInitEvent`: 在尝试生成新地牢之前以及每次 IsValidDungeon 返回 false 时调用。
- `OnGenerationSuccessEvent`: 当地牢成功生成时调用（IsValidDungeon 返回 true）。
- `OnGenerationFailedEvent`: 当所有地牢生成尝试都耗尽时调用（IsValidDungeon 总是返回 false）。没有生成地牢。
- `OnRoomAddedEvent`: 每次在地牢中添加房间时调用（但尚未生成）。
- `OnFailedToAddRoomEvent`: 每次无法在门处放置房间时调用（所有房间放置尝试都已耗尽）。