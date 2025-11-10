# TriggerDoor 类

## 概述

`ATriggerDoor` 类是一个当 Actor 进入触发器框时打开的门。你可以通过重写 `IsValidActor` 函数来自定义哪些 Actor 可以打开门。它继承自 `ADoor`。

## 成员变量

- `BoxComponent` (UBoxComponent*): 触发器框组件。
- `ActorList` (TSet<class AActor*>): 可以打开门的 Actor 列表。

## 成员函数

### 构造函数

- `ATriggerDoor()`: 构造函数。

### 虚函数重写

- `BeginPlay()`: 开始游戏时调用。

### 受保护函数

- `OnTriggerEnter(...)`: 触发器进入时调用。
- `OnTriggerExit(...)`: 触发器退出时调用。
- `IsValidActor(AActor* Actor, UPrimitiveComponent* Component)`: 应该返回 true 如果 Actor 可以打开门。默认情况下，如果 Actor 是 Character，则有效。
- `UpdateOpenState()`: 更新门的打开状态。