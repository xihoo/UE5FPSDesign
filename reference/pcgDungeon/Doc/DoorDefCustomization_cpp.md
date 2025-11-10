# DoorDefCustomization.cpp

## 概述

`DoorDefCustomization.cpp` 是 `FDoorDefCustomization` 类的实现文件。它实现了在 Unreal Engine 编辑器的细节面板中自定义 `FDoorDef` 结构体显示和编辑逻辑的具体方法。

## 依赖项

- `DoorDefCustomization.h`: 类的声明。
- `ProceduralDungeonTypes.h`: 包含 `FDoorDef` 结构体的定义。
- `PropertyCustomizationHelpers.h`: 提供属性定制的辅助函数。

## 实现细节

### MakeInstance

- `FDoorDefCustomization::MakeInstance()`: 返回一个 `FDoorDefCustomization` 实例的共享引用。这是编辑器用来获取自定义属性类型实例的标准工厂方法。

### GetSortedChildren

- `FDoorDefCustomization::GetSortedChildren()`: 此方法负责定义 `FDoorDef` 结构体在细节面板中的子属性排列。
  1.  它首先获取 `Position` (FVector), `Direction` (EDoorDirection枚举), 和 `Type` (TSoftObjectPtr<UDoorType>) 三个子属性的 `IPropertyHandle`。
  2.  对于 `Position` 属性（一个 FVector 结构体），它会手动获取其 `X`, `Y`, `Z` 三个子属性的句柄，并按 `X`, `Y`, `Z` 的顺序将它们添加到 `OutChildren` 数组中。这使得 FVector 的组件可以在同一行内编辑，而不是默认的折叠显示。
  3.  接着，它将 `Direction` 和 `Type` 属性的句柄也添加到 `OutChildren` 数组中。
  4.  最终，`FDoorDef` 在细节面板中的显示顺序将是：Position 的 X, Y, Z 输入框，然后是 Direction 下拉框，最后是 Type 资产选择器。

### MakeChildWidget

- `FDoorDefCustomization::MakeChildWidget()`: 此方法为 `GetSortedChildren` 中定义的每个子属性创建 UI 控件。
  1.  它检查属性的类型（枚举、结构体、对象）。
  2.  对于这些类型，它调用 `PropertyHandle->CreatePropertyValueWidget()` 来创建默认的编辑控件（如下拉菜单、结构体展开区域、资产选择器等）。
  3.  对于其他类型的属性（主要是数值类型，如 int, float，它们由 `FMathStructCustomization` 处理），它会调用基类 `FMathStructCustomization::MakeChildWidget` 来创建标准的数值输入控件。