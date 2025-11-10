# DungeonSave.spec.cpp

## 概述

`DungeonSave.spec.cpp` 是一个使用 Unreal Engine 的自动化测试框架中的 `DEFINE_SPEC` 宏编写的测试文件，专门用于测试地牢保存/加载系统的核心功能。该文件主要验证 `IDungeonSaveInterface` 和 `IDungeonCustomSerialization` 接口在保存和加载过程中的行为是否符合预期，确保实现了这些接口的对象能够被正确地序列化和反序列化。

## 代码解析

该文件使用 UE 的 `DEFINE_SPEC` 宏来定义一个行为驱动开发 (BDD) 风格的测试规范。

### 测试规范定义

```cpp
DEFINE_SPEC(FDungeonSaveSpecs, "ProceduralDungeon.SaveLoad", FLAG_APPLICATION_CONTEXT | EAutomationTestFlags::EngineFilter)
```

- 定义了一个名为 `FDungeonSaveSpecs` 的测试规范类。
- 测试类别为 `"ProceduralDungeon.SaveLoad"`。
- 使用 `EAutomationTestFlags::EngineFilter`，表明这是一个需要引擎环境的测试。

### `Define` 函数

这是测试规范的核心，定义了测试的结构和内容。

#### `Describe` 块

```cpp
Describe(TEXT("Interfaces"), [this]() { ... });
```

- 定义了一个名为 "Interfaces" 的测试描述块，用于组织相关的测试用例。

#### `It` 块 (测试用例)

```cpp
It(TEXT("DungeonSaveInterface"), [this]() { ... });
```

- 定义了一个具体的测试用例，名为 "DungeonSaveInterface"。

##### 测试逻辑

1.  **对象创建**:
    - 使用 `TStrongObjectPtr` 创建了两个 `USaveTestObject` 类型的对象：`TestSave`（用于保存）和 `TestLoad`（用于加载）。`USaveTestObject` 是一个专门用于测试的 UObject 类，它实现了 `IDungeonSaveInterface` 和 `IDungeonCustomSerialization` 接口。
2.  **设置初始状态**:
    - 为 `TestSave` 对象设置了一些测试属性值：`TestSaveGameFlag = 5` 和 `TestSerializeObjectFunction = 8`。
3.  **保存过程**:
    - 创建一个空的 `TArray<uint8> SavedData` 用于存储序列化后的二进制数据。
    - 调用 `SerializeUObject(SavedData, TestSave.Get(), false)`。这里的 `false` 表示这是保存操作。
    - 验证 `TestSave->OrderOfExecution` 字符串是否为 `"BCD"`。这表明在保存过程中，`IDungeonSaveInterface` 的相关事件（如 `DungeonPreSerialize`）已被正确调用。
    - 验证 `SavedData` 数组不为空，确认有数据被写入。
4.  **加载过程**:
    - 调用 `SerializeUObject(SavedData, TestLoad.Get(), true)`。这里的 `true` 表示这是加载操作。
    - 验证 `TestLoad->OrderOfExecution` 字符串是否为 `"WXY"`。这表明在加载过程中，`IDungeonSaveInterface` 的相关事件（如 `DungeonPostSerialize`）已被正确调用。
    - 验证 `TestLoad` 对象的 `TestSaveGameFlag` 和 `TestSerializeObjectFunction` 属性值是否与 `TestSave` 对象的原始值相等，确认数据被成功加载和恢复。

### 关键函数

- `SerializeUObject`: 这是一个在 `DungeonSaveUtils.h/cpp` 中定义的辅助函数，负责执行 UObject 的自定义序列化逻辑，处理 `IDungeonSaveInterface` 和 `IDungeonCustomSerialization` 接口的调用。

## 关键概念

- **行为驱动开发 (BDD)**: 使用 `Describe` 和 `It` 宏使测试更具可读性和描述性，清晰地表达了被测试的功能和期望的行为。
- **接口测试**: 该测试的核心是验证自定义保存/加载接口的正确实现和调用顺序。
- **序列化/反序列化**: 测试了整个保存和加载流程，确保数据能够被完整、准确地持久化和恢复。
- **事件驱动**: `IDungeonSaveInterface` 定义的事件（如 `DungeonPreSerialize`, `DungeonPostSerialize`）在保存/加载生命周期的关键节点被触发，测试验证了这些事件的执行顺序。
- **自动化测试**: 通过自动化测试确保保存/加载系统的稳定性和可靠性，防止在代码变更时引入回归错误。