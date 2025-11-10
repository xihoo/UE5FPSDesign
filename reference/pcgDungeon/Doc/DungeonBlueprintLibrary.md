# DungeonBlueprintLibrary 类

## 概述

`UDungeonBlueprintLibrary` 类是一个蓝图函数库，提供了一系列与地牢生成和管理相关的实用函数。这些函数可以在蓝图中直接调用，方便开发者进行地牢相关的操作。

## 成员函数

### 门相关函数

- `IsDoorOfType(const TSubclassOf<class ADoor> DoorClass, const class UDoorType* DoorType)`: 检查门类是否为指定类型。
- `DoorDef_GetOpposite(const FDoorDef& DoorDef)`: 获取门定义的相反方向。

### 数据表相关函数

- `CompareDataTableRows(const FDataTableRowHandle& A, const FDataTableRowHandle& B)`: 比较两个数据表行句柄是否相等。

### 房间相关函数

- `GetOwningRoom(const AActor* Target)`: 获取Actor所在的房间实例。
- `GetOwningRoomCustomData(const AActor* Target, TSubclassOf<URoomCustomData> CustomDataClass, URoomCustomData*& CustomData)`: 获取拥有房间的自定义数据。

### 门方向数学工具函数

- `DoorDirection_Valid(const EDoorDirection& A)`: 检查门方向值是否有效。
- `DoorDirection_Add(const EDoorDirection& A, const EDoorDirection& B)`: 门方向加法。
- `DoorDirection_Sub(const EDoorDirection& A, const EDoorDirection& B)`: 门方向减法。
- `DoorDirection_Increment(UPARAM(ref) EDoorDirection& A)`: 递增门方向。
- `DoorDirection_Decrement(UPARAM(ref) EDoorDirection& A)`: 递减门方向。
- `DoorDirection_Negate(UPARAM(ref) EDoorDirection& A)`: 取反门方向。
- `DoorDirection_Opposite(const EDoorDirection& A)`: 获取门方向的相反方向。
- `DoorDirection_ToIntVector(const EDoorDirection& A)`: 将门方向枚举值转换为单位整数向量。
- `DoorDirection_ToAngle(const EDoorDirection& A)`: 将门方向枚举值转换为角度。

### 地牢数学变换函数

- `IntVector_Next(const FIntVector& Vector, const EDoorDirection& Direction)`: 获取指定方向的邻居单元格。
- `IntVector_Rotate(const FIntVector& Vector, const EDoorDirection& Direction)`: 旋转向量。
- `Dungeon_TransformPosition(const FIntVector& LocalPos, const FIntVector& Translation, const EDoorDirection& Rotation)`: 将局部坐标转换为地牢坐标。
- `Dungeon_InverseTransformPosition(const FIntVector& DungeonPos, const FIntVector& Translation, const EDoorDirection& Rotation)`: 将地牢坐标转换为局部坐标。
- `Dungeon_TransformDoorDef(const FDoorDef& DoorDef, const FIntVector& Translation, const EDoorDirection& Rotation)`: 将门定义从局部坐标转换为地牢坐标。
- `Dungeon_InverseTransformDoorDef(const FDoorDef& DoorDef, const FIntVector& Translation, const EDoorDirection& Rotation)`: 将门定义从地牢坐标转换为局部坐标。

### 整数向量操作函数

- `IntVector_Add(const FIntVector& A, const FIntVector& B)`: 整数向量加法。
- `IntVector_Subtract(const FIntVector& A, const FIntVector& B)`: 整数向量减法。
- `IntVector_Equal(const FIntVector& A, const FIntVector& B)`: 整数向量相等比较。
- `IntVector_NotEqual(const FIntVector& A, const FIntVector& B)`: 整数向量不等比较。

### 插件设置访问函数

- `Settings_RoomUnit()`: 获取房间单元大小（以虚幻单位表示）。
- `Settings_DefaultDoorSize()`: 获取默认门的大小。
- `Settings_DoorOffset()`: 获取门的偏移量（以房间单元高度的百分比表示）。
- `Settings_OcclusionCulling()`: 检查插件的遮挡剔除系统是否启用。
- `Settings_SetOcclusionCulling(bool Enable)`: 启用/禁用插件的遮挡剔除系统。
- `Settings_OcclusionDistance()`: 获取从玩家房间可见的房间数量（1表示仅玩家房间可见）。
- `Settings_SetOcclusionDistance(int32 Distance)`: 设置从玩家房间可见的房间数量。
- `Settings_OccludeDynamicActors()`: 检查是否应使用房间切换动态Actor的可见性。

### 游戏玩法工具函数

- `Spectate(APlayerController* Controller, bool DestroyPawn = true)`: 设置玩家为观察者模式。