# ProceduralDungeonEdLog.cpp

## 概述

`ProceduralDungeonEdLog.cpp` 是 `ProceduralDungeonEdLog.h` 的配套实现文件。它的主要作用是定义在头文件中声明的日志类别。

## 实现细节

### 日志类别定义

- `DEFINE_LOG_CATEGORY(LogProceduralDungeonEditor);`: 这行代码定义了 `LogProceduralDungeonEditor` 日志类别。`DECLARE_LOG_CATEGORY_EXTERN` 在头文件中声明了这个类别，而 `DEFINE_LOG_CATEGORY` 则在 `.cpp` 文件中为其分配实际的内存和标识符，使其可以在整个模块中使用。

没有其他代码。这个文件非常简洁，只负责日志类别的定义。