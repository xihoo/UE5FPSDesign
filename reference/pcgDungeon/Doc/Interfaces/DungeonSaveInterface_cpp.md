# DungeonSaveInterface.cpp

## 概述

`DungeonSaveInterface.cpp` 文件包含了 `IDungeonSaveInterface` 接口的实现。该接口为在地牢保存/加载过程中需要执行特定逻辑的 Actor 或 UObject 提供了几个可重写的事件函数。此 `.cpp` 文件主要实现了两个静态的分发函数，用于在保存前和加载后触发这些事件。

## 代码解析

### `IDungeonSaveInterface::DispatchPreSaveEvent` 函数

```cpp
void IDungeonSaveInterface::DispatchPreSaveEvent(UObject* Obj)
{
	check(IsValid(Obj));
	DungeonLog_Debug("[BEGIN] Dispatch 'Dungeon Pre Save' events from object '%s'.", *GetNameSafe(Obj));

	ObjectUtils::DispatchToObjectAndSubobjects(Obj, [](UObject* O) {
		if (IsValid(O) && O->Implements<UDungeonSaveInterface>())
		{
			IDungeonSaveInterface::Execute_PreSaveDungeon(O);
		}
	});

	DungeonLog_Debug("[END] Dispatch 'Dungeon Post Load' events from object '%s'.", *GetNameSafe(Obj)); // Note: Log message seems to have a typo, should probably be 'Pre Save'.
}
```

- **功能**: 这是一个静态函数，用于在地牢保存之前，分发 `PreSaveDungeon` 事件给指定对象 `Obj` 及其所有子对象（Subobjects）。
- **参数**: `UObject* Obj` - 需要分发事件的对象指针。
- **逻辑**:
    1.  使用 `check(IsValid(Obj))` 确保传入的对象指针有效。
    2.  记录调试日志，表示开始分发 "Dungeon Pre Save" 事件。
    3.  调用 `ObjectUtils::DispatchToObjectAndSubobjects` 工具函数。这个函数会遍历 `Obj` 对象本身及其所有子对象。
    4.  对于遍历到的每一个对象 `O`，检查它是否有效并且是否实现了 `UDungeonSaveInterface` 接口。
    5.  如果条件满足，则通过 `IDungeonSaveInterface::Execute_PreSaveDungeon(O)` 执行该对象上实现的 `PreSaveDungeon` 事件。
    6.  记录调试日志，表示结束分发。**注意**: 日志信息中写的是 "Dungeon Post Load"，这可能是一个笔误，应为 "Dungeon Pre Save"。

### `IDungeonSaveInterface::DispatchPostLoadEvent` 函数

```cpp
void IDungeonSaveInterface::DispatchPostLoadEvent(UObject* Obj)
{
	check(IsValid(Obj));
	DungeonLog_Debug("[BEGIN] Dispatch 'Dungeon Post Load' events from object '%s'.", *GetNameSafe(Obj));

	ObjectUtils::DispatchToObjectAndSubobjects(Obj, [](UObject* O) {
		if (IsValid(O) && O->Implements<UDungeonSaveInterface>())
		{
			IDungeonSaveInterface::Execute_PostLoadDungeon(O);
		}
	});

	DungeonLog_Debug("[END] Dispatch 'Dungeon Post Load' events from object '%s'.", *GetNameSafe(Obj));
}
```

- **功能**: 这是一个静态函数，用于在地牢加载完成之后，分发 `PostLoadDungeon` 事件给指定对象 `Obj` 及其所有子对象。
- **参数**: `UObject* Obj` - 需要分发事件的对象指针。
- **逻辑**:
    1.  使用 `check(IsValid(Obj))` 确保传入的对象指针有效。
    2.  记录调试日志，表示开始分发 "Dungeon Post Load" 事件。
    3.  调用 `ObjectUtils::DispatchToObjectAndSubobjects` 工具函数，遍历 `Obj` 对象本身及其所有子对象。
    4.  对于遍历到的每一个对象 `O`，检查它是否有效并且是否实现了 `UDungeonSaveInterface` 接口。
    5.  如果条件满足，则通过 `IDungeonSaveInterface::Execute_PostLoadDungeon(O)` 执行该对象上实现的 `PostLoadDungeon` 事件。
    6.  记录调试日志，表示结束分发 "Dungeon Post Load" 事件。

## 关键概念

- **事件分发机制**: 该文件的核心是提供了一种机制，能够将特定的保存/加载事件广播给一个对象及其所有子对象。这使得复杂的 Actor 结构（包含多个组件）能够统一响应地牢的保存和加载过程。
- **接口执行**: 通过 `IDungeonSaveInterface::Execute_*` 宏来调用接口中定义的蓝图可重写事件（`BlueprintNativeEvent`），这确保了无论是在 C++ 还是在蓝图中实现的逻辑都能被正确执行。
- **工具函数 `ObjectUtils::DispatchToObjectAndSubobjects`**: 这个未在此文件中定义的工具函数是实现遍历和分发的关键，它负责处理对象及其子对象的迭代。