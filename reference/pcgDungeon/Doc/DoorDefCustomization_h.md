# DoorDefCustomization.h

## 概述

`DoorDefCustomization.h` 定义了 `FDoorDefCustomization` 类，该类继承自 `FMathStructCustomization`。它的主要作用是在 Unreal Engine 编辑器的细节面板（Details Panel）中为 `FDoorDef` 结构体提供自定义的属性显示和编辑界面。

## 类定义

### FDoorDefCustomization

此类继承自 `FMathStructCustomization`，用于自定义 `FDoorDef` 结构体在编辑器中的显示方式。

#### 方法

- `static TSharedRef<IPropertyTypeCustomization> MakeInstance()`: 静态工厂方法，用于创建 `FDoorDefCustomization` 的实例。这个方法会被编辑器调用以获取自定义属性类型的实例。
- `virtual void GetSortedChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, TArray<TSharedRef<IPropertyHandle>>& OutChildren) override`: 重写基类方法，用于定义 `FDoorDef` 结构体子属性在细节面板中的显示顺序和方式。它会获取 `Position` (一个 FVector)、`Direction` 和 `Type` 子属性的句柄，并将它们按特定顺序添加到 `OutChildren` 数组中。对于 `Position` 属性，它会进一步展开其 `X`, `Y`, `Z` 子属性以实现内联编辑。
- `virtual TSharedRef<SWidget> MakeChildWidget(TSharedRef<IPropertyHandle>& StructurePropertyHandle, TSharedRef<IPropertyHandle>& PropertyHandle) override`: 重写基类方法，用于为特定的子属性创建自定义的 UI 控件（SWidget）。对于枚举、结构体和对象属性，它会创建默认的属性值控件。对于其他类型的属性，它会调用基类 `FMathStructCustomization` 的实现。