# DungeonSaveUtils.cpp

## 概述

`DungeonSaveUtils.cpp` 文件提供了一套用于序列化和反序列化 UObject 及其相关类（如 `UClass`）的实用工具函数，特别针对地牢保存/加载系统进行了优化。这些函数处理了标准的 UObject 属性序列化，以及通过 `IDungeonSaveInterface` 和 `IDungeonCustomSerialization` 接口实现的自定义序列化逻辑。该文件是地牢系统持久化功能的核心组件之一。

## 代码解析

### `CreateArchiveFormatterFromArchive` 函数

```cpp
TUniquePtr<FArchiveFormatterType> CreateArchiveFormatterFromArchive(FArchive& Ar, bool bTextFormat)
```

- **功能**: 根据给定的 `FArchive` 和格式标志 (`bTextFormat`)，创建并返回一个合适的 `FArchiveFormatterType` 对象的唯一指针。
- **参数**:
    - `FArchive& Ar`: 基础的档案对象，用于实际的读写操作。
    - `bool bTextFormat`: 指示是否应使用文本（JSON）格式进行序列化。
- **逻辑**:
    - 如果启用了文本格式支持（非 Shipping 构建且 `WITH_TEXT_ARCHIVE_SUPPORT` 为真）且 `bTextFormat` 为 `true`，则根据 `Ar` 是保存还是加载，创建 `FJsonArchiveOutputFormatter` 或 `FJsonArchiveInputFormatter`。
    - 否则，创建默认的 `FBinaryArchiveFormatter` 用于二进制序列化。
- **关键点**: 这个函数是实现二进制和文本（JSON）格式保存/加载的关键。

### `SerializeUObject` (FStructuredArchive::FRecord 版本)

```cpp
bool SerializeUObject(FStructuredArchive::FRecord& Record, UObject* Obj, bool bIsLoading)
```

- **功能**: 使用 `FStructuredArchive::FRecord` 对一个 `UObject` 进行序列化或反序列化。
- **参数**:
    - `FStructuredArchive::FRecord& Record`: 结构化档案记录，用于读写数据。
    - `UObject* Obj`: 要序列化的 UObject 指针。
    - `bool bIsLoading`: `true` 表示正在加载（反序列化），`false` 表示正在保存（序列化）。
- **返回值**: `true` 表示操作成功（尽管内部自定义序列化可能失败，但此函数本身不直接反映其结果）。
- **逻辑**:
    1.  检查 `Obj` 是否有效。
    2.  检查 `Obj` 是否实现了 `UDungeonSaveInterface` 接口。
    3.  如果实现了接口且正在保存，则调用 `IDungeonSaveInterface::Execute_DungeonPreSerialize`，允许对象在序列化前进行准备。
    4.  调用 `Obj->SerializeScriptProperties` 序列化 UObject 的蓝图可序列化属性。
    5.  尝试将 `Obj` 转换为 `IDungeonCustomSerialization` 接口。如果成功，则调用其 `SerializeObject` 方法处理非标准属性（如 UObject 指针）。
    6.  如果实现了 `UDungeonSaveInterface` 且正在加载，则调用 `IDungeonSaveInterface::Execute_DungeonPostSerialize`，允许对象在反序列化后进行初始化。
- **关键点**: 这是序列化流程的核心，整合了标准属性序列化和自定义接口。

### `SerializeUObject` (FArchive 版本)

```cpp
bool SerializeUObject(FArchive& Ar, UObject* Obj, bool bIsLoading, bool bTextFormat)
```

- **功能**: 使用 `FArchive` 对一个 `UObject` 进行序列化或反序列化。
- **参数**:
    - `FArchive& Ar`: 基础档案对象。
    - `UObject* Obj`: 要序列化的 UObject 指针。
    - `bool bIsLoading`: `true` 表示正在加载。
    - `bool bTextFormat`: `true` 表示使用文本格式。
- **返回值**: `true`。
- **逻辑**:
    1.  调用 `CreateArchiveFormatterFromArchive` 获取合适的格式化器。
    2.  使用该格式化器创建一个 `FStructuredArchive` 对象。
    3.  打开档案并进入记录。
    4.  调用 `FStructuredArchive::FRecord` 版本的 `SerializeUObject` 函数完成实际工作。
- **关键点**: 作为 `FStructuredArchive::FRecord` 版本的包装器，提供了从传统 `FArchive` 到结构化档案的桥梁。

### `SerializeUObject` (TArray<uint8> 版本)

```cpp
bool SerializeUObject(TArray<uint8>& Data, UObject* Obj, bool bIsLoading, bool bTextFormat)
```

- **功能**: 直接将 `UObject` 序列化到一个字节数组，或从字节数组反序列化。
- **参数**:
    - `TArray<uint8>& Data`: 用于存储序列化数据的字节数组。
    - `UObject* Obj`: 要序列化的 UObject 指针。
    - `bool bIsLoading`: `true` 表示从 `Data` 加载，`false` 表示保存到 `Data`。
    - `bool bTextFormat`: `true` 表示使用文本格式。
- **返回值**: `true`。
- **逻辑**:
    1.  根据 `bIsLoading` 标志，创建 `FMemoryReader` 或 `FMemoryWriter` 来包装 `Data` 数组。
    2.  创建 `FObjectAndNameAsStringProxyArchive` 包装器，这通常用于处理 UObject 的引用。
    3.  设置 `ArIsSaveGame = true`，表明这是一个保存游戏相关的操作。
    4.  调用 `FArchive` 版本的 `SerializeUObject` 函数。
- **关键点**: 提供了最顶层的便利函数，直接操作内存中的字节数据，非常适合保存到文件或网络传输。

### `SerializeUClass` 函数

```cpp
void SerializeUClass(FStructuredArchiveSlot Slot, UClass*& Class)
```

- **功能**: 序列化或反序列化一个 `UClass` 指针。
- **参数**:
    - `FStructuredArchiveSlot Slot`: 用于读写的结构化档案槽。
    - `UClass*& Class`: 要序列化的 `UClass` 指针的引用。
- **逻辑**:
    1.  将 `Class` 转换为 `FSoftObjectPath`。
    2.  通过 `Slot` 序列化 `FSoftObjectPath` 字符串。
    3.  如果正在加载 (`Slot.GetArchiveState().IsLoading()`):
        -  调用 `ClassPath.FixupCoreRedirects()` 处理可能的类重定向（例如，类被重命名后仍能正确加载）。
        -  尝试通过 `ClassPath.ResolveObject()` 解析对象。
        -  如果解析失败且路径非空，尝试通过 `ClassPath.TryLoad()` 加载类。
        -  如果最终 `Class` 仍为空，则记录错误日志。
- **关键点**: 安全地序列化 `UClass` 引用，处理了重定向和延迟加载，避免了直接序列化原始指针的问题。

### `IsLoading` 和 `IsSaving` 函数

```cpp
bool IsLoading(const FStructuredArchiveSlotBase& Slot)
bool IsSaving(const FStructuredArchiveSlotBase& Slot)
```

- **功能**: 提供便捷的方法来检查给定的 `FStructuredArchiveSlotBase` 是处于加载状态还是保存状态。
- **参数**: `FStructuredArchiveSlotBase` 的常量引用。
- **返回值**: 布尔值，表示当前状态。
- **关键点**: 简化了在自定义序列化代码中检查操作类型的逻辑。

## 关键概念

- **分层序列化**: 代码展示了从底层 `FArchive` 到 `FStructuredArchive` 再到 `UObject` 属性和自定义接口的分层处理方式。
- **接口驱动**: 通过 `IDungeonSaveInterface` 和 `IDungeonCustomSerialization` 接口，允许对象和类自定义其保存/加载行为，提供了极大的灵活性。
- **格式支持**: 支持二进制和 JSON 文本格式的序列化，便于调试和跨平台兼容性。
- **安全引用**: 使用 `FSoftObjectPath` 序列化 `UClass`，并处理重定向，确保了引用的健壮性。
- **生命周期事件**: `DungeonPreSerialize` 和 `DungeonPostSerialize` 提供了在序列化关键节点执行自定义逻辑的机会。