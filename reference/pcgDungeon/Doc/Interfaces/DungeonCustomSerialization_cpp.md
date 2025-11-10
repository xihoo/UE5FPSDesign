# DungeonCustomSerialization.cpp

## 概述

`DungeonCustomSerialization.cpp` 文件包含了 `IDungeonCustomSerialization` 接口的实现。该接口用于处理地牢保存/加载过程中非普通数据类型的自定义序列化和反序列化，特别是 UObject 指针的引用修复。此 `.cpp` 文件主要实现了一个静态的分发函数 `DispatchFixupReferences`，用于在加载后调用对象及其子对象上的 `FixupReferences` 方法。

## 代码解析

### `IDungeonCustomSerialization::DispatchFixupReferences` 函数

```cpp
bool IDungeonCustomSerialization::DispatchFixupReferences(UObject* Obj, UObject* Context)
{
	check(IsValid(Obj));
	DungeonLog_Debug("[BEGIN] Dispatch 'Fixup References' function from object '%s'.", *GetNameSafe(Obj));

	ObjectUtils::DispatchToObjectAndSubobjects(Obj, [Context](UObject* O) {
		auto* Custom = Cast<IDungeonCustomSerialization>(O);
		if (nullptr != Custom)
		{
			Custom->FixupReferences(Context);
		}
	});

	DungeonLog_Debug("[END] Dispatch 'Fixup References' function from object '%s'.", *GetNameSafe(Obj));
	return true;
}
```

- **功能**: 这是一个静态函数，用于在地牢加载过程中，分发 `FixupReferences` 调用给指定对象 `Obj` 及其所有子对象。这对于修复保存/加载过程中可能失效的 UObject 引用至关重要。
- **参数**:
    - `UObject* Obj`: 需要分发 `FixupReferences` 调用的对象指针。
    - `UObject* Context`: 传递给 `FixupReferences` 方法的上下文对象，通常包含加载过程中需要的信息。
- **返回值**: 返回 `true`，表示分发过程已成功启动（但不保证每个 `FixupReferences` 调用都成功）。
- **逻辑**:
    1.  使用 `check(IsValid(Obj))` 确保传入的主对象指针有效。
    2.  记录调试日志，表示开始分发 "Fixup References" 函数。
    3.  调用 `ObjectUtils::DispatchToObjectAndSubobjects` 工具函数，遍历 `Obj` 对象本身及其所有子对象。
    4.  对于遍历到的每一个对象 `O`，使用 `Cast<IDungeonCustomSerialization>(O)` 尝试将其转换为 `IDungeonCustomSerialization` 接口指针。
    5.  如果转换成功（即 `O` 实现了该接口），则调用该对象上的 `Custom->FixupReferences(Context)` 方法，传入上下文 `Context`。
    6.  记录调试日志，表示结束分发 "Fixup References" 函数。

## 关键概念

- **自定义序列化**: `IDungeonCustomSerialization` 接口允许对象定义自己的序列化逻辑 (`SerializeObject`) 和加载后的引用修复逻辑 (`FixupReferences`)，这对于处理复杂的 UObject 图非常有用。
- **引用修复 (Fixup References)**: 在序列化系统加载对象后，某些 UObject 指针可能尚未正确解析或指向了错误的对象。`FixupReferences` 方法提供了一个机会，在所有对象都加载完毕后，手动修复这些引用。
- **事件分发机制**: 与 `DungeonSaveInterface` 类似，该文件也利用 `ObjectUtils::DispatchToObjectAndSubobjects` 来确保对象及其所有子对象都能接收到 `FixupReferences` 调用。
- **接口转换 (Cast)**: 使用 `Cast<T>(Object)` 是 Unreal Engine 中检查对象是否实现特定接口或类并获取其接口/类指针的标准方法。