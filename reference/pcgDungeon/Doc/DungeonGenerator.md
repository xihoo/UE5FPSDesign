# DungeonGenerator 类

## 概述

`ADungeonGenerator` 类是插件的主要 Actor。地牢生成器负责生成地牢并通过网络进行复制。它继承自 `ADungeonGeneratorBase`，并提供了生成地牢的核心逻辑和接口。

## 成员变量

- `GenerationType` (EGenerationType): 地牢生成房间的顺序。Depth First（深度优先）：地牢将使用最后生成的房间来放置下一个房间，生成更线性的地牢。Breadth First（广度优先）：地牢将在当前房间的每个门上生成一个房间，然后再转到下一个房间，生成更分散的地牢。
- `bCanLoop` (bool): 如果勾选，新放置的房间将检查是否有任何门与另一个房间对齐，如果有，则将它们连接起来。如果未勾选，则仅连接 ChooseNextRoom 函数中 CurrentRoom 和 NextRoom 之间的门。
- `DungeonLimits` (FBoundsParams): 地牢的边界限制。
- `bAutoDiscardRoomIfNull` (bool): 如果为 true，在 ChooseNextRoom 中返回 null 与调用 DiscardRoom 相同。
- `bDiscardRoom` (bool): 标志位，明确表示我们不想放置房间。

## 成员函数

### 构造函数

- `ADungeonGenerator()`: 构造函数。

### 虚函数重写

- `CreateDungeon_Implementation()`: 创建地牢的实现。

### 蓝图原生事件（需在蓝图中重写）

- `ChooseFirstRoomData()`: 返回你想要作为地牢生成根的 RoomData。
- `ChooseNextRoomData(const URoomData* CurrentRoom, const TScriptInterface<IReadOnlyRoom>& CurrentRoomInstance, const FDoorDef& DoorData, int& DoorIndex)`: 返回将连接到当前房间的 RoomData。
- `IsValidDungeon()`: 验证地牢生成的条件。
- `ContinueToAddRoom()`: 继续或停止向地牢添加房间的条件。

### 蓝图可调用函数（工具函数）

- `HasAlreadyRoomData(URoomData* RoomData)`: 如果地牢中已存在特定的 RoomData，则返回 true（已弃用）。
- `HasAlreadyOneRoomDataFrom(TArray<URoomData*> RoomDataList)`: 如果地牢中已存在提供的列表中的至少一个 RoomData，则返回 true（已弃用）。
- `CountRoomData(URoomData* RoomData)`: 返回地牢中特定 RoomData 的数量（已弃用）。
- `CountTotalRoomData(TArray<URoomData*> RoomDataList)`: 返回地牢中提供的列表中的 RoomData 的总数（已弃用）。
- `HasAlreadyRoomType(TSubclassOf<URoomData> RoomType)`: 如果地牢中已存在特定的 RoomData 类型，则返回 true（已弃用）。
- `HasAlreadyOneRoomTypeFrom(TArray<TSubclassOf<URoomData>> RoomTypeList)`: 如果地牢中已存在提供的列表中的至少一个 RoomData 类型，则返回 true（已弃用）。
- `CountRoomType(TSubclassOf<URoomData> RoomType)`: 返回地牢中特定 RoomData 类型的数量（已弃用）。
- `CountTotalRoomType(TArray<TSubclassOf<URoomData>> RoomTypeList)`: 返回地牢中提供的列表中的 RoomData 类型的总数（已弃用）。
- `GetNbRoom()`: 返回生成的地牢中的房间总数（已弃用）。
- `DiscardRoom()`: 必须在 "Choose Next Room" 函数中调用才能使用。明确告诉生成器我们不想为特定门放置房间。

### 私有函数

- `AddNewRooms(URoom& ParentRoom, TArray<URoom*>& AddedRooms)`: 将一些与 ParentRoom 链接的新房间添加到 Rooms 列表输出中。AddedRooms 仅包含添加到 Rooms 列表中的新房间。如果地牢应继续添加新房间，则返回 true。

核心思想:
  ADungeonGenerator 通过一个循环来不断尝试生成房间，直到满足用户定义的“有效地牢”条件 (IsValidDungeon)
  或者达到最大尝试次数。它使用一个辅助数据结构（栈或队列）来管理待处理的房间，决定下一个要处理哪个房间以及从哪个门开始生成新房间。

  详细流程:

   1. 初始化与权限检查:
       * 首先检查是否在服务器上运行（因为地牢生成和复制是由服务器控制的）。
       * 调用 StartNewDungeon() 重置生成器状态，清空上一次生成的数据（如 UDungeonGraph）。

   2. 生成循环:
       * 进入一个大的循环，这个循环会重复执行，直到生成的地牢被 IsValidDungeon 判定为有效，或者达到 UProceduralDungeonSettings 中设置的 MaxGenerationTry 最大尝试次数。
       * 在每次循环开始时（除了第一次），会调用 StartNewDungeon() 来清空上一次不成功的尝试。

   3. 选择数据结构 (栈 or 队列):
       * 根据 ADungeonGenerator::GenerationType 的设置来决定使用哪种数据结构：
           * DFS (Depth First Search): 使用栈 (Stack)。这会使得生成器倾向于沿着一条路径深入生成，创建更线性、更长的走廊。
           * BFS (Breadth First Search): 使用队列 (Queue)。这会使得生成器先填充当前房间的所有相邻位置，再移动到下一批房间，创建更宽广、更分散的结构。

   4. 生成第一个房间:
       * 调用蓝图可重写事件 ChooseFirstRoomData() 来决定使用哪个 URoomData 作为地牢的起点。
       * 使用这个 URoomData 调用 CreateRoomInstance() 创建第一个 URoom 实例。
       * 将这个初始房间添加到 UDungeonGraph 中。
       * 将这个初始房间（的指针）压入（栈）或推入（队列）RoomContainer。

   5. 主生成循环 (处理待处理房间):
       * 进入一个内部循环，只要 RoomContainer（栈或队列）不为空，就继续执行。
       * 取出待处理房间:
           * 从 RoomContainer 中弹出（栈）或取出（队列）一个 URoom 指针，作为当前要处理的 ParentRoom。
       * 为当前房间添加新邻居 (`AddNewRooms`):
           * 调用 AddNewRooms(ParentRoom, AddedRooms) 函数。这是生成新房间的关键步骤：
               * 遍历 ParentRoom 对应的 URoomData 中定义的每一个门 (DoorDef)。
               * 对于每一个门：
                  a.  选择下一个房间数据: 调用蓝图可重写事件 ChooseNextRoomData(CurrentRoomData, CurrentRoomInstance, DoorData, DoorIndex)。用户在这里决定在当前门连接哪个
  URoomData。如果返回 nullptr 且 bAutoDiscardRoomIfNull 为 true，或者用户在 ChooseNextRoomData 中调用了 DiscardRoom()，则跳过这个门，不生成新房间。
                  b.  检查兼容性: 检查选中的 URoomData 是否与当前门兼容（类型匹配等）。
                  c.  计算新房间位置: 根据 ParentRoom 的位置、方向以及新房间的门的位置和方向，计算出新房间在世界（逻辑网格）中的正确 Position 和 Direction。
                  d.  边界检查: 检查新房间放置后是否会超出 DungeonLimits 定义的边界。
                  e.  碰撞检查: 调用 TryPlaceRoom() 尝试放置新房间。这个函数会检查新房间的边界是否会与图中已有的任何房间重叠。如果启用了
  bUseWorldCollisionChecks，还会检查是否与持久关卡中的静态网格体重叠。
                  f.  添加到地牢: 如果放置成功，调用 AddRoomToDungeon() 将新房间实例添加到 UDungeonGraph 中，并建立与 ParentRoom 的连接 (URoomConnection)。
                  g.  加入待处理队列: 将新添加的房间指针压入（栈）或推入（队列）RoomContainer，以便后续处理它的门。
                  h.  房间数量限制: 检查是否达到了 UProceduralDungeonSettings::RoomLimit 的限制，如果达到则停止生成。
       * 检查是否继续: 调用蓝图可重写事件 ContinueToAddRoom()。如果返回 false，则跳出内部循环，停止添加新房间。

   6. 最终化地牢:
       * 当内部循环结束（待处理房间列表为空或 ContinueToAddRoom 返回 false）后，调用 FinalizeDungeon()。
       * FinalizeDungeon 会调用 UDungeonGraph::InitRooms() 来完成所有房间连接的最终设置（例如为未连接的门选择默认门类），然后调用蓝图可重写事件
         InitializeDungeon()，允许用户对整个已生成但尚未加载关卡的地牢图进行最后的初始化操作。

   7. 验证地牢:
       * 调用蓝图可重写事件 IsValidDungeon() 来检查刚刚生成的地牢是否满足用户定义的所有条件。
       * 如果返回 true，则整个生成循环结束，地牢生成成功。
       * 如果返回 false，则整个大循环会继续，清空当前不成功的地牢，重新开始下一次尝试，直到成功或达到最大尝试次数。

  总结:
  ADungeonGenerator 的流程是一个“生长”过程。它从一个种子房间开始，利用栈（DFS）或队列（BFS）来管理生长点，通过反复调用用户可定制的蓝图事件（ChooseFirstRoomData,
  ChooseNextRoomData, IsValidDungeon,
  ContinueToAddRoom）来决定“长什么”、“怎么长”以及“长成什么样才算好”。这个设计使得开发者可以通过蓝图脚本灵活地控制地牢的结构、主题和复杂度。