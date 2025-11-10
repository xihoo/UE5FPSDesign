# DoorDirectionTests.cpp

## 概述

`DoorDirectionTests.cpp` 是一个用于测试 `EDoorDirection` 枚举及其相关运算符功能的自动化测试文件。`EDoorDirection` 定义在 `ProceduralDungeonTypes.h` 中，用于表示门或房间连接的四个基本方向：北 (North)、东 (East)、南 (South)、西 (West)。该测试文件通过一系列单元测试用例，验证与 `EDoorDirection` 枚举相关的各种运算符（加法、减法、取反、按位取反、自增、自减、逻辑非）的行为是否符合预期。

## 代码解析

该文件使用 Unreal Engine 的自动化测试框架 (`Misc/AutomationTest.h`)。

### 测试套件定义

```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDoorDirectionTest, "ProceduralDungeon.Types.DoorDirection", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::SmokeFilter)
```

- 定义了一个名为 `FDoorDirectionTest` 的简单自动化测试类。
- 测试类别为 `"ProceduralDungeon.Types.DoorDirection"`。

### `RunTest` 函数

这是测试的主体函数，包含了所有针对 `EDoorDirection` 枚举运算符的测试用例。

#### 1. 方向加法测试 (Adding directions)

- 测试了 `EDoorDirection` 枚举的加法运算符 (`operator+`)。
- 验证了任意两个方向相加的结果是否符合预定义的规则。例如，`North + East = East`，`East + South = West` 等。这些规则定义了在一个方向上再施加另一个方向的旋转效果。

#### 2. 方向减法测试 (Subtracting directions)

- 测试了 `EDoorDirection` 枚举的减法运算符 (`operator-`)。
- 验证了任意两个方向相减的结果。例如，`North - East = West`，`East - South = West` 等。这可以理解为从一个方向“减去”另一个方向的旋转效果。

#### 3. 方向取反测试 (Negating directions)

- 测试了 `EDoorDirection` 枚举的一元负号运算符 (`operator-`)。
- 验证了对方向取反的结果。根据测试，`-North = North`，`-East = West`，`-South = South`，`-West = East`。这似乎定义了一种特定的“镜像”或“反转”操作。

#### 4. 方向对立测试 (Opposite directions)

- 测试了 `EDoorDirection` 枚举的按位取反运算符 (`operator~`)。
- 验证了获取对立方向的结果：`~North = South`，`~East = West`，`~South = North`，`~West = East`。这是最直观的获取相反方向的方法。

#### 5. 方向自增/自减测试 (Incrementing/decrementing directions)

- 测试了 `EDoorDirection` 枚举的前缀自增 (`++`) 和前缀自减 (`--`) 运算符。
- 验证了方向在循环中的递增和递减行为：`North -> East -> South -> West -> North` (递增) 和 `North -> West -> South -> East -> North` (递减)。

#### 6. 方向布尔测试 (Boolean testing directions)

- 测试了 `EDoorDirection` 枚举的逻辑非运算符 (`operator!`)。
- 验证了方向在布尔上下文中的表现。普通方向（North, East, South, West）的逻辑非结果为 `false`，而 `EDoorDirection::NbDirection`（通常用作枚举计数或无效值）的逻辑非结果为 `true`。

### 辅助宏

- `TestEqual`: 用于断言两个值是否相等。
- `TestTrue`: 用于断言一个布尔表达式是否为真。
- `TestFalse`: 用于断言一个布尔表达式是否为假。

## 关键概念

- **枚举运算符重载**: `EDoorDirection` 通过重载多种运算符，使其不仅仅是一个简单的枚举值，而具有了更丰富的语义和操作能力，如旋转、获取对立方向等。
- **方向代数**: 加法和减法运算符定义了一种方向之间的“代数”关系，可以用来计算复合旋转。
- **循环枚举**: 自增和自减运算符使得 `EDoorDirection` 可以方便地在预设的四个方向之间循环遍历。
- **对立与镜像**: 按位取反 (`~`) 和一元负号 (`-`) 提供了获取对立方向和镜像方向的不同方式。
- **自动化测试**: 通过详尽的测试用例确保这些自定义的枚举运算符行为稳定且符合设计意图，这对于依赖这些运算的地牢生成逻辑至关重要。