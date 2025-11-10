# Margin3DCustomization.cpp

## 概述

`Margin3DCustomization.cpp` 是 `FMargin3DCustomization` 类的实现文件。它实现了在 Unreal Engine 编辑器的细节面板中自定义 `FMargin3D` 结构体显示和编辑逻辑的具体方法。

## 依赖项

- `Margin3DCustomization.h`: 类的声明。
- `ProceduralDungeonEditorSettings.h`: 可能用于获取默认设置（尽管在此文件中未直接使用）。
- `PropertyCustomizationHelpers.h`: 提供属性定制的辅助函数。

## 实现细节

### MakeInstance

- `FMargin3DCustomization::MakeInstance()`: 返回一个 `FMargin3DCustomization` 实例的共享引用。这是编辑器用来获取自定义属性类型实例的标准工厂方法。

### CustomizeHeader

- `FMargin3DCustomization::CustomizeHeader()`: 此方法负责构建 `FMargin3D` 属性在细节面板中的完整显示行。
  1.  **获取子属性句柄**: 它首先获取 `FMargin3D` 结构体的 `XAxis`, `YAxis`, `ZAxis` 三个 `FVector2D` 类型子属性的句柄。
  2.  **创建基础布局**: 它创建一个水平框 (`SHorizontalBox`) 作为主要的值内容区域 (`ValueContent`)。
  3.  **为每个轴构建 UI**:
      *   对于 `XAxis`, `YAxis`, `ZAxis` 中的每一个:
          *   获取该轴的 `FVector2D` 的 `X` (正向) 和 `Y` (负向) 子属性句柄。
          *   在水平框中添加一个槽位 (Slot) 来显示轴的名称 (例如 "XAxis")。
          *   如果存在正向 (`X`) 属性句柄，则添加另一个槽位来显示其对应的属性值编辑控件 (通常是数值输入框)。
          *   如果存在负向 (`Y`) 属性句柄，则再添加一个槽位来显示其对应的属性值编辑控件。
  4.  **布局效果**: 最终的 UI 会将 `FMargin3D` 的所有六个数值 (X+, X-, Y+, Y-, Z+, Z-) 在同一行内水平排列，每个轴的名称后紧跟其正负方向的输入框，并在它们之间添加适当的间距。

### CustomizeChildren

- `FMargin3DCustomization::CustomizeChildren()`: 此方法当前为空实现。因为所有必要的 UI 元素都在 `CustomizeHeader` 中创建并显示了，所以不需要在此处添加额外的子控件。