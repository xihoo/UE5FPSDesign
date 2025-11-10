# ProceduralDungeon.cpp

该文件是 `ProceduralDungeon` 模块的主要实现文件，负责模块的启动和关闭，以及插件设置的注册和注销。

## 主要功能

- **模块生命周期管理**：实现 `StartupModule` 和 `ShutdownModule` 函数，分别在模块加载和卸载时调用。
- **设置注册**：在 `StartupModule` 中注册插件设置，使用户可以在编辑器中配置插件。
- **设置注销**：在 `ShutdownModule` 中注销插件设置。
- **设置保存处理**：处理设置保存事件，可以用于验证设置或响应设置更改。
- **UE 5.4 Actor 替换修复**：针对 UE 5.4 版本中 PIE 模式下 `RoomLevel` Actor 引用问题的临时修复。

## 详细实现

### 模块生命周期

- `StartupModule`：
  - 调用 `RegisterSettings()` 注册插件设置。
  - 如果启用了 `ACTOR_REPLACEMENT_FIX_HACK`，则注册 `ObjectReplaced` 回调函数以修复 UE 5.4 中的 Actor 替换问题。
- `ShutdownModule`：
  - 如果 UObject 已初始化，则调用 `UnregisterSettings()` 注销插件设置。
  - 如果启用了 `ACTOR_REPLACEMENT_FIX_HACK`，则移除 `ObjectReplaced` 回调函数。

### 设置管理

- `RegisterSettings`：
  - 获取 `ISettingsModule` 指针。
  - 使用 `RegisterSettings` 函数注册 `UProceduralDungeonSettings` 类的默认对象。
  - 绑定 `OnModified` 事件到 `HandleSettingsSaved` 函数。
- `UnregisterSettings`：
  - 获取 `ISettingsModule` 指针。
  - 使用 `UnregisterSettings` 函数注销插件设置。

### 设置保存处理

- `HandleSettingsSaved`：
  - 获取 `UProceduralDungeonSettings` 的可变默认对象。
  - 可以在此处添加设置验证逻辑。
  - 如果需要重新保存设置，调用 `Settings->SaveConfig()`。

### UE 5.4 Actor 替换修复

- `ObjectReplaced`：
  - 遍历 `ReplacementMap`，查找 `ARoomLevel` Actor 的替换。
  - 如果找到有效的旧 Actor 和新 Actor，并且它们都不是 CDO，则修复 `Room` 引用。
  - 将旧 Actor 的 `Room` 引用设置为 `nullptr`，并将新 Actor 初始化为旧 Actor 的 `Room` 实例。

这个文件是整个插件的入口点，负责初始化和清理插件所需的各种资源和设置。