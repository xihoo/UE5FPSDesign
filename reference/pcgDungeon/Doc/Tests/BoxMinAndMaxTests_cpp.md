# BoxMinAndMaxTests.cpp

## 概述

`BoxMinAndMaxTests.cpp` 是一个用于测试 `FBoxMinAndMax` 结构体功能的自动化测试文件。`FBoxMinAndMax` 定义在 `ProceduralDungeonTypes.h` 中，用于表示一个由最小整数坐标 (`Min`) 和最大整数坐标 (`Max`) 定义的 3D 轴对齐包围盒（AABB）。该测试文件通过一系列单元测试用例，验证 `FBoxMinAndMax` 的构造、大小计算、重叠判断、旋转、扩展以及点/盒包含判断等功能的正确性。

## 代码解析

该文件使用 Unreal Engine 的自动化测试框架 (`Misc/AutomationTest.h`) 来组织和执行测试。

### 测试套件定义

```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxMinAndMaxTest, "ProceduralDungeon.Types.BoxMinAndMax", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::SmokeFilter)
```

- 定义了一个名为 `FBoxMinAndMaxTest` 的简单自动化测试类。
- 测试类别为 `"ProceduralDungeon.Types.BoxMinAndMax"`。
- `EAutomationTestFlags::SmokeFilter` 表示这是一个冒烟测试，通常用于快速验证基本功能。

### `RunTest` 函数

这是测试的主体函数，包含了所有具体的测试用例。

#### 1. 构造函数测试 (Constructor Test)

- 测试了 `FBoxMinAndMax` 的默认构造函数、带参数构造函数以及拷贝构造函数。
- 验证了构造函数能否正确地根据输入的 `Min` 和 `Max` 值初始化对象，包括处理 `Min` 坐标大于 `Max` 坐标的情况（内部会自动交换）。

#### 2. 大小计算测试 (Size Test)

- 测试了 `FBoxMinAndMax::GetSize()` 方法。
- 验证了不同 `Min` 和 `Max` 组合下包围盒尺寸的计算是否正确，确保结果始终为非负值。

#### 3. 重叠判断测试 (Overlap Test)

- 测试了静态函数 `FBoxMinAndMax::Overlap(const FBoxMinAndMax&, const FBoxMinAndMax&)`。
- 通过多个预设的包围盒对，验证了该函数能否准确判断两个包围盒是否相交（包括边界接触的情况）。

#### 4. 旋转测试 (Rotation Test)

- 测试了 `Rotate(const FBoxMinAndMax&, EDoorDirection)` 辅助函数（定义在 `TestUtils.h` 中，用于模拟 `FBoxMinAndMax` 的旋转）。
- 验证了包围盒绕其中心点按不同方向（北、东、南、西）旋转90度后，其 `Min` 和 `Max` 坐标是否符合预期。

#### 5. 扩展测试 (Extend Test)

- 测试了 `FBoxMinAndMax::Extend(const FBoxMinAndMax&)` 方法。
- 验证了该方法能否正确地将当前包围盒扩展为包含另一个给定包围盒的最小包围盒。

#### 6. 包含判断测试 (IsInside Test)

- 测试了 `FBoxMinAndMax::IsInside(const FBoxMinAndMax&)` 和 `FBoxMinAndMax::IsInside(const FIntVector&)` 两个重载方法。
- 通过将测试包围盒移动到不同位置（完全包含、边界接触、相交、完全分离），验证了这两个方法能否准确判断一个包围盒/点是否被另一个包围盒完全包含。

### 辅助宏

- `TestEqual`: 用于断言两个值是否相等。
- `TestTrue`: 用于断言一个布尔表达式是否为真。
- `TestFalse`: 用于断言一个布尔表达式是否为假。

## 关键概念

- **自动化测试**: 使用 UE 的自动化测试框架来确保代码质量，防止回归错误。
- **单元测试**: 针对 `FBoxMinAndMax` 结构体的每个公开方法和相关辅助函数编写独立的测试用例。
- **边界条件测试**: 测试用例特别关注了边界情况，如 `Min` 和 `Max` 相等、包围盒旋转后坐标变化、边界接触等。
- **数据结构验证**: `FBoxMinAndMax` 作为一个基础几何数据结构，其正确性对整个地牢生成逻辑至关重要，因此需要全面的测试覆盖。