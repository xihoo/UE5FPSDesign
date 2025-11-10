# DoorDefTests.cpp

## 概述

`DoorDefTests.cpp` 是一个用于测试 `FDoorDef` 结构体功能的自动化测试文件。`FDoorDef` 定义在 `ProceduralDungeonTypes.h` 中，用于描述地牢中一个门的基本信息，包括其在网格中的位置 (`Position`)、方向 (`Direction`) 和类型 (`Type`)。该测试文件通过一系列单元测试用例，验证 `FDoorDef` 的构造、有效性判断、兼容性判断、获取对立门、以及坐标变换等功能的正确性。

## 代码解析

该文件同样使用 Unreal Engine 的自动化测试框架 (`Misc/AutomationTest.h`)。

### 测试套件定义

```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDoorDefTest, "ProceduralDungeon.Types.DoorDef", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::SmokeFilter)
```

- 定义了一个名为 `FDoorDefTest` 的简单自动化测试类。
- 测试类别为 `"ProceduralDungeon.Types.DoorDef"`。

### `RunTest` 函数

这是测试的主体函数。

#### 1. 构造函数和有效性测试 (Constructor & IsValid Test)

- 测试了 `FDoorDef` 的默认构造函数、带参数构造函数以及拷贝构造函数。
- 使用 `CREATE_DATA_ASSET(UDoorType, DoorTypeA)` 宏创建了测试所需的 `UDoorType` 数据资产实例。
- 验证了构造函数能否正确初始化 `Position`、`Direction` 和 `Type` 成员。
- 验证了 `IsValid()` 方法能否正确判断一个 `FDoorDef` 实例是否有效（即 `Type` 不为空指针）。

#### 2. 兼容性测试 (Compatibility Test)

- 测试了静态函数 `FDoorDef::AreCompatible(const FDoorDef&, const FDoorDef&)`。
- 通过创建具有相同或不同类型、不同位置和方向的 `FDoorDef` 实例，验证了该函数能否准确判断两个门定义是否可以连接（即方向相对、类型相同且至少有一个是有效的）。

#### 3. 对立门测试 (Opposite Test)

- 测试了 `FDoorDef::GetOpposite()` 方法。
- 验证了该方法能否根据当前门的位置和方向，正确计算出其在相邻房间中对应的对立门的位置和方向，并确保新门是有效的且与原门兼容。

#### 4. 坐标变换测试 (Transform Test)

- 测试了静态函数 `FDoorDef::Transform(const FDoorDef&, const FIntVector&, EDoorDirection)` 和 `FDoorDef::InverseTransform(const FDoorDef&, const FIntVector&, EDoorDirection)`。
- 验证了 `Transform` 方法能否根据给定的偏移和旋转方向，正确地将一个门定义从一个房间的局部坐标系变换到世界坐标系（或另一个房间的坐标系）。
- 验证了 `InverseTransform` 方法能否执行反向变换，并且 `Transform` 和 `InverseTransform` 能够相互抵消。

### 辅助宏和函数

- `CREATE_DATA_ASSET`: 一个在 `TestUtils.h` 中定义的宏，用于方便地创建和初始化测试所需的数据资产。
- `TestTrue`, `TestFalse`, `TestEqual`: 用于断言的宏。

## 关键概念

- **门的定义与表示**: `FDoorDef` 是地牢生成中连接不同房间的基本元素，其属性的正确性至关重要。
- **门的兼容性**: 两个门能否连接取决于它们的方向是否相对、类型是否相同，这是地牢生成算法中的核心逻辑之一。
- **对立门**: 在生成相邻房间时，需要根据一个门的位置和方向计算出其对立门，以确保连接的正确性。
- **坐标变换**: 在处理不同房间的相对位置和旋转时，需要对门的定义进行坐标变换，以保证它们在全局坐标系下能正确对齐。
- **自动化测试**: 通过全面的单元测试确保 `FDoorDef` 结构体在各种情况下的行为符合预期，为地牢生成的稳定性提供基础。