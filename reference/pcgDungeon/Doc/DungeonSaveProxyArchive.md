# DungeonSaveProxyArchive 类

## 概述

`FDungeonSaveProxyArchive` 是一个专门用于地牢保存的归档代理。它继承自 `FObjectAndNameAsStringProxyArchive`，并重写了 `operator<<` 以处理 `FSoftObjectPath` 的序列化。

## 成员函数

### 构造函数

- `FDungeonSaveProxyArchive(FArchive& InInnerArchive)`: 构造函数，接受一个 `FArchive` 引用作为内部归档。它将 `ArIsSaveGame` 设置为 `true`。

### 重载操作符

- `virtual FArchive& operator<<(FSoftObjectPath& Value) override`: 重载 `<<` 操作符以序列化 `FSoftObjectPath`。它首先调用基类的 `operator<<` 来序列化路径，然后在加载时应用核心重定向。