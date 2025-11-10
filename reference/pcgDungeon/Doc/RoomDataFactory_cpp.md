# RoomDataFactory.cpp

## 概述

`RoomDataFactory.cpp` 是 `URoomDataFactory` 类的实现文件。它包含了创建 `URoomData` 资产的逻辑，特别是通过类选择器让用户选择具体子类的功能。

## 依赖项

- `RoomDataFactory.h`: 类的声明。
- `RoomData.h`: 要创建的 `URoomData` 类。
- `AssetTypeCategories.h`: 资产分类枚举。
- `ProceduralDungeonEditor.h`: 获取插件模块以访问资产分类。
- `ClassViewerModule.h`, `ClassViewerFilter.h`, `SClassPickerDialog.h`: 用于显示和选择类的编辑器模块。
- `ProceduralDungeonEditorSettings.h`: 获取默认 `RoomData` 类的设置。
- `Misc/EngineVersionComparison.h`: 用于处理不同 UE 版本的 API 差异。

## 实现细节

### FRoomDataClassFilter

- 这是一个内部定义的类，继承自 `IClassViewerFilter`。它用于过滤 `SClassPickerDialog` 中显示的类。
  - **构造函数**: 初始化 `DisallowedClassFlags`，排除抽象类、已弃用类等。
  - **静态成员 `ParentClasses`**: 初始化为包含 `URoomData::StaticClass()` 的集合，确保只显示 `URoomData` 及其子类。
  - **`IsClassAllowed`**: 检查一个已加载的类是否允许显示。它检查类是否没有被禁止的标志，并且是 `URoomData` 的子类。
  - **`IsUnloadedClassAllowed`**: 检查一个未加载的蓝图类是否允许显示，逻辑类似。

### 构造函数

- `URoomDataFactory::URoomDataFactory()`: 构造函数。
  - `bCreateNew = true;`
  - `bEditAfterNew = true;`
  - `SupportedClass = URoomData::StaticClass();`

### GetMenuCategories

- `uint32 URoomDataFactory::GetMenuCategories() const`: 
  - 获取 `FProceduralDungeonEditorModule` 引用。
  - 返回插件的资产分类。

### ConfigureProperties

- `bool URoomDataFactory::ConfigureProperties()`: 这是 `URoomDataFactory` 的核心逻辑。
  - **获取默认设置**: 从 `UProceduralDungeonEditorSettings` 获取 `DefaultRoomDataClass`。
  - **处理默认类为空**: 如果默认类为空，则使用 `URoomData::StaticClass()`。
  - **处理无子类情况**: 如果插件设置 `bUseDefaultIfNoChild` 为 `true`，并且默认类没有子类，则直接将 `RoomDataClass` 设置为默认类并返回 `true`，跳过类选择对话框。
  - **准备类选择器**:
    - 将 `RoomDataClass` 置为 `nullptr`。
    - 加载 `FClassViewerModule`。
    - 创建 `FRoomDataClassFilter` 实例。
    - 配置 `FClassViewerInitializationOptions`:
      - `Mode = EClassViewerMode::ClassPicker;`
      - `InitiallySelectedClass = DefaultClass;`
      - 根据 UE 版本设置 `ClassFilter` 或 `ClassFilters`。
  - **显示对话框**: 使用 `SClassPickerDialog::PickClass` 显示类选择对话框。
  - **处理结果**: 如果用户点击了 "OK"，则将选中的类保存到 `RoomDataClass` 并返回 `true`。否则返回 `false`（取消创建）。

### FactoryCreateNew

- `UObject* URoomDataFactory::FactoryCreateNew(...)`: 
  - 检查 `RoomDataClass` 是否在 `ConfigureProperties` 中被设置。
  - 如果设置了 `RoomDataClass`，则使用 `NewObject<URoomData>` 创建该类的实例，并添加 `RF_Transactional` 标志。
  - 如果未设置（例如 `ConfigureProperties` 返回 `false` 或未被调用），则使用传入的 `InClass`（应为 `URoomData` 或其子类）创建实例。