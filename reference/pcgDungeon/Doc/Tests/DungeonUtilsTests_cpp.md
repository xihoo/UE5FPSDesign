# DungeonUtilsTests.cpp

## 概述

`DungeonUtilsTests.cpp` 是一个用于测试 `ProceduralDungeonUtils.h` 中定义的通用工具函数的自动化测试文件。该文件主要包含三组测试：加权映射 (`Weighted Maps`)、GUID 转随机种子 (`Guid2Seed`) 的基本功能测试，以及 `Guid2Seed` 的统计学测试。这些工具函数在地牢生成过程中扮演着重要角色，例如用于根据权重随机选择房间，或为不同实例生成确定性的随机数流。

## 代码解析

该文件使用了 Unreal Engine 的两种自动化测试方式：`IMPLEMENT_SIMPLE_AUTOMATION_TEST` 和 `BEGIN_DEFINE_SPEC` / `END_DEFINE_SPEC`。

### 加权映射测试 (FDungeonUtilsTest_WeightedMap)

#### 测试套件定义

```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDungeonUtilsTest_WeightedMap, "ProceduralDungeon.Utils.WeightedMaps", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::SmokeFilter)
```

#### `RunTest` 函数

- **内置类型测试 (Built-in types test)**:
    - 创建一个 `TMap<int, int>`，其中键是值，值是对应的权重。
    - 测试 `Dungeon::GetTotalWeight` 函数，验证它能正确计算所有非零权重的总和。
    - 测试 `Dungeon::GetWeightedAt` 函数，验证它能根据索引（基于累积权重）正确返回对应的键。测试覆盖了边界情况，如负索引、超出范围的索引以及权重为0的项。
- **指针类型测试 (Pointer test)**:
    - 创建一些整型变量和一个 `TMap<int*, int>`。
    - 类似地测试 `GetTotalWeight` 和 `GetWeightedAt` 函数，验证它们在处理指针键时的正确性，特别是跳过权重为0的项。

### GUID 转随机种子基本测试 (FDungeonUtilsTest_Guid2Seed)

#### 测试套件定义

```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDungeonUtilsTest_Guid2Seed, "ProceduralDungeon.Utils.Guid2Seed.Simple", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::SmokeFilter)
```

#### `RunTest` 函数

- 创建几对不同的 `FGuid` 和 `int64` 盐值。
- 通过一系列 `TestTrue` 断言，验证 `Random::Guid2Seed` 函数的行为：
    - 相同的 `Guid` 和盐值应产生相同的种子。
    - 相同的 `Guid` 但不同的盐值应产生不同的种子。
    - 不同的 `Guid` 但相同的盐值应产生不同的种子。
    - 不同的 `Guid` 和不同的盐值应产生不同的种子。
- 这确保了 `Guid2Seed` 函数能够提供足够的唯一性和可变性，这对于生成确定性随机数至关重要。

### GUID 转随机种子统计学测试 (FGuid2SeedStatisticalTests)

#### 测试规范定义

```cpp
BEGIN_DEFINE_SPEC(FGuid2SeedStatisticalTests, "ProceduralDungeon.Utils.Guid2Seed.Stats", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::EngineFilter)
...
END_DEFINE_SPEC(FGuid2SeedStatisticalTests)
```

#### 辅助结构和函数

- `FTestParams`: 定义测试参数，包括要测试的 `Guid`、元素数量、样本数量和卡方检验的临界值。
- `GenerateSamples`: 核心函数，使用 `Random::Guid2Seed` 生成大量随机样本。它模拟了在不同房间实例中使用相同逻辑生成随机数的场景（通过改变盐值）。
- `PassChiSquaredTest`: 执行卡方检验，这是一种统计学方法，用于检验观察到的频率分布与期望的均匀分布之间是否存在显著差异。

#### `Define` 函数

- 使用 BDD 风格的 `Describe` 和 `It` 宏来组织测试。
- 测试多个不同的 `FGuid`。
- 对每个 `Guid`，测试不同的元素数量（10, 100, 1000）。
- 对于每种情况，生成十亿个样本，并使用卡方检验来验证这些样本的分布是否足够接近均匀分布（p-value = 5%）。
- 如果检验通过，则认为 `Guid2Seed` 生成的种子具有良好的随机性。

## 关键概念

- **加权随机选择**: `GetWeightedAt` 提供了一种根据权重从集合中选择元素的机制，这在需要根据概率选择房间类型或布局时非常有用。
- **确定性随机数生成**: `Guid2Seed` 是实现确定性过程生成 (DPG) 的关键。通过将房间或 Actor 的 GUID 与实例 ID（作为盐值）结合，可以为每个实例生成一个唯一的、但可重现的随机数种子。这确保了同一房间在不同游戏会话中生成的内容是一致的。
- **统计学验证**: 仅通过功能测试无法完全保证随机数生成器的质量。卡方检验提供了一种量化的方法来评估生成的随机数序列是否具有所需的统计特性（如均匀性）。
- **自动化测试**: 通过多层次的测试（单元测试 + 统计测试），确保工具函数的正确性、鲁棒性和随机性，为整个地牢生成系统的可靠性奠定基础。