# DoorTypeFactory.cpp

## 概述

`DoorTypeFactory.cpp` 是 `UDoorTypeFactory` 类的实现文件。它包含了创建 `UDoorType` 资产的具体逻辑。

## 依赖项

- `DoorTypeFactory.h`: 类的声明。
- `DoorType.h`: 要创建的 `UDoorType` 类。
- `AssetTypeCategories.h`: 资产分类枚举。
- `ProceduralDungeonEditor.h`: 获取插件模块以访问资产分类。

## 实现细节

### 构造函数

- `UDoorTypeFactory::UDoorTypeFactory()`: 构造函数。
  - `bCreateNew = true;`: 设置为 `true`，表示此工厂用于创建全新的对象。
  - `bEditAfterNew = true;`: 设置为 `true`，表示创建后应立即打开进行编辑。
  - `SupportedClass = UDoorType::StaticClass();`: 指定工厂支持创建 `UDoorType` 类的实例。

### GetMenuCategories

- `uint32 UDoorTypeFactory::GetMenuCategories() const`: 
  - 通过 `FModuleManager::LoadModuleChecked` 获取 `FProceduralDungeonEditorModule` 的引用。
  - 调用 `EditorModule.GetAssetTypeCategory()` 返回插件在内容浏览器中注册的资产分类。这确保了 `DoorType` 资产会出现在 "Procedural Dungeon" 分类下。

### FactoryCreateNew

- `UObject* UDoorTypeFactory::FactoryCreateNew(...)`: 
  - 使用 `NewObject<UDoorType>` 模板函数创建一个新的 `UDoorType` 对象实例。
  - 参数:
    - `InParent`: 新对象的父对象（通常是包或另一个对象）。
    - `InClass`: 要创建的对象的类（应为 `UDoorType::StaticClass()`）。
    - `InName`: 新对象的名称。
    - `Flags`: 应用于新对象的 `EObjectFlags`（如 `RF_Transactional`）。
  - 返回新创建的 `UDoorType` 对象指针。编辑器会接管这个对象并将其保存为资产。