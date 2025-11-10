# DungeonGenerator.cpp

## 概述

`DungeonGenerator.cpp` 文件包含了 `ADungeonGenerator` 类的实现。这个类是插件的主要 Actor，负责生成地牢并通过网络进行复制。它实现了地牢生成的核心逻辑。

## 函数实现

### ADungeonGenerator()

```cpp
ADungeonGenerator::ADungeonGenerator()
```

构造函数。初始化地牢生成器的基本属性：

- 启用 Tick。
- 设置默认的生成类型为深度优先搜索 (DFS)。

### CreateDungeon_Implementation()

```cpp
bool ADungeonGenerator::CreateDungeon_Implementation()
```

创建地牢的实现。这是地牢生成的核心函数：

1. **权限检查**: 只有服务器才能生成地牢，地牢图将被复制到所有客户端。
2. **生成循环**: 循环生成地牢，直到 `IsValidDungeon` 返回 `true` 或达到最大尝试次数。
3. **初始化**: 调用 `StartNewDungeon` 重置生成数据。
4. **选择模式**: 根据 `GenerationType` 选择使用栈 (DFS) 或队列 (BFS)。
5. **创建第一个房间**: 调用 `ChooseFirstRoomData` 选择第一个房间数据，然后创建并添加第一个房间。
6. **构建房间列表**: 使用栈或队列来管理房间，循环调用 `AddNewRooms` 添加新房间。
7. **最终化**: 调用 `FinalizeDungeon` 初始化地牢。
8. **验证**: 调用 `IsValidDungeon` 验证生成的地牢是否有效。

### AddNewRooms()

```cpp
bool ADungeonGenerator::AddNewRooms(URoom& ParentRoom, TArray<URoom*>& AddedRooms)
```

向父房间添加新房间。这是生成地牢的关键步骤：

1. **检查权限**: 确保只有服务器可以调用此函数。
2. **检查门**: 确保父房间至少有一个门。
3. **循环处理门**: 遍历父房间的每个门，尝试在该门处添加新房间。
4. **选择下一个房间**: 调用 `ChooseNextRoomData` 选择下一个房间数据。
5. **检查兼容性**: 确保选择的房间数据与门兼容，并且可以放入地牢边界内。
6. **放置房间**: 调用 `TryPlaceRoom` 尝试放置新房间。
7. **添加房间到地牢**: 调用 `AddRoomToDungeon` 将新房间添加到地牢图中。
8. **检查房间限制**: 检查是否达到房间数量限制。

### ChooseFirstRoomData_Implementation()

```cpp
URoomData* ADungeonGenerator::ChooseFirstRoomData_Implementation()
```

选择第一个房间数据的默认实现。这是一个蓝图原生事件，需要在蓝图中重写。

### ChooseNextRoomData_Implementation()

```cpp
URoomData* ADungeonGenerator::ChooseNextRoomData_Implementation(const URoomData* CurrentRoom, const TScriptInterface<IReadOnlyRoom>& CurrentRoomInstance, const FDoorDef& DoorData, int& DoorIndex)
```

选择下一个房间数据的默认实现。这是一个蓝图原生事件，需要在蓝图中重写。

### IsValidDungeon_Implementation()

```cpp
bool ADungeonGenerator::IsValidDungeon_Implementation()
```

验证地牢是否有效的默认实现。这是一个蓝图原生事件，需要在蓝图中重写。

### ContinueToAddRoom_Implementation()

```cpp
bool ADungeonGenerator::ContinueToAddRoom_Implementation()
```

决定是否继续添加房间的默认实现。这是一个蓝图原生事件，需要在蓝图中重写。

### 工具函数 (已弃用)

以下函数是用于访问地牢图信息的工具函数，但已被标记为弃用，建议直接使用 `Graph` 变量：

- `HasAlreadyRoomData()`
- `HasAlreadyOneRoomDataFrom()`
- `CountRoomData()`
- `CountTotalRoomData()`
- `HasAlreadyRoomType()`
- `HasAlreadyOneRoomTypeFrom()`
- `CountRoomType()`
- `CountTotalRoomType()`
- `GetNbRoom()`