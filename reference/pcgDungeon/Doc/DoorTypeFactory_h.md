# DoorTypeFactory.h

## 概述

`DoorTypeFactory.h` 定义了 `UDoorTypeFactory` 类。这是一个 Unreal Engine 的 `UFactory` 子类，用于在编辑器的“内容浏览器”(Content Browser) 中创建新的 `UDoorType` 资产。它通过右键菜单的“新建资产”(New Asset) 选项，提供一个创建 `UDoorType` 的入口。

## 类定义

### UDoorTypeFactory

此类继承自 `UFactory`，专门用于创建 `UDoorType` 对象。

#### 构造函数

- `UDoorTypeFactory()`: 构造函数，设置工厂的基本属性。
  - `bCreateNew = true;`: 表明这是一个创建新对象的工厂。
  - `bEditAfterNew = true;`: 表明创建新对象后应立即在属性编辑器中打开进行编辑。
  - `SupportedClass = UDoorType::StaticClass();`: 指定此工厂支持创建的类是 `UDoorType`。

#### 重写的基类方法

- `virtual uint32 GetMenuCategories() const override;`: 返回此工厂在“新建资产”菜单中应归属的分类。此实现从 `FProceduralDungeonEditorModule` 获取插件注册的分类。
- `virtual UObject* FactoryCreateNew(...) override;`: 核心的创建函数。当用户通过此工厂创建新资产时，会调用此函数来实际生成 `UDoorType` 对象实例。