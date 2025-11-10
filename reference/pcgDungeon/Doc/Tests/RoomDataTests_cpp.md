# RoomDataTests.cpp

## 概述

`RoomDataTests.cpp` 是一个用于测试 `URoomData` 类核心功能的自动化测试文件。`URoomData` 是地牢系统中表示房间模板的数据资产，定义了房间的大小、门的位置和类型等关键信息。该测试文件通过一系列详尽的测试用例，验证 `URoomData` 的边界检查 (`IsRoomInBounds`)、门类型查询 (`HasDoorOfType` 及其变体)、自定义数据查询 (`HasCustomData` 及其变体)、尺寸和体积计算 (`GetSize`, `GetVolume`) 以及体素边界转换 (`GetVoxelBounds`) 等方法的正确性。

## 代码解析

该文件使用 Unreal Engine 的 `IMPLEMENT_SIMPLE_AUTOMATION_TEST` 宏。

### 测试套件定义

```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRoomDataTests, "ProceduralDungeon.Types.RoomData", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::EngineFilter)
```

- 定义了一个名为 `FRoomDataTests` 的简单自动化测试类。
- 测试类别为 `"ProceduralDungeon.Types.RoomData"`。
- 使用 `EAutomationTestFlags::EngineFilter`。

### 辅助宏

- `CREATE_ROOM_DATA(Data)`: 创建一个 `URoomData` 数据资产并清空其门列表。
- `ADD_DOOR(ROOM, DOOR_POS, DOOR_DIR, DOOR_TYPE)`: 向指定的 `URoomData` 添加一个门定义。

### `RunTest` 函数

这是测试的主体函数，包含了多个独立的测试块。

#### 1. 边界检查测试 (IsRoomInBounds Test)

- **目标**: 验证 `URoomData::IsRoomInBounds` 方法能否正确判断一个房间在给定方向和位置旋转后，是否会超出指定的地牢边界。
- **过程**:
    1.  创建一个 `URoomData` 实例，定义其 `FirstPoint` 和 `SecondPoint`，从而确定房间的逻辑大小和形状。
    2.  为该房间数据添加一个门。
    3.  定义一个 `DungeonBounds`（地牢的总边界）。
    4.  计算房间相对于其门位置的边界 (`RoomBoundsAtDoorLocation`)。
    5.  分别模拟房间朝北、东、南、西四个方向放置的情况。
    6.  对于每种方向，计算旋转后的房间边界，并与预期值进行比较。
    7.  通过移动门的位置（从而移动整个房间），测试房间在边界内部、与边界相切、与边界相交以及在边界外部等各种情况下的返回值。
- **验证点**:
    - 旋转后房间边界计算的准确性。
    - `IsRoomInBounds` 方法在不同相对位置下判断的正确性，特别是处理边界接触的情况。

#### 2. 门类型查询测试 (HasDoorOfType and variants Test)

- **目标**: 验证 `URoomData::HasDoorOfType`, `HasAnyDoorOfType`, `HasAllDoorOfType` 方法能否正确查询房间数据中门的类型。
- **过程**:
    1.  创建一个 `URoomData` 实例和多个 `UDoorType` 数据资产。
    2.  向 `URoomData` 添加多个具有不同类型的门。
    3.  分别调用 `HasDoorOfType` (检查是否存在指定类型的门)、`HasAnyDoorOfType` (检查是否存在列表中任意类型的门)、`HasAllDoorOfType` (检查是否包含列表中所有类型的门) 方法。
    4.  使用包含、不包含、空列表、包含空指针等多种输入进行测试。
- **验证点**:
    - 各种门类型查询方法在不同输入条件下的返回值是否正确。

#### 3. 自定义数据查询测试 (HasCustomData and variants Test)

- **目标**: 验证 `URoomData::HasCustomData`, `HasAnyCustomData`, `HasAllCustomData` 方法能否正确查询房间数据中关联的自定义数据类。
- **过程**:
    1.  创建一个 `URoomData` 实例。
    2.  向其 `CustomData` 数组中添加几个 `UClass` 指针（代表不同的自定义数据类）。
    3.  类似于门类型查询，测试 `HasCustomData`, `HasAnyCustomData`, `HasAllCustomData` 方法。
- **验证点**:
    - 各种自定义数据查询方法的正确性。

#### 4. 尺寸和体积测试 (Size and Volume Test)

- **目标**: 验证 `URoomData::GetSize` 和 `URoomData::GetVolume` 方法能否根据 `FirstPoint` 和 `SecondPoint` 正确计算房间的尺寸和体积。
- **过程**:
    1.  创建多个 `URoomData` 实例，分别设置不同的 `FirstPoint` 和 `SecondPoint`。
    2.  调用 `GetSize` 和 `GetVolume` 方法。
    3.  将结果与手动计算的预期值进行比较。
- **验证点**:
    - 尺寸和体积计算的准确性，包括处理坐标点顺序颠倒的情况。

#### 5. 体素边界转换测试 (GetVoxelBounds Test)

- **目标**: 验证 `URoomData::GetVoxelBounds` 方法能否正确地将房间数据（大小、门）转换为 `FVoxelBounds` 对象。
- **过程**:
    1.  创建多个 `URoomData` 实例，配置不同的尺寸和门。
    2.  调用 `GetVoxelBounds` 方法。
    3.  将返回的 `FVoxelBounds` 对象与手动构建的预期对象进行逐单元格、逐面连接类型的比较。
- **验证点**:
    - 生成的 `FVoxelBounds` 是否准确反映了房间的占用空间和门的位置。

### 辅助宏清理

文件末尾使用 `#undef` 清理了之前定义的辅助宏。

## 关键概念

- **房间模板验证**: `URoomData` 作为地牢生成的基础模板，其属性的正确性直接影响生成质量。测试确保了模板数据的合法性。
- **空间关系判断**: `IsRoomInBounds` 是防止房间生成越界的关键检查，测试覆盖了各种空间相对关系。
- **数据查询**: `Has*` 系列方法提供了灵活的查询能力，测试确保了这些查询逻辑的健壮性。
- **几何计算**: `GetSize`, `GetVolume`, `GetVoxelBounds` 涉及到基础的3D几何计算和数据结构转换，测试保证了计算结果的准确性。
- **自动化测试**: 通过全面的单元测试，确保 `URoomData` 类在各种使用场景下都能按预期工作，为上层的地牢生成逻辑提供可靠的基础。