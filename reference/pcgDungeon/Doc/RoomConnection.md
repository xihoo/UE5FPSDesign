# RoomConnection 类

## 概述

`URoomConnection` 类是一个表示两个房间之间连接的地牢图子对象。它继承自 `UReplicableObject`，并实现了 `IDungeonCustomSerialization` 和 `IDungeonSaveInterface` 接口。

## 成员函数

### IDungeonCustomSerialization 接口

- `SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading)`: 序列化对象。
- `FixupReferences(UObject* Context)`: 修复引用。

### IDungeonSaveInterface 接口

- `PreSaveDungeon_Implementation()`: 保存地牢前调用。
- `PostLoadDungeon_Implementation()`: 加载地牢后调用。

### 公共函数

- `GetID()`: 获取连接的 ID。
- `GetRoomA()`: 获取房间 A。
- `GetRoomB()`: 获取房间 B。
- `GetRoomADoorId()`: 获取房间 A 的门 ID。
- `GetRoomBDoorId()`: 获取房间 B 的门 ID。
- `GetOtherRoom(const URoom* FromRoom)`: 获取另一个房间。
- `GetOtherDoorId(const URoom* FromRoom)`: 获取另一个门的 ID。
- `IsDoorInstanced()`: 检查门是否已实例化。
- `GetDoorInstance()`: 获取门实例。
- `GetDoorLocation(bool bIgnoreGeneratorTransform)`: 获取门的位置。
- `GetDoorRotation(bool bIgnoreGeneratorTransform)`: 获取门的旋转。
- `SetDoorClass(TSubclassOf<ADoor> DoorClass, bool bFlipped)`: 设置门类。
- `InstantiateDoor(UWorld* World, AActor* Owner = nullptr, bool bUseOwnerTransform = false)`: 实例化门。
- `GetOtherRoom(const URoomConnection* Conn, const URoom* FromRoom)`: 获取另一个房间（静态函数）。
- `GetOtherDoorId(const URoomConnection* Conn, const URoom* FromRoom)`: 获取另一个门的 ID（静态函数）。
- `GetDoorInstance(const URoomConnection* Conn)`: 获取门实例（静态函数）。
- `GetDoorType(const URoomConnection* Conn)`: 获取门类型（静态函数）。
- `CreateConnection(URoom* RoomA, int32 DoorA, URoom* RoomB, int32 DoorB, UObject* Outer, int32 IdInOuter)`: 创建连接。

### 受保护函数

- `GetRoomA_BP()`: 获取房间 A（蓝图纯函数）。
- `GetRoomB_BP()`: 获取房间 B（蓝图纯函数）。

### 私有函数

- `OnRep_ID()`: ID 复制时调用。
- `OnRep_RoomA()`: 房间 A 复制时调用。
- `OnRep_RoomB()`: 房间 B 复制时调用。

## 成员变量

- `ID` (int32): 连接的 ID。
- `RoomA` (TWeakObjectPtr<URoom>): 房间 A。
- `RoomADoorId` (int32): 房间 A 的门 ID。
- `RoomB` (TWeakObjectPtr<URoom>): 房间 B。
- `RoomBDoorId` (int32): 房间 B 的门 ID。
- `DoorClass` (TSubclassOf<ADoor>): 门类。
- `bFlipped` (bool): 是否翻转。
- `DoorInstance` (TWeakObjectPtr<ADoor>): 门实例。

## 私有结构体

### FSaveData

存储仅在保存/加载游戏期间使用的临时数据。

- `RoomAID` (int64): 房间 A 的 ID。
- `RoomBID` (int64): 房间 B 的 ID。
- `DoorSavedData` (TArray<uint8>): 门的保存数据。