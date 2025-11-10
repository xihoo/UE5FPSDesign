# VoxelBoundsTests.cpp

## 概述

`VoxelBoundsTests.cpp` 是一个用于测试 `FVoxelBounds` 结构体功能的自动化测试文件。`FVoxelBounds` 是一个用于表示和操作3D体素（Voxel）空间中对象边界和连接信息的数据结构。它在地牢生成系统中扮演着核心角色，用于描述房间、走廊或其他结构在3D网格中的占用情况以及它们与相邻结构的连接方式（如门或墙）。该测试文件通过一系列单元测试，验证 `FVoxelBounds` 的扩展、比较、变换、连接、相减以及兼容性评分 (`GetCompatibilityScore`) 等核心操作的正确性。

## 代码解析

该文件使用 Unreal Engine 的 `IMPLEMENT_SIMPLE_AUTOMATION_TEST` 宏。

### 测试套件定义

```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVoxelBoundsTest, "ProceduralDungeon.Types.VoxelBounds", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::SmokeFilter)
```

- 定义了一个名为 `FVoxelBoundsTest` 的简单自动化测试类。
- 测试类别为 `"ProceduralDungeon.Types.VoxelBounds"`。

### 辅助宏

- `SET_CONNECTION(BOUNDS, CELL, DIR, TYPE)`: 简化设置体素单元格连接类型的代码。

### `RunTest` 函数

这是测试的主体函数，包含了多个独立的测试块。

#### 1. 扩展测试 (Extend Test)

- **目标**: 验证 `FVoxelBounds::AddCell` 方法能否正确地将新的体素单元格添加到边界中，并自动更新整体的包围盒 (`GetBounds`)。
- **过程**: 创建空的 `FVoxelBounds` 实例，添加几个分散的单元格，然后检查 `GetBounds()` 返回的 `FBoxMinAndMax` 是否正确包含了所有添加的点。

#### 2. 比较测试 (Comparison Test)

- **目标**: 验证 `FVoxelBounds` 的相等性 (`==`) 和不等性 (`!=`) 运算符能否正确比较两个 `FVoxelBounds` 实例。
- **过程**:
    1.  创建两个具有相同单元格和连接信息的 `FVoxelBounds` 实例 `BoundsA` 和 `BoundsB`，验证它们相等。
    2.  创建一个与 `BoundsA` 单元格相同但连接信息不同的 `BoundsC`，验证它们不相等。
    3.  创建一个空的 `FVoxelBounds` 实例 `BoundsD`，验证它无效 (`!IsValid()`) 且与 `BoundsA` 不相等。

#### 3. 变换测试 (Transform Test)

- **目标**: 验证 `FVoxelBounds` 的平移 (`operator+`, `operator-`) 和旋转 (`Rotate` 辅助函数) 操作能否正确地改变其在3D空间中的位置和朝向。
- **过程**:
    1.  创建一个基础的 `FVoxelBounds` 实例。
    2.  测试平移操作：`Bounds + Offset` 和 `Bounds - Offset`，并验证结果是否符合预期。
    3.  测试旋转操作：分别对基础实例进行北、东、南、西四个方向的旋转，并将结果与手动构建的预期 `FVoxelBounds` 进行比较。特别注意旋转时连接方向的同步变换。

#### 4. 连接测试 (Concatenation Test)

- **目标**: 验证 `FVoxelBounds` 的连接操作 (`operator+`) 能否正确地将两个 `FVoxelBounds` 实例合并。
- **过程**:
    1.  创建两个独立的 `FVoxelBounds` 实例 `BoundsA` 和 `BoundsB`。
    2.  执行 `BoundsA + BoundsB` 和 `BoundsB + BoundsA`，验证连接操作满足交换律（结果相等）。
    3.  将连接结果与手动构建的预期 `FVoxelBounds` 进行比较，确保所有单元格和连接信息都正确合并。

#### 5. 相减测试 (Subtraction Test)

- **目标**: 验证 `FVoxelBounds` 的相减操作 (`operator-`) 能否正确地从一个 `FVoxelBounds` 中移除另一个 `FVoxelBounds` 的内容。
- **过程**:
    1.  创建三个 `FVoxelBounds` 实例 `BoundsA`, `BoundsB`, `BoundsC`，其中 `BoundsC` 是 `BoundsA` 和 `BoundsB` 的并集。
    2.  执行 `BoundsC - BoundsA` 和 `BoundsC - BoundsB`，验证相减结果是否符合预期。
    3.  执行 `BoundsA - BoundsC` 和 `BoundsB - BoundsC`，验证当被减数是减数的子集时，相减结果无效 (`!IsValid()`)，因为无法从一个较小的集合中减去一个较大的集合。

#### 6. 外部适配测试 (DoesFitOutside Test)

- **目标**: 验证 `FVoxelBounds::GetCompatibilityScore` 方法能否正确评估一个 `FVoxelBounds` 是否能适配到另一个（通常是代表现有地牢结构的）`FVoxelBounds` 的外部空间中，并给出一个兼容性评分。
- **过程**:
    1.  **构建场景**: 创建一个复杂的 `DungeonBounds` 作为已存在的地牢空间。
    2.  **适配测试**:
        - 创建多个不同的 `FVoxelBounds` 实例 (`BoundsA` 到 `BoundsG`)，它们代表了不同形状、大小和连接方式的待放置结构。
        - 调用 `GetCompatibilityScore` 方法，验证哪些结构能够适配 (`TestTrue`)，哪些不能 (`TestFalse`)。
        - 对于能够适配的结构，比较它们的兼容性评分 (`Score`)，验证评分逻辑是否符合预期（例如，完全匹配的结构得分更高，有门对墙的结构得分较低等）。
    3.  **不适应测试**:
        - 创建一些明显无法适配的 `FVoxelBounds` 实例。
        - 调用 `GetCompatibilityScore` 方法，验证返回值为 `false`。
    4.  **自定义评分测试**:
        - 创建自定义的评分回调函数 (`UCustomScoreCallback`)。
        - 使用这些回调函数调用 `GetCompatibilityScore`，验证系统能够正确处理外部提供的评分逻辑（如总是返回0分或总是失败）。

### 辅助宏清理

文件末尾使用 `#undef` 清理了之前定义的辅助宏。

## 关键概念

- **体素表示**: `FVoxelBounds` 提供了一种高效的方式来表示3D网格空间中的复杂形状及其边界连接信息。
- **空间操作**: 平移、旋转、连接、相减等操作是构建和修改地牢结构的基础工具。
- **兼容性评估**: `GetCompatibilityScore` 是地牢生成算法中的关键函数，用于在多个候选房间中选择最合适的一个进行放置，确保生成的地牢结构在空间和连接上都是合理的。
- **数据结构健壮性**: 通过全面的测试，确保 `FVoxelBounds` 在各种边界条件和复杂操作下都能保持数据的一致性和正确性。
- **自动化测试**: 作为底层核心数据结构，`FVoxelBounds` 的稳定性至关重要。详尽的单元测试为其上层应用（如地牢生成、碰撞检测）提供了可靠的保障。