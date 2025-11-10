# DungeonGraphTests.cpp

## 概述

`DungeonGraphTests.cpp` 是一个用于测试 `UDungeonGraph` 类核心功能的自动化测试文件。`UDungeonGraph` 是地牢生成系统中用于表示和管理已生成房间及其连接关系的图数据结构。该测试文件通过一系列复杂的测试用例，验证 `UDungeonGraph` 的房间管理、路径查找 (`FindPath`)、体素边界转换 (`GetVoxelBounds`) 以及房间候选过滤与排序 (`FilterAndSortRooms`) 等关键功能的正确性。

## 代码解析

该文件使用 Unreal Engine 的自动化测试框架 (`Misc/AutomationTest.h`)。

### 测试套件定义

```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDungeonGraphTest, "ProceduralDungeon.Types.DungeonGraph", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::EngineFilter)
```

- 定义了一个名为 `FDungeonGraphTest` 的简单自动化测试类。
- 测试类别为 `"ProceduralDungeon.Types.DungeonGraph"`。
- 使用了 `EAutomationTestFlags::EngineFilter`，表明这是一个更深入的引擎级别测试。

### 辅助宏

文件开头定义了几个用于简化测试代码的宏：

- `INIT_TEST(Graph)`: 创建一个新的 `UDungeonGraph` 对象。
- `CLEAN_TEST()`: 清空图中的所有房间。
- `CREATE_ROOM(Name, RoomDataPtr)`: 创建并初始化一个 `URoom` 对象，并将其添加到图中。
- `CREATE_ROOM_DATA(Data)`: 创建一个 `URoomData` 数据资产，并清空其门列表。
- `DUMMY_PATH(Path)`: 创建一个非空的路径数组（用于测试路径查找失败的情况）。

### `RunTest` 函数

这是测试的主体函数，包含了三个主要的测试块。

#### 1. 路径查找测试 (Pathfind Test)

- **目标**: 验证 `UDungeonGraph::FindPath` 静态方法的正确性。
- **过程**:
    1.  创建多个 `URoomData` 数据资产 (`DA_A` 到 `DA_D`) 并配置它们的门。
    2.  使用 `INIT_TEST` 创建一个 `UDungeonGraph` 实例 `Graph`。
    3.  使用 `CREATE_ROOM` 创建11个 `URoom` 实例 (`Room0` 到 `Room10`) 并添加到 `Graph` 中。
    4.  手动锁定某些房间 (`Room3`, `Room7`)。
    5.  使用 `Graph->Connect` 方法手动建立房间之间的连接，构建一个预设的复杂图结构。
    6.  执行一系列 `UDungeonGraph::FindPath` 调用，测试各种场景：
        - 两个空指针房间。
        - 一个空指针房间和一个有效房间。
        - 起点和终点为同一房间（包括锁定房间）。
        - 起点或终点为锁定房间，或路径必须经过锁定房间。
        - 正常的、存在路径的房间对，并验证返回的路径数组 `Path` 中房间的顺序和内容是否正确。
        - 测试 `IgnoreLocked` 参数，允许路径穿过被锁定的房间。
- **验证点**:
    - `FindPath` 方法在各种边界条件和异常情况下的返回值（`true`/`false`）。
    - 返回的 `Path` 数组在成功时包含正确的房间序列，在失败时为空。
    - 锁定房间对路径查找的阻断作用。

#### 2. 体素边界转换测试 (Voxel Bounds Conversions Test)

- **目标**: 验证 `URoom::GetVoxelBounds` 方法能否正确地将房间的逻辑信息（位置、方向、门）转换为 `FVoxelBounds` 对象，该对象描述了房间在体素空间中的占用和连接情况。
- **过程**:
    1.  创建一个新的 `UDungeonGraph` 实例 `Graph`。
    2.  创建并配置 `URoomData` (`DA_A` 到 `DA_C`, `DA_D`)。
    3.  创建6个 `URoom` 实例 (`Room0` 到 `Room5`)，并手动设置它们的位置和方向，构建一个简单的2层地牢结构。
    4.  调用 `Graph->RebuildBounds()` 以根据手动设置的位置重建图的边界信息。
    5.  对每个 `Room` 调用 `GetVoxelBounds()`，并将其结果与手动构建的预期 `FVoxelBounds` 对象进行比较。
- **验证点**:
    - 每个房间生成的 `FVoxelBounds` 对象是否准确反映了其在3D网格中的占用单元格。
    - 每个单元格的六个面（上下东西南北）的连接类型（墙 `Wall` 或门 `Door`）是否设置正确。

#### 3. 房间过滤与排序测试 (FilterAndSort Test)

- **目标**: 验证 `UDungeonGraph::FilterAndSortRooms` 方法能否根据给定的门定义 (`FDoorDef`)，从一个房间数据列表中筛选出兼容的房间，并按某种（未明确说明的）标准进行排序。
- **过程**:
    1.  创建一个新的 `UDungeonGraph` 实例 `Graph`。
    2.  创建并配置 `URoomData` (`DA_A`, `DA_B`, `DA_C`, `DA_D`, `DA_E`)。
    3.  创建6个 `URoom` 实例并设置位置，构建地牢结构。
    4.  调用 `Graph->RebuildBounds()`。
    5.  定义一个待筛选的房间数据列表 `RoomList`。
    6.  定义一个起始门 `FromDoor`。
    7.  调用 `Graph->FilterAndSortRooms`，传入 `RoomList` 和 `FromDoor`，输出到 `SortedRooms` 数组。
    8.  验证 `SortedRooms` 数组是否包含预期数量的候选项，并检查排序是否符合预期（例如，第一个候选项是否是最佳匹配）。
- **验证点**:
    - `FilterAndSortRooms` 方法能否正确识别与 `FromDoor` 兼容的房间数据及其门索引。
    - 返回的 `FRoomCandidate` 数组是否按预期排序。
    - 方法在有候选和无候选情况下的返回布尔值是否正确。

### 辅助宏清理

文件末尾使用 `#undef` 清理了之前定义的辅助宏。

## 关键概念

- **图数据结构测试**: `UDungeonGraph` 作为核心数据结构，其正确性至关重要。测试覆盖了图的构建、查询和修改。
- **路径查找算法**: 验证了地牢中两点间路径查找功能的鲁棒性，特别是对锁定房间的处理。
- **空间数据转换**: `GetVoxelBounds` 测试确保了逻辑房间信息能准确转换为用于碰撞检测、可视化或进一步处理的体素表示。
- **生成逻辑验证**: `FilterAndSortRooms` 测试是生成算法的一部分，确保系统能根据连接规则正确选择和排序下一个要生成的房间。
- **复杂场景模拟**: 通过构建预设的复杂房间连接图和地牢布局，测试了系统在真实使用场景下的表现。