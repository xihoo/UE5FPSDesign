# QueueOrStack 类

## 概述

`TQueueOrStack` 是一个模板类，它可以根据指定的模式（队列或栈）来存储和管理元素。它内部使用 `TQueue` 和 `TArray` 来实现队列和栈的功能。

## 枚举

### EMode

- `QUEUE`: 队列模式（先进先出）。
- `STACK`: 栈模式（后进先出）。

## 成员函数

### 构造函数

- `TQueueOrStack(EMode _Mode)`: 构造函数，接受一个 `EMode` 枚举值来指定模式（队列或栈）。

### 公共函数

- `void Push(T& Element)`: 将元素推入队列或栈中，具体取决于模式。
- `T Pop()`: 从队列或栈中弹出元素，具体取决于模式。如果队列或栈为空，则会触发断言。
- `int Num()`: 返回队列或栈中元素的数量。
- `bool IsEmpty()`: 检查队列或栈是否为空。

## 私有成员

- `Mode` (EMode): 存储当前的模式（队列或栈）。
- `Queue` (TQueue<T>): 用于队列模式的内部队列。
- `Stack` (TArray<T>): 用于栈模式的内部数组。