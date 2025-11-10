# VoxelBounds/VoxelBounds.cpp

## 概述

`VoxelBounds/VoxelBounds.cpp` 文件是 `FVoxelBounds` 结构体的实现文件。`FVoxelBounds` 是一个用于表示3D体素空间中对象边界和连接信息的数据结构，在地牢生成系统中扮演着核心角色。该文件实现了 `FVoxelBounds` 及其相关结构体 `FVoxelBoundsConnection` 的所有核心逻辑，包括单元格管理、连接信息处理、兼容性评分、空间变换（平移、旋转、合并、相减）以及比较操作。

## 代码解析

### `FVoxelBounds::Directions` 数组

```cpp
const FIntVector FVoxelBounds::Directions[] = {
	{1, 0, 0},	// North
	{0, 1, 0},	// East
	{-1, 0, 0}, // South
	{0, -1, 0}, // West
	{0, 0, 1},	// Up
	{0, 0, -1}	// Down
};
```

- **功能**: 定义了六个基本方向的向量偏移量，用于在3D网格中导航到相邻的单元格。
- **关键点**: 这是一个静态常量数组，为整个类提供方向参考。

### `FVoxelBoundsConnection` 结构体

#### `operator==` 运算符

```cpp
bool FVoxelBoundsConnection::operator==(const FVoxelBoundsConnection& Other) const
```

- **功能**: 比较两个 `FVoxelBoundsConnection` 实例是否相等。
- **逻辑**:
    1.  首先比较 `Type`。
    2.  如果类型都是 `Door`，则进一步比较 `DoorType` 指针。
    3.  其他情况（`Wall` 或 `None`）只要类型相同即认为相等。

#### `GetCompatibilityScore` 静态函数

```cpp
int32 FVoxelBoundsConnection::GetCompatibilityScore(const FVoxelBoundsConnection& A, const FVoxelBoundsConnection& B)
```

- **功能**: 计算两个连接信息之间的兼容性评分。
- **逻辑**:
    - `None` 与任何类型都兼容（评分为0）。
    - 类型不匹配时，如果其中一个是 `Door`，则给予较大负分（-10），否则评分为0。
    - 类型匹配且都是 `Door` 时，如果 `DoorType` 也匹配，则给予高分（10）；否则给予负分（-10）。
    - 类型匹配且都是 `Wall` 时，评分为0。
- **关键点**: 这是评估两个 `FVoxelBounds` 能否正确拼接的基础。

### `FVoxelBounds` 结构体

#### `Opposite` 静态函数

```cpp
FVoxelBounds::EDirection FVoxelBounds::Opposite(EDirection Direction)
```

- **功能**: 获取给定方向的反方向。
- **逻辑**: 使用一个预定义的查找表来快速获取反方向。

#### `AddCell` 函数

```cpp
TArray<FVoxelBoundsConnection>& FVoxelBounds::AddCell(FIntVector Cell)
```

- **功能**: 向 `FVoxelBounds` 中添加一个新的体素单元格。
- **逻辑**:
    1.  在 `Cells` 映射中为该单元格添加一个条目，并初始化其连接数组（6个方向）。
    2.  更新整体的包围盒 `Bounds`。
    3.  返回该单元格的连接数组引用，以便调用者设置连接信息。

#### `GetCellConnection` 和 `SetCellConnection` 函数

- **功能**: 获取和设置特定单元格在特定方向上的连接信息。
- **逻辑**: 通过 `Cells` 映射查找单元格，然后访问其连接数组。

#### `GetCompatibilityScore` 函数

```cpp
bool FVoxelBounds::GetCompatibilityScore(const FVoxelBounds& Other, int32& Score, const FScoreCallback& CustomScore) const
```

- **功能**: 评估当前 `FVoxelBounds` 与另一个 `FVoxelBounds` 的兼容性，并计算一个评分。
- **参数**:
    - `Other`: 要比较的另一个 `FVoxelBounds`。
    - `Score`: 输出参数，用于返回计算出的兼容性评分。
    - `CustomScore`: 可选的自定义评分回调函数。
- **逻辑**:
    1.  初始化评分为当前 `FVoxelBounds` 包含的单元格数量。
    2.  检查两个 `FVoxelBounds` 的包围盒是否重叠（包括边界接触）。
    3.  如果不重叠，直接返回 `true`（兼容）和单元格数量作为评分。
    4.  如果重叠，遍历当前 `FVoxelBounds` 的每个单元格：
        -   如果该单元格在 `Other` 中也存在，则不兼容（返回 `false`）。
        -   否则，检查该单元格的每个方向上的邻居是否在 `Other` 中存在。如果存在，则计算这两个相邻面的连接信息的兼容性评分，并累加到总分。
        -   如果提供了 `CustomScore` 回调，则调用它来计算评分。
    5.  返回 `true`（兼容）和最终计算出的评分。
- **关键点**: 这是地牢生成中选择和放置房间的关键算法。

#### 空间变换运算符 (`operator+`, `operator-`, `operator+=`, `operator-=`)

- **功能**: 实现 `FVoxelBounds` 的平移操作。
- **逻辑**: 对所有单元格的坐标进行加减操作，并相应地平移包围盒。

#### 合并与相减运算符 (`operator+=`, `operator-=`, `operator+`, `operator-`)

- **功能**: 实现 `FVoxelBounds` 之间的合并（连接）和相减操作。
- **`operator+=` 逻辑**:
    1.  遍历要合并的 `Other` 的所有单元格。
    2.  如果当前 `FVoxelBounds` 中已存在该单元格，则跳过。
    3.  否则，添加该单元格。
    4.  检查新添加单元格的邻居是否已在当前 `FVoxelBounds` 中。如果是，则清除邻居单元格指向新单元格的连接（因为它们现在是内部连接了）。
    5.  如果没有邻居，则从 `Other` 复制连接信息。
- **`operator-=` 逻辑**:
    1.  遍历要减去的 `Other` 的所有单元格。
    2.  从当前 `FVoxelBounds` 中移除该单元格。
    3.  检查被移除单元格的邻居是否仍存在于当前 `FVoxelBounds` 中。如果是，则将被移除单元格在该方向上的连接信息复制到邻居单元格上（修复邻居的连接）。

#### `operator==` 运算符

```cpp
bool FVoxelBounds::operator==(const FVoxelBounds& Other) const
```

- **功能**: 比较两个 `FVoxelBounds` 实例是否完全相等。
- **逻辑**: 比较单元格数量、每个单元格的坐标以及每个单元格所有六个方向的连接信息。

#### `Overlap` 静态函数

```cpp
bool FVoxelBounds::Overlap(const FVoxelBounds& A, const FVoxelBounds& B)
```

- **功能**: 检查两个 `FVoxelBounds` 是否在体素级别上重叠。
- **逻辑**:
    1.  首先检查它们的包围盒是否重叠。
    2.  如果包围盒重叠，则进一步检查是否有任何单元格坐标是相同的。

#### `Rotate` 全局函数

```cpp
FVoxelBounds Rotate(const FVoxelBounds& Bounds, const EDoorDirection& Rot)
```

- **功能**: 对一个 `FVoxelBounds` 进行旋转。
- **逻辑**:
    1.  遍历 `Bounds` 的所有单元格。
    2.  使用 `Rotate(FIntVector, EDoorDirection)` 函数计算旋转后的新坐标。
    3.  将新单元格添加到 `NewBounds` 中。
    4.  旋转连接信息：将原连接信息数组中的方向索引进行循环移位，以匹配新的朝向。注意，代码注释指出当前未处理上下（Up/Down）方向的旋转，这在将来支持上下开门时需要更新。

## 关键概念

- **体素表示**: 使用稀疏的 `TMap<FIntVector, TArray<FVoxelBoundsConnection>>` 来高效地表示不规则的3D形状。
- **连接信息**: 每个单元格的每个面都存储一个 `FVoxelBoundsConnection`，描述该面是墙、门还是空。
- **兼容性评估**: 通过比较相邻单元格的连接信息来判断两个 `FVoxelBounds` 能否无缝拼接。
- **空间操作**: 提供了完整的平移、旋转、合并、相减等操作，是构建复杂地牢结构的基础。
- **数据一致性**: 在合并和相减操作中，自动维护邻居单元格连接信息的正确性。