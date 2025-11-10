# ProceduralDungeonCustomVersion 类

## 概述

`FProceduralDungeonCustomVersion` 结构体定义了 Procedural Dungeon 插件的自定义序列化版本。它用于管理插件数据的序列化和反序列化过程中的版本控制。

## 枚举

### Type

- `InitialVersion`: 初始版本，表示在进行任何版本更改之前的状态。
- `VersionPlusOne`: 版本加一，用于标记新版本的添加点。
- `LatestVersion`: 最新版本，等于 `VersionPlusOne - 1`。

## 成员变量

- `GUID`: 此自定义版本号的 GUID。用于唯一标识此版本。