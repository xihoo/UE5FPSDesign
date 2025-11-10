# ProceduralDungeonEditor/Private/ProceduralDungeonEditor.cpp

## 概述

`ProceduralDungeonEditor.cpp` 是 ProceduralDungeon 插件编辑器模块的主要实现文件。它负责在 Unreal Editor 启动时初始化插件的所有编辑器相关功能，并在插件关闭时进行清理。这包括注册资产类型、自定义属性编辑器、Slate 样式、编辑器模式以及项目设置等。

## 代码解析

### `StartupModule` 函数

```cpp
void FProceduralDungeonEditorModule::StartupModule()
```

- **功能**: 模块启动时调用，执行所有必要的初始化。
- **主要逻辑**:

    1.  **注册设置**:
        ```cpp
        RegisterSettings();
        ```
        调用 `RegisterSettings` 函数来注册插件的项目设置。

    2.  **注册编辑器命令**:
        ```cpp
        FProceduralDungeonEditorCommands::Register();
        ```
        注册插件定义的自定义编辑器命令（快捷键等）。

    3.  **注册资产类型**:
        ```cpp
        IAssetTools& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
        AssetTypeCategory = AssetToolsModule.RegisterAdvancedAssetCategory(...);
        TSharedPtr<FAssetTypeActions_RoomData> RoomDataAction = MakeShareable(new FAssetTypeActions_RoomData());
        AssetToolsModule.RegisterAssetTypeActions(...);
        ```
        -   加载 `AssetTools` 模块。
        -   注册一个名为 "Procedural Dungeon" 的新资产分类。
        -   为 `URoomData` 和 `UDoorType` 资产类型创建并注册 `AssetTypeActions`，使它们能在内容浏览器中被正确识别、创建和管理。

    4.  **注册属性自定义**:
        ```cpp
        FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
        PropertyModule.RegisterCustomPropertyTypeLayout(...);
        PropertyModule.NotifyCustomizationModuleChanged();
        ```
        -   加载 `PropertyEditor` 模块。
        -   为 `FDoorDef` 和 `FMargin3D` 结构体注册自定义的细节面板（Details Panel）编辑器，以提供更友好的编辑界面。
        -   通知属性编辑器模块自定义项已更改。

    5.  **注册 Slate 样式**:
        ```cpp
        StyleSet = MakeShareable(new FSlateStyleSet("ProceduralDungeonStyle"));
        ... // 设置内容根目录和图标
        FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);
        ```
        -   创建一个新的 `FSlateStyleSet` 来管理插件的 UI 样式。
        -   设置样式集的内容根目录（通常指向插件目录）。
        -   定义并设置多个图标资源（如编辑器模式图标、工具图标）。
        -   将样式集注册到全局 `FSlateStyleRegistry` 中，使其在整个编辑器中可用。

    6.  **注册编辑器模式**:
        ```cpp
        FSlateIcon ModeIcon = FSlateIcon("ProceduralDungeonStyle", "ProceduralDungeon.Icon", "ProceduralDungeon.Icon.Small");
        FEditorModeRegistry::Get().RegisterMode<FProceduralDungeonEdMode>(...);
        ```
        -   使用之前注册的样式创建一个图标。
        -   向 `FEditorModeRegistry` 注册一个新的编辑器模式 `FProceduralDungeonEdMode`，命名为 "Dungeon Room"。这使得用户可以在编辑器的模式面板中激活此模式，以获得特定于地牢房间编辑的工具和功能。

### `ShutdownModule` 函数

```cpp
void FProceduralDungeonEditorModule::ShutdownModule()
```

- **功能**: 模块关闭时调用，执行所有必要的清理工作。
- **主要逻辑**:

    1.  **注销编辑器命令**:
        ```cpp
        FProceduralDungeonEditorCommands::Unregister();
        ```
        注销之前注册的自定义编辑器命令。

    2.  **注销属性自定义**:
        ```cpp
        if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
        {
            FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
            PropertyModule.UnregisterCustomPropertyTypeLayout(...);
        }
        ```
        检查 `PropertyEditor` 模块是否已加载，如果是，则注销为 `FDoorDef` 和 `FMargin3D` 注册的自定义布局。

    3.  **注销编辑器模式**:
        ```cpp
        FEditorModeRegistry::Get().UnregisterMode(FProceduralDungeonEdMode::EM_ProceduralDungeon);
        ```
        从 `FEditorModeRegistry` 中注销 "Dungeon Room" 编辑器模式。

    4.  **注销设置**:
        ```cpp
        if (UObjectInitialized())
        {
            UnregisterSettings();
        }
        ```
        检查 UObject 系统是否已初始化，如果是，则调用 `UnregisterSettings` 注销项目设置。

### `RegisterSettings` 函数

```cpp
void FProceduralDungeonEditorModule::RegisterSettings()
```

- **功能**: 将插件的配置类 `UProceduralDungeonEditorSettings` 注册到编辑器的项目设置（Project Settings）中。
- **逻辑**:
    1.  获取 `ISettingsModule` 指针。
    2.  调用 `RegisterSettings`，指定设置位于 "Project" -> "Editor" -> "Procedural Dungeon" 路径下，并关联 `UProceduralDungeonEditorSettings` 类的默认对象。
    3.  如果注册成功，绑定 `OnModified` 事件到 `HandleSettingsSaved` 函数，以便在用户修改并保存设置时得到通知。

### `UnregisterSettings` 函数

```cpp
void FProceduralDungeonEditorModule::UnregisterSettings()
```

- **功能**: 从编辑器的项目设置中注销插件的配置。
- **逻辑**: 获取 `ISettingsModule` 指针并调用 `UnregisterSettings`。

### `HandleSettingsSaved` 函数

```cpp
bool FProceduralDungeonEditorModule::HandleSettingsSaved()
```

- **功能**: 当用户在编辑器中保存插件设置时被调用。
- **逻辑**:
    1.  获取 `UProceduralDungeonEditorSettings` 的可变默认对象。
    2.  （理论上）检查设置是否有需要特殊处理的更改。
    3.  如果有（`ResaveSettings` 为 true），则调用 `SaveConfig()` 保存配置。
    4.  返回 `true`。
    -   **注意**: 当前实现中的检查逻辑是空的，`ResaveSettings` 始终为 `false`。

### 宏定义和模块实现

```cpp
#define LOCTEXT_NAMESPACE "FProceduralDungeonEditorModule"
...
#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FProceduralDungeonEditorModule, ProceduralDungeonEditor)
```

- **`LOCTEXT_NAMESPACE`**: 为源文件定义本地化文本命名空间，用于包裹 `LOCTEXT` 宏。
- **`IMPLEMENT_MODULE`**: 这是 Unreal Engine 模块系统的标准宏，用于指定 `FProceduralDungeonEditorModule` 是 `ProceduralDungeonEditor` 模块的入口点类。

## 关键概念

- **模块生命周期管理**: 通过 `StartupModule` 和 `ShutdownModule` 精确控制插件在编辑器中的初始化和清理。
- **资产类型集成**: 通过 `AssetTypeActions` 将自定义资产（如 `URoomData`）无缝集成到内容浏览器中。
- **UI 自定义**: 利用 `PropertyEditor` 模块和 `FSlateStyleSet` 提供定制化的编辑器用户体验。
- **编辑器模式**: 通过注册 `FEditorMode` 提供专门的编辑工具和上下文。
- **项目设置**: 通过 `ISettingsModule` 允许用户通过标准的编辑器 UI 配置插件行为。