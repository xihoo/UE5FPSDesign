# AssetTypeActions_DoorType.h

## 概述

`AssetTypeActions_DoorType.h` 定义了 `FAssetTypeActions_DoorType` 类，该类继承自 `FAssetTypeActions_Base`。它的主要作用是在 Unreal Engine 编辑器的“内容浏览器”中为 `UDoorType` 资产类型提供自定义的显示和行为，例如资产的名称、颜色和分类。

## 类定义

### FAssetTypeActions_DoorType

此类继承自 `FAssetTypeActions_Base`，用于在编辑器中注册和管理 `UDoorType` 资产的行为。

#### 方法

- `FAssetTypeActions_DoorType()`: 构造函数。
- `GetName() const override`: 返回在内容浏览器中显示的资产类型的名称，这里是 "Door Type"。
- `GetSupportedClass() const override`: 返回此资产类型动作支持的 UClass，即 `UDoorType::StaticClass()`。
- `GetTypeColor() const override`: 返回资产在内容浏览器中显示的颜色，这里定义为 `FColor(255, 50, 128)`。
- `GetCategories() override`: 返回资产所属的分类。此实现返回 `EAssetTypeCategories::None`，意味着分类将在工厂中定义。
- `HasActions(const TArray<UObject*>& InObjects) const override`: 指示此资产类型是否具有可以在内容浏览器右键菜单中执行的额外操作。此实现返回 `false`，表示没有额外操作。