# ProceduralDungeonEdTypes.h

## 概述

`ProceduralDungeonEdTypes.h` 定义了一些在“地牢房间编辑器”中使用的特定类型、宏和实用函数。它主要处理与不同 Unreal Engine 版本的兼容性问题，并定义了一个用于表示 3D 边距的结构体 `FMargin3D`。

## 宏定义

### 版本兼容性宏

- `COMPATIBILITY`: 根据 UE 版本（< 5.0.0）定义为 1 或 0，用于条件编译。
- `GC_PTR(VAR_TYPE)`: 根据 UE 版本（> 5.3.0）定义为 `TObjectPtr<VAR_TYPE>` 或 `VAR_TYPE*`，用于处理不同版本中 UObject 指针的垃圾回收差异。

### 类型别名

- `WidgetMode`: 根据 `COMPATIBILITY` 宏，将 `WidgetMode` 别名为旧版的 `FWidget::EWidgetMode` 或新版的 `UE::Widget::EWidgetMode`，以确保在不同 UE 版本中都能正确引用变换控件的模式枚举。

## 实用函数声明

这些是为 `EAxisList::Type` 枚举定义的运算符重载，方便进行位操作。

- `EAxisList::Type operator~(const EAxisList::Type& This);`: 按位取反。
- `EAxisList::Type& operator&=(EAxisList::Type& This, const EAxisList::Type& Other);`: 按位与并赋值。

## 结构体定义

### FMargin3D

这是一个 `USTRUCT`，用于存储 3D 空间中各个轴向的边距值，常用于调整体积（如触发器、碰撞体）的大小。

#### 属性

- `UPROPERTY(...) FVector2D XAxis {0.0f, 0.0f};`: X 轴方向的边距。`FVector2D` 的 X 分量代表正 X 方向的扩展，Y 分量代表负 X 方向的扩展。
- `UPROPERTY(...) FVector2D YAxis {0.0f, 0.0f};`: Y 轴方向的边距。
- `UPROPERTY(...) FVector2D ZAxis {0.0f, 0.0f};`: Z 轴方向的边距。

#### 方法

- `FBoxCenterAndExtent Apply(const FBoxCenterAndExtent& Bounds) const;`: 核心方法。接受一个边界框 (`FBoxCenterAndExtent`)，根据 `FMargin3D` 的设置计算并返回一个新的、经过扩展或收缩的边界框。