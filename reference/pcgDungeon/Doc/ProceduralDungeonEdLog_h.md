# ProceduralDungeonEdLog.h

## 概述

`ProceduralDungeonEdLog.h` 定义了用于“地牢房间编辑器模式”和相关编辑器功能的日志类别和宏。它提供了一种标准化的方式来记录插件在编辑器中运行时的信息、警告和错误。

## 宏定义

### 日志类别声明

- `DECLARE_LOG_CATEGORY_EXTERN(LogProceduralDungeonEditor, Verbose, All);`: 声明一个名为 `LogProceduralDungeonEditor` 的日志类别。它在外部（`.cpp` 文件）中定义，日志级别为 `Verbose`，可以记录所有级别的日志（`All`）。

### 日志宏

这些宏为 `LogProceduralDungeonEditor` 日志类别提供了便捷的调用方式。

- `DungeonEd_LogInfo(Format, ...)`: 记录一条 `Log` 级别的信息。对应 `UE_LOG(LogProceduralDungeonEditor, Log, ...)`。
- `DungeonEd_LogWarning(Format, ...)`: 记录一条 `Warning` 级别的信息。对应 `UE_LOG(LogProceduralDungeonEditor, Warning, ...)`。
- `DungeonEd_LogError(Format, ...)`: 记录一条 `Error` 级别的信息。对应 `UE_LOG(LogProceduralDungeonEditor, Error, ...)`。

这些宏简化了日志记录的代码，使其更易读和使用。