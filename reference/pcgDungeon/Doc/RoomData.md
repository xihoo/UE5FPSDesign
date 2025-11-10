# RoomData 类

## 概述

`URoomData` 类是多个房间实例共享的静态数据。此数据用于定义房间的属性和要生成的房间关卡。它继承自 `UPrimaryDataAsset`。

## 成员变量

- `Level` (TSoftObjectPtr<UWorld>): 房间的关卡。
- `RandomDoor` (bool): 这将在地牢生成期间强制选择一个随机门。**已弃用**：它将在插件的未来版本中删除。作为替代，你应该在 DungeonGenerator 的 ChooseNextRoomData 中返回 -1 作为 DoorIndex。
- `Doors` (TArray<FDoorDef>): 房间的门定义。
- `FirstPoint` (FIntVector): 房间的第一个点。
- `SecondPoint` (FIntVector): 房间的第二个点。
- `CustomData` (TSet<TSubclassOf<URoomCustomData>>): 房间的自定义数据。

在 URoomData 类中，FirstPoint 和 SecondPoint 这两个 FIntVector 类型的成员变量用于定义房间在逻辑网格（以房间单元为单位）中的大小和形状。

  它们的具体作用如下：

   1. 定义房间边界:
       * 这两个点共同确定了一个轴对齐的长方体边界框（AABB）。
       * 这个边界框描述了该房间模板在3D逻辑网格空间中所占据的区域。
       * 例如，如果 FirstPoint 是 (0, 0, 0) 而 SecondPoint 是 (3, 2, 1)，那么这个房间模板在逻辑上就占据了从 (0,0,0) 到 (3,2,1) 的空间，尺寸为 3x2x1 个房间单元。

   2. 计算房间大小和体积:
       * 插件内部会根据这两个点来计算房间的大小（GetSize()）和体积（GetVolume()）。
       * 通常计算方法是 Abs(SecondPoint - FirstPoint) 来得到各个维度的大小。

   3. 确定门的有效位置:
       * 门（FDoorDef）的位置（Position）是用 FIntVector 定义的，这个坐标必须位于由 FirstPoint 和 SecondPoint 定义的边界框的表面上，否则会被认为是无效的门。
       * 例如，在上面 3x2x1 的例子里，一个位于 (1, 0, 0) 的门是有效的（在X-Y平面的“地面”边上），而一个位于 (5, 1, 0) 的门就是无效的（超出了边界）。

   4. 碰撞和边界检查:
       * 在地牢生成过程中，当尝试放置一个新房间时，系统会检查这个新房间的边界（基于其 FirstPoint, SecondPoint
         和放置位置/方向）是否会与已放置的其他房间的边界重叠。这是通过 URoomData::IsRoomInBounds 和 URoom::Overlap 等函数实现的。

   5. 生成体素边界 (`FVoxelBounds`):
       * 这两个点是生成 URoomData::GetVoxelBounds() 的基础。FVoxelBounds 是一个更详细的结构，它不仅描述了房间占据的逻辑单元格，还描述了每个单元格面是墙还是门。FirstPoint
         和 SecondPoint 定义的区域内的所有单元格都会被包含在生成的 FVoxelBounds 中，并根据门的位置设置相应的连接信息。

   6. 编辑器中的可视化:
       * 在 Unreal Editor 的 "Room Editor Mode"
         中，这两个点定义了用于调整房间大小的控制点（通常是立方体的两个对角顶点），并且也会用来绘制房间的网格线框，帮助开发者直观地看到房间的范围。

  简单来说，FirstPoint 和 SecondPoint 就像是定义了一个房间“蓝图”的物理尺寸和形状，是地牢生成系统理解和摆放房间的基础数据。

## 成员函数

### 构造函数

- `URoomData()`: 构造函数。

### 蓝图纯函数

- `GetNbDoor()`: 获取门的数量。
- `GetDoorDef(int32 DoorIndex)`: 获取门定义。
- `HasCompatibleDoor(const FDoorDef& DoorData)`: 检查是否有兼容的门。
- `GetCompatibleDoors(const FDoorDef& DoorData, TArray<int>& CompatibleDoors)`: 获取兼容的门。
- `HasDoorOfType(UDoorType* DoorType)`: 检查是否有指定类型的门。
- `HasAnyDoorOfType(const TArray<UDoorType*>& DoorTypes)`: 检查是否有任何指定类型的门。
- `HasAllDoorOfType(const TArray<UDoorType*>& DoorTypes)`: 检查是否所有指定类型的门都存在。
- `HasCustomData(TSubclassOf<URoomCustomData> CustomDataClass)`: 检查是否有指定的自定义数据。
- `HasAnyCustomData(const TArray<TSubclassOf<URoomCustomData>>& CustomDataList)`: 检查是否有任何指定的自定义数据。
- `HasAllCustomData(const TArray<TSubclassOf<URoomCustomData>>& CustomDataList)`: 检查是否所有指定的自定义数据都存在。
- `GetSize()`: 获取房间大小。
- `GetVolume()`: 获取房间体积。

### 蓝图原生事件

- `InitializeRoom(URoom* Room, UDungeonGraph* Dungeon)`: 初始化房间。
- `CleanupRoom(URoom* Room, UDungeonGraph* Dungeon)`: 清理房间。

### 其他函数

- `GetBounds(FTransform Transform = FTransform::Identity)`: 获取边界。
- `GetIntBounds()`: 获取整数边界。
- `GetVoxelBounds()`: 获取体素边界。
- `IsRoomInBounds(const FBoxMinAndMax& Bounds, int DoorIndex, const FDoorDef& DoorDungeonPos)`: 检查房间是否在边界内。
- `IsDoorValid(int DoorIndex)`: 检查门是否有效（仅在非发布版本或编辑器中）。
- `IsDoorDuplicate(int DoorIndex)`: 检查门是否重复（仅在非发布版本或编辑器中）。

### 编辑器函数

- `IsDataValid(...)`: 检查数据是否有效。
- `PostEditChangeProperty(...)`: 属性更改后调用。
- `OnPropertiesChanged`: 属性更改事件。

## 私有成员

- `CachedVoxelBounds` (FVoxelBounds): 缓存的体素边界，用于避免不必要的计算。