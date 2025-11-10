# AssetTypeActions_RoomData.cpp

## 概述

`AssetTypeActions_RoomData.cpp` 是 `FAssetTypeActions_RoomData` 类的实现文件。它提供了在 Unreal Engine 编辑器中管理 `URoomData` 资产类型所需的具体行为，例如在内容浏览器中显示的名称、颜色和分类。

## 依赖项

- `AssetTypeActions_RoomData.h`: 类的声明。
- `RoomData.h`: `URoomData` 类的定义。
- `Modules/ModuleManager.h`: 用于访问模块管理器。
- `ProceduralDungeonEditor.h`: 获取资产类型分类。

## 实现细节

### 构造函数

- `FAssetTypeActions_RoomData::FAssetTypeActions_RoomData()`: 默认构造函数，未执行任何特殊初始化。

### 方法实现

- `GetName()`: 返回本地化的文本 "Room Data"，作为在内容浏览器中显示的资产类型名称。
- `GetSupportedClass()`: 返回 `URoomData::StaticClass()`，指明此类动作支持 `URoomData` 类型的资产。
- `GetTypeColor()`: 返回 `FColor(255, 50, 128)`，定义了资产在内容浏览器中的显示颜色。
- `GetCategories()`: 返回 `EAssetTypeCategories::None`。注释指出分类将在工厂中定义。
- `HasActions()`: 返回 `false`，表示此资产类型没有额外的右键菜单操作。