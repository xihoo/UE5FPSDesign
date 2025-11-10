# DungeonGeneratorBase.cpp

## 概述

`DungeonGeneratorBase.cpp` 文件包含了 `ADungeonGeneratorBase` 类的实现。这个类是插件的主要 Actor 基类，负责生成地牢并通过网络进行复制。它实现了一个有限状态机来管理地牢的生成、加载和卸载过程。

## 函数实现

### ADungeonGeneratorBase()

```cpp
ADungeonGeneratorBase::ADungeonGeneratorBase()
```

构造函数。初始化地牢生成器的基本属性：

- 启用 Tick。
- 设置默认的种子类型为随机。
- 设置默认种子和种子增量。
- 设置网络相关属性，如始终相关、复制、网络优先级等。
- 创建默认的 `UDungeonGraph` 子对象。
- 初始化八叉树。

### GetLifetimeReplicatedProps()

```cpp
void ADungeonGeneratorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
```

定义需要复制的属性。复制的属性包括：

- `Seed`: 生成地牢时使用的种子。

### ReplicateSubobjects()

```cpp
bool ADungeonGeneratorBase::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
```

复制子对象。调用 `Graph` 的 `ReplicateSubobject` 方法来复制地牢图。

### SaveDungeon()

```cpp
void ADungeonGeneratorBase::SaveDungeon(FDungeonSaveData& SaveData)
```

保存地牢状态。将地牢的 GUID 和序列化数据存储到 `SaveData` 中。

### LoadDungeon()

```cpp
void ADungeonGeneratorBase::LoadDungeon(const FDungeonSaveData& SaveData)
```

加载地牢状态。从 `SaveData` 中读取数据并反序列化地牢。

### SerializeDungeon()

```cpp
void ADungeonGeneratorBase::SerializeDungeon(FArchive& Archive)
```

序列化地牢。使用 `FDungeonSaveProxyArchive` 和 `FStructuredArchive` 来序列化地牢数据。

### SerializeObject()

```cpp
void ADungeonGeneratorBase::SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading)
```

序列化对象。处理地牢对象的序列化，包括属性和地牢图。

### PostActorCreated()

```cpp
void ADungeonGeneratorBase::PostActorCreated()
```

Actor 创建后调用。生成或重用 GUID。

### PostInitializeComponents()

```cpp
void ADungeonGeneratorBase::PostInitializeComponents()
```

组件初始化后调用。设置 `Graph` 的生成器并注册为可复制对象。

### EndPlay()

```cpp
void ADungeonGeneratorBase::EndPlay(EEndPlayReason::Type EndPlayReason)
```

游戏结束时调用。如果结束原因是销毁，则卸载所有房间。

### Tick()

```cpp
void ADungeonGeneratorBase::Tick(float DeltaTime)
```

每帧调用。调用当前状态的 Tick 函数。

### Generate()

```cpp
void ADungeonGeneratorBase::Generate()
```

生成地牢。仅在服务器上调用，设置生成标志。

### Unload()

```cpp
void ADungeonGeneratorBase::Unload()
```

卸载地牢。仅在服务器上调用，标记图已修改。

### StartNewDungeon()

```cpp
void ADungeonGeneratorBase::StartNewDungeon()
```

开始新地牢生成。调用 `OnGenerationInit` 并清空图。

### FinalizeDungeon()

```cpp
void ADungeonGeneratorBase::FinalizeDungeon()
```

最终化地牢。初始化房间并调用 `InitializeDungeon`。

### CreateRoomInstance()

```cpp
URoom* ADungeonGeneratorBase::CreateRoomInstance(URoomData* RoomData)
```

创建房间实例。创建一个新的 `URoom` 对象并初始化它。

### TryPlaceRoom()

```cpp
bool ADungeonGeneratorBase::TryPlaceRoom(URoom* const& Room, int DoorIndex, const FDoorDef& TargetDoor, const UWorld* World) const
```

尝试放置房间。设置房间位置和旋转，并检查是否与现有房间或世界碰撞。

### AddRoomToDungeon()

```cpp
bool ADungeonGeneratorBase::AddRoomToDungeon(URoom* const& Room, const TArray<int>& DoorsToConnect, bool bFailIfNotConnected)
```

将房间添加到地牢。连接门并添加房间到图中。

### CreateDungeon_Implementation()

```cpp
bool ADungeonGeneratorBase::CreateDungeon_Implementation()
```

创建地牢的默认实现。记录错误信息，需要在子类中重写。

### ChooseDoorClasses()

```cpp
void ADungeonGeneratorBase::ChooseDoorClasses()
```

选择门类。为所有连接选择门类。

### UpdateRoomVisibility()

```cpp
void ADungeonGeneratorBase::UpdateRoomVisibility()
```

更新房间可见性。根据玩家位置和遮挡剔除设置更新房间的可见性。

### Reset()

```cpp
void ADungeonGeneratorBase::Reset()
```

重置生成器。清空当前玩家房间并销毁八叉树。

### UpdateOctree()

```cpp
void ADungeonGeneratorBase::UpdateOctree()
```

更新八叉树。清空并重新填充八叉树。

### UpdateSeed()

```cpp
void ADungeonGeneratorBase::UpdateSeed()
```

更新种子。根据种子类型更新随机种子。

### DrawDebug()

```cpp
void ADungeonGeneratorBase::DrawDebug() const
```

绘制调试信息。绘制地牢边界框。

### 状态机函数

#### SetState()

```cpp
void ADungeonGeneratorBase::SetState(EGenerationState NewState)
```

设置状态。结束当前状态并开始新状态。

#### OnStateBegin()

```cpp
void ADungeonGeneratorBase::OnStateBegin(EGenerationState State)
```

状态开始时调用。根据状态执行相应操作。

#### OnStateTick()

```cpp
void ADungeonGeneratorBase::OnStateTick(EGenerationState State)
```

状态 Tick 时调用。根据状态执行相应操作。

#### OnStateEnd()

```cpp
void ADungeonGeneratorBase::OnStateEnd(EGenerationState State)
```

状态结束时调用。根据状态执行相应操作。

### 蓝图原生事件默认实现

- `ChooseDoor_Implementation()`: 选择门的默认实现。
- `InitializeDungeon_Implementation()`: 初始化地牢的默认实现。
- `GetVisibilityPawn_Implementation()`: 获取可见性 Pawn 的默认实现。
- `OnPreGeneration_Implementation()`: 生成前的默认实现。
- `OnPostGeneration_Implementation()`: 生成后的默认实现。
- `OnGenerationInit_Implementation()`: 生成初始化的默认实现。
- `OnGenerationSuccess_Implementation()`: 生成成功的默认实现。
- `OnGenerationFailed_Implementation()`: 生成失败的默认实现。
- `OnRoomAdded_Implementation()`: 房间添加的默认实现。
- `OnFailedToAddRoom_Implementation()`: 添加房间失败的默认实现。

### 工具函数

- `GetRandomRoomData()`: 获取随机房间数据。
- `GetRandomRoomDataWeighted()`: 获取加权随机房间数据。
- `GetCompatibleRoomData()`: 获取兼容的房间数据。
- `GetProgress()`: 获取生成进度。
- `GetRoomByIndex()`: 根据索引获取房间。
- `SaveAllDungeons()`: 保存所有地牢。
- `LoadAllDungeons()`: 加载所有地牢。
- `SetSeed()`: 设置种子。
- `GetSeed()`: 获取种子。
- `GetDungeonOffset()`: 获取地牢偏移。
- `GetDungeonRotation()`: 获取地牢旋转。
- `GetDungeonTransform()`: 获取地牢变换。

### 控制台命令

- `pd.Generate`: 生成地牢。
- `pd.Unload`: 卸载地牢。