# DoorType 类

## 概述

`UDoorType` 类是一个数据资产，用于定义门的类型。门类型用于定义门的大小、偏移量和颜色。具有不同类型的门彼此不兼容。

## 成员变量

- `Size` (FVector): 门的大小，仅用于调试绘制，作为设计师和艺术家的视觉提示。
- `Offset` (float): 门相对于房间基础的偏移量（以房间单元 Z 的百分比表示）。
- `Color` (FColor): 在编辑器中绘制门的边界框时使用的颜色。
- `Description` (FText): 仅用于描述，不用于任何功能。

## 成员函数

### 构造函数

- `UDoorType()`: 构造函数。

### 静态函数

- `GetSize(const UDoorType* DoorType)`: 从门类型资产返回门的大小，如果未定义门类型，则返回插件设置中的默认门大小。
- `GetOffset(const UDoorType* DoorType)`: 从门类型资产返回门的偏移量，如果未定义门类型，则返回插件设置中的默认门偏移量。
- `GetColor(const UDoorType* DoorType)`: 从门类型资产返回门的颜色，如果未定义门类型，则返回插件设置中的默认门颜色。