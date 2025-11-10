# QueueOrStack.cpp

该文件包含了 `TQueueOrStack` 模板类的实现，该类是一个可以根据模式在队列（FIFO）和栈（LIFO）之间切换的数据结构。

## 主要功能

- **模式切换**：通过构造函数指定使用队列模式还是栈模式。
- **元素操作**：提供 `Push` 和 `Pop` 方法来添加和移除元素。
- **状态查询**：提供 `Num` 和 `IsEmpty` 方法来查询元素数量和是否为空。

## 详细实现

### 模板类定义

- `template<typename T> class TQueueOrStack`：定义了一个模板类，可以存储任意类型 `T` 的元素。

### 枚举类型

- `enum class EMode { QUEUE, STACK }`：定义了两种模式，队列模式和栈模式。

### 构造函数

- `TQueueOrStack(EMode _Mode)`：接受一个 `EMode` 枚举值作为参数，初始化 `Mode` 成员变量，并初始化内部的 `Queue` 和 `Stack`。

### 成员变量

- `EMode Mode`：存储当前的模式（队列或栈）。
- `TQueue<T> Queue`：用于队列模式的内部队列。
- `TArray<T> Stack`：用于栈模式的内部数组。

### 公共方法

- `void Push(T& Element)`：根据当前模式将元素添加到队列或栈中。
  - 如果是队列模式，调用 `Queue.Enqueue`。
  - 如果是栈模式，调用 `Stack.Push`。
- `T Pop()`：根据当前模式从队列或栈中移除并返回元素。
  - 如果是队列模式，调用 `Queue.Dequeue`。
  - 如果是栈模式，调用 `Stack.Pop`。
  - 使用 `check(!IsEmpty())` 确保在弹出元素前队列或栈不为空。
- `int Num()`：返回当前队列或栈中的元素数量。
  - 如果是队列模式，返回 `Queue.Num()`。
  - 如果是栈模式，返回 `Stack.Num()`。
- `bool IsEmpty()`：检查队列或栈是否为空。
  - 如果是队列模式，返回 `Queue.IsEmpty()`。
  - 如果是栈模式，返回 `Stack.Num() <= 0`。

这个类的主要用途是在需要根据配置或条件在队列和栈行为之间切换的场景中，例如在地牢生成算法中选择深度优先搜索（DFS）或广度优先搜索（BFS）。