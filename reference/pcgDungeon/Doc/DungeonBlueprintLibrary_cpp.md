# DungeonBlueprintLibrary.cpp

## 概述

`DungeonBlueprintLibrary.cpp` 文件包含了 `UDungeonBlueprintLibrary` 类的实现。这个类是一个蓝图函数库，提供了一系列与地牢生成和管理相关的实用函数。

## 函数实现

### IsDoorOfType()

```cpp
bool UDungeonBlueprintLibrary::IsDoorOfType(const TSubclassOf<ADoor> DoorClass, const UDoorType* DoorType)
```

检查门类是否为指定类型。通过获取门类的默认对象并比较其门类型来实现。

### CompareDataTableRows()

```cpp
bool UDungeonBlueprintLibrary::CompareDataTableRows(const FDataTableRowHandle& A, const FDataTableRowHandle& B)
```

比较两个数据表行句柄是否相等。

### GetOwningRoom()

```cpp
URoom* UDungeonBlueprintLibrary::GetOwningRoom(const AActor* Target)
```

获取 Actor 所在的房间实例。首先检查 Actor 是否为 `ARoomLevel`，如果是则直接返回其房间。否则，通过 Actor 的关卡获取关卡脚本 Actor，并从中获取房间。

### GetOwningRoomCustomData()

```cpp
bool UDungeonBlueprintLibrary::GetOwningRoomCustomData(const AActor* Target, TSubclassOf<URoomCustomData> CustomDataClass, URoomCustomData*& CustomData)
```

获取拥有房间的自定义数据。首先获取 Actor 所在的房间，然后从房间中获取指定类型的自定义数据。

### DoorDef_GetOpposite()

```cpp
FDoorDef UDungeonBlueprintLibrary::DoorDef_GetOpposite(const FDoorDef& DoorDef)
```

获取门定义的相反方向。

### IntVector_Next()

```cpp
FIntVector UDungeonBlueprintLibrary::IntVector_Next(const FIntVector& Vector, const EDoorDirection& Direction)
```

获取指定方向的邻居单元格。通过将向量与方向的整数向量相加来实现。

### IntVector_Rotate()

```cpp
FIntVector UDungeonBlueprintLibrary::IntVector_Rotate(const FIntVector& Vector, const EDoorDirection& Direction)
```

旋转向量。使用 `Rotate` 函数来实现。

### Dungeon_TransformPosition()

```cpp
FIntVector UDungeonBlueprintLibrary::Dungeon_TransformPosition(const FIntVector& LocalPos, const FIntVector& Translation, const EDoorDirection& Rotation)
```

将局部坐标转换为地牢坐标。使用 `Transform` 函数来实现。

### Dungeon_InverseTransformPosition()

```cpp
FIntVector UDungeonBlueprintLibrary::Dungeon_InverseTransformPosition(const FIntVector& DungeonPos, const FIntVector& Translation, const EDoorDirection& Rotation)
```

将地牢坐标转换为局部坐标。使用 `InverseTransform` 函数来实现。

### Dungeon_TransformDoorDef()

```cpp
FDoorDef UDungeonBlueprintLibrary::Dungeon_TransformDoorDef(const FDoorDef& DoorDef, const FIntVector& Translation, const EDoorDirection& Rotation)
```

将门定义从局部坐标转换为地牢坐标。使用 `FDoorDef::Transform` 函数来实现。

### Dungeon_InverseTransformDoorDef()

```cpp
FDoorDef UDungeonBlueprintLibrary::Dungeon_InverseTransformDoorDef(const FDoorDef& DoorDef, const FIntVector& Translation, const EDoorDirection& Rotation)
```

将门定义从地牢坐标转换为局部坐标。使用 `FDoorDef::InverseTransform` 函数来实现。

### IntVector_Add()

```cpp
FIntVector UDungeonBlueprintLibrary::IntVector_Add(const FIntVector& A, const FIntVector& B)
```

整数向量加法。

### IntVector_Subtract()

```cpp
FIntVector UDungeonBlueprintLibrary::IntVector_Subtract(const FIntVector& A, const FIntVector& B)
```

整数向量减法。

### IntVector_Equal()

```cpp
bool UDungeonBlueprintLibrary::IntVector_Equal(const FIntVector& A, const FIntVector& B)
```

整数向量相等比较。

### IntVector_NotEqual()

```cpp
bool UDungeonBlueprintLibrary::IntVector_NotEqual(const FIntVector& A, const FIntVector& B)
```

整数向量不等比较。

### Settings_RoomUnit()

```cpp
FVector UDungeonBlueprintLibrary::Settings_RoomUnit()
```

获取房间单元大小（以虚幻单位表示）。从插件设置中获取。

### Settings_DefaultDoorSize()

```cpp
FVector UDungeonBlueprintLibrary::Settings_DefaultDoorSize()
```

获取默认门的大小。从插件设置中获取。

### Settings_DoorOffset()

```cpp
float UDungeonBlueprintLibrary::Settings_DoorOffset()
```

获取门的偏移量（以房间单元高度的百分比表示）。从插件设置中获取。

### Settings_OcclusionCulling()

```cpp
bool UDungeonBlueprintLibrary::Settings_OcclusionCulling()
```

检查插件的遮挡剔除系统是否启用。从插件设置中获取。

### Settings_SetOcclusionCulling()

```cpp
void UDungeonBlueprintLibrary::Settings_SetOcclusionCulling(bool Enable)
```

启用/禁用插件的遮挡剔除系统。设置插件设置中的值。

### Settings_OcclusionDistance()

```cpp
int32 UDungeonBlueprintLibrary::Settings_OcclusionDistance()
```

获取从玩家房间可见的房间数量（1表示仅玩家房间可见）。从插件设置中获取。

### Settings_SetOcclusionDistance()

```cpp
void UDungeonBlueprintLibrary::Settings_SetOcclusionDistance(int32 Distance)
```

设置从玩家房间可见的房间数量。设置插件设置中的值。

### Settings_OccludeDynamicActors()

```cpp
bool UDungeonBlueprintLibrary::Settings_OccludeDynamicActors()
```

检查是否应使用房间切换动态Actor的可见性。从插件设置中获取。

### Spectate()

```cpp
void UDungeonBlueprintLibrary::Spectate(APlayerController* Controller, bool DestroyPawn)
```

设置玩家为观察者模式。将玩家控制器的状态更改为观察者，并可选择销毁玩家的 Pawn。