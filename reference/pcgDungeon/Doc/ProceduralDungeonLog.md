# ProceduralDungeonLog 类

## 概述

`ProceduralDungeonLog` 头文件定义了用于 Procedural Dungeon 插件的日志记录功能。它提供了多种宏来记录不同级别的日志信息，并可以选择性地将日志显示在屏幕上。

## 日志类别

- `LogProceduralDungeon`: 插件的主要日志类别。

## 函数

### LogOnScreen

```cpp
void LogOnScreen(const FString& Message, FColor Color, bool bForceOnScreen = false);
```

在屏幕上显示日志消息。

- `Message`: 要显示的消息。
- `Color`: 消息的颜色。
- `bForceOnScreen`: 是否强制显示在屏幕上。

## 宏定义

### DungeonLog_Debug

```cpp
#define DungeonLog_Debug(Format, ...)
```

记录调试级别的日志信息，仅输出到日志文件。

### DungeonLog_Info

```cpp
#define DungeonLog_Info(Format, ...)
```

记录信息级别的日志信息，并显示在屏幕上。

### DungeonLog_InfoSilent

```cpp
#define DungeonLog_InfoSilent(Format, ...)
```

记录信息级别的日志信息，仅输出到日志文件。

### DungeonLog_Warning

```cpp
#define DungeonLog_Warning(Format, ...)
```

记录警告级别的日志信息，并显示在屏幕上。

### DungeonLog_WarningSilent

```cpp
#define DungeonLog_WarningSilent(Format, ...)
```

记录警告级别的日志信息，仅输出到日志文件。

### DungeonLog_Error

```cpp
#define DungeonLog_Error(Format, ...)
```

记录错误级别的日志信息，并强制显示在屏幕上。