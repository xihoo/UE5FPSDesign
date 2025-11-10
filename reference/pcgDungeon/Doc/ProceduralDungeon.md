# ProceduralDungeon 模块

## 概述

`FProceduralDungeonModule` 类是 ProceduralDungeon 插件的模块管理器。它继承自 `IModuleInterface`，负责插件的启动和关闭逻辑。

## 成员函数

### IModuleInterface 实现

- `StartupModule()`: 模块启动时调用。用于初始化插件。
- `ShutdownModule()`: 模块关闭时调用。用于清理插件资源。
- `SupportsDynamicReloading()`: 返回 `true`，表示该模块支持动态重载。

### 私有函数

- `RegisterSettings()`: 注册插件设置。
- `UnregisterSettings()`: 注销插件设置。
- `HandleSettingsSaved()`: 处理设置保存事件。