# DoorType.cpp

## 概述

`DoorType.cpp` 文件包含了 `UDoorType` 类的实现。这个类是一个数据资产，用于定义门的类型，包括门的大小、偏移量和颜色。

## 函数实现

### UDoorType()

```cpp
UDoorType::UDoorType()
```

构造函数。初始化门类型数据资产的默认值：

- `Size`: 从插件设置中获取默认门大小。
- `Offset`: 从插件设置中获取默认门偏移量。
- `Color`: 在编辑器中默认为蓝色。
- `Description`: 在编辑器中默认为 "No Description"。

### GetSize()

```cpp
FVector UDoorType::GetSize(const UDoorType* DoorType)
```

获取门的大小。如果提供了有效的 `DoorType`，则返回该门类型的大小；否则返回插件设置中的默认门大小。

### GetOffset()

```cpp
float UDoorType::GetOffset(const UDoorType* DoorType)
```

获取门的偏移量。如果提供了有效的 `DoorType`，则返回该门类型的偏移量；否则返回插件设置中的默认门偏移量。

### GetColor()

```cpp
FColor UDoorType::GetColor(const UDoorType* DoorType)
```

获取门的颜色。如果提供了有效的 `DoorType`，则返回该门类型的颜色；否则返回插件设置中的默认门颜色。