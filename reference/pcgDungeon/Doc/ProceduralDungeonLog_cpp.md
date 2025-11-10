# ProceduralDungeonLog.cpp

该文件实现了 `ProceduralDungeon` 插件的日志记录功能。

## 主要功能

- **定义日志类别**：定义 `LogProceduralDungeon` 日志类别。
- **屏幕日志输出**：提供将日志消息输出到屏幕的功能。

## 详细实现

### 日志类别定义

- `DEFINE_LOG_CATEGORY(LogProceduralDungeon);`
  - 定义了 `LogProceduralDungeon` 日志类别，用于在 Unreal Engine 的日志系统中记录插件相关的消息。

### 屏幕日志输出

- `ShowLogsOnScreen` (匿名命名空间内的局部函数)：
  - 从 `UProceduralDungeonSettings` 获取设置，判断是否应该在屏幕上显示日志。
  - 返回 `OnScreenPrintDebug` 设置的值，并通过引用参数 `_duration` 返回 `PrintDebugDuration` 设置的值。
- `LogOnScreen`：
  - 接受一个消息字符串、颜色和一个强制显示在屏幕上的标志。
  - 调用 `ShowLogsOnScreen` 获取显示设置和持续时间。
  - 如果设置允许或 `bForceOnScreen` 为 `true`，则使用 `GEngine->AddOnScreenDebugMessage` 将消息显示在屏幕上。

这个文件与 `ProceduralDungeonLog.h` 头文件配合使用，提供了方便的日志记录宏，可以在开发和调试过程中输出详细的信息。