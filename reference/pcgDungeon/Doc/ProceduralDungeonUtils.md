# ProceduralDungeonUtils 类

## 概述

`ProceduralDungeonUtils.h` 定义了 Procedural Dungeon 插件使用的各种实用函数和工具。它包含用于整数向量操作、地牢坐标转换、随机数生成、世界工具和对象工具的命名空间。

## 命名空间

### IntVector

提供用于 `FIntVector` 的实用函数。

- `Min(const FIntVector& A, const FIntVector& B)`: 返回 A 和 B 的分量最小值。
- `Max(const FIntVector& A, const FIntVector& B)`: 返回 A 和 B 的分量最大值。
- `MinMax(const FIntVector& A, const FIntVector& B, FIntVector& OutMin, FIntVector& OutMax)`: 返回 A 和 B 的分量最小值和最大值。

### Dungeon

提供用于地牢相关操作的实用函数。

#### 坐标转换

- `ToWorldLocation(FIntVector RoomPoint)`: 返回房间单元中点的真实世界位置。
- `ToWorldVector(FIntVector RoomVector)`: 返回房间单元中向量（无偏移）的真实世界向量。
- `ToWorld(const FBoxMinAndMax& Box, const FTransform& Transform = FTransform::Identity)`: 将边界框从地牢坐标转换为世界坐标，并应用可选的变换。
- `ToWorld(const FBoxCenterAndExtent& Box, const FTransform& Transform = FTransform::Identity)`: 将边界框从地牢坐标转换为世界坐标，并应用可选的变换。
- `ToRoomLocation(FVector WorldPoint)`: 返回真实世界中点的房间单元位置。
- `ToRoomVector(FVector WorldVector)`: 返回真实世界中向量（无偏移）的房间单元向量。
- `SnapPoint(FVector Point)`: 返回真实世界中点的最近房间单元点的真实世界位置。

#### 权重映射

- `GetTotalWeight(const TMap<T, int>& WeightMap)`: 获取权重映射的总权重。
- `GetWeightedAt(const TMap<T, int>& WeightMap, int Index)`: 根据索引获取权重映射中的元素。

#### 插件设置

- `RoomUnit()`: 获取房间单元大小。
- `DefaultDoorSize()`: 获取默认门大小。
- `DefaultDoorColor()`: 获取默认门颜色。
- `DoorOffset()`: 获取门偏移量。
- `OcclusionCulling()`: 检查是否启用遮挡剔除。
- `UseLegacyOcclusion()`: 检查是否使用旧版遮挡剔除。
- `OcclusionDistance()`: 获取遮挡距离。
- `OccludeDynamicActors()`: 检查是否遮挡动态 Actor。
- `DrawDebug()`: 检查是否绘制调试信息。
- `DrawOnlyWhenEditingRoom()`: 检查是否仅在编辑房间时绘制调试信息。
- `ShowRoomOrigin()`: 检查是否显示房间原点。
- `FlipDoorArrow()`: 检查是否翻转门箭头。
- `DoorArrowLength()`: 获取门箭头长度。
- `DoorArrowHeadSize()`: 获取门箭头头部大小。
- `CanLoop()`: 检查是否可以循环。
- `RoomObjectType()`: 获取房间对象类型。
- `MaxGenerationTryBeforeGivingUp()`: 获取生成失败前的最大尝试次数。
- `MaxRoomPlacementTryBeforeGivingUp()`: 获取房间放置失败前的最大尝试次数。
- `RoomLimit()`: 获取房间限制。
- `EnableOcclusionCulling(bool Enable)`: 启用或禁用遮挡剔除。
- `SetOcclusionDistance(int32 Distance)`: 设置遮挡距离。

### Random

提供随机数生成相关的实用函数。

- `Guid2Seed(FGuid Guid, int64 Salt)`: 将 GUID 和盐值转换为种子。

### WorldUtils

提供用于世界操作的实用函数。

- `FindAllActors(UWorld* InWorld, TArray<T*>& OutActors)`: 查找世界中的所有指定类型的 Actor。
- `FindAllActorsByPredicate(UWorld* InWorld, TArray<T*>& OutActors, TFunction<bool(const T*)> Predicate)`: 根据谓词查找世界中的所有指定类型的 Actor。
- `MapActors(UWorld* InWorld, TMap<U, T*>& OutActorMap, TFunction<U(const T*)> MapFunction)`: 将世界中的 Actor 映射到映射中。

### ObjectUtils

提供用于对象操作的实用函数。

- `DispatchToObjectAndSubobjects(UObject* Obj, TFunction<void(UObject*)> Func, int32 Depth = 0)`: 将函数分派到对象及其子对象。