# Margin3DCustomization.h

## 概述

`Margin3DCustomization.h` 定义了 `FMargin3DCustomization` 类，该类继承自 `IPropertyTypeCustomization`。它的主要作用是在 Unreal Engine 编辑器的细节面板（Details Panel）中为 `FMargin3D` 结构体提供自定义的属性显示和编辑界面。

## 类定义

### FMargin3DCustomization

此类继承自 `IPropertyTypeCustomization`，用于完全控制 `FMargin3D` 结构体在编辑器中的显示方式。

#### 方法

- `static TSharedRef<IPropertyTypeCustomization> MakeInstance()`: 静态工厂方法，用于创建 `FMargin3DCustomization` 的实例。这个方法会被编辑器调用以获取自定义属性类型的实例。
- `virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override`: 重写基类方法，用于自定义结构体在细节面板中的“头部”（Header）行，即包含属性名称和主要编辑控件的那一行。这是实现自定义 UI 的核心方法。
- `virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override`: 重写基类方法，用于自定义结构体子属性的显示。如果结构体的子属性需要在主属性下方展开显示，可以在这里实现。在此实现中，该方法为空，因为所有编辑控件都在 `CustomizeHeader` 中创建了。