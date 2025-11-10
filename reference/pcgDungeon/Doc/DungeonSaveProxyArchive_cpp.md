# DungeonSaveProxyArchive.cpp

该文件包含了 `FDungeonSaveProxyArchive` 结构体的实现，该结构体是一个专门用于地牢保存的档案代理。

## 主要功能

- **自定义序列化**：重写 `operator<<` 以自定义 `FSoftObjectPath` 的序列化过程。

## 详细实现

### FDungeonSaveProxyArchive

- **构造函数**：`FDungeonSaveProxyArchive(FArchive& InInnerArchive)` 构造函数接受一个 `FArchive` 引用作为参数，并初始化基类 `FObjectAndNameAsStringProxyArchive`。
  - `ArIsSaveGame = true;`：标记此档案用于保存游戏。
- **重写 operator<<**：`virtual FArchive& operator<<(FSoftObjectPath& Value) override` 重写了基类的 `operator<<` 方法，以自定义 `FSoftObjectPath` 的序列化。
  - 首先调用基类的 `operator<<` 方法。
  - 记录一条警告日志，表明正在进行自定义序列化。
  - 如果正在加载且 `Value` 不为空，则调用 `Value.FixupCoreRedirects()` 来应用核心重定向。

这个结构体的主要目的是为了在保存和加载地牢数据时，能够正确地处理 `FSoftObjectPath` 类型的数据。