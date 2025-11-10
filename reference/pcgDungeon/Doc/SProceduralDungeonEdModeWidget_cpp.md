# SProceduralDungeonEdModeWidget.cpp

## 概述

`SProceduralDungeonEdModeWidget.cpp` 是 `SProceduralDungeonEdModeWidget` 类的实现文件。它包含了构建复杂 UI 布局的逻辑、处理用户交互、与编辑器数据和子系统进行交互的所有细节。

## 依赖项

- `SProceduralDungeonEdModeWidget.h`: 类的声明。
- 大量的 Slate UI 头文件 (如 `SVerticalBox.h`, `SButton.h`, `SSpinBox.h` 等)。
- `EditorModeManager.h`, `EditorStyleSet.h`, `ISinglePropertyView.h`, `AssetEditorSubsystem.h`: 编辑器模块和子系统。
- `FileHelpers.h`, `KismetEditorUtilities.h`: 用于保存文件和编译蓝图。
- `Engine/LevelScriptBlueprint.h`, `GameFramework/Volume.h`, `Builders/CubeBuilder.h`: 关卡、体积和构建器相关类。
- `Engine/Selection.h`: 管理编辑器中的 Actor 选择。
- `ProceduralDungeonEdLog.h`: 编辑器日志宏。
- `ProceduralDungeonEditor.h`, `ProceduralDungeonEdMode.h`, `ProceduralDungeonEdModeToolkit.h`, `ProceduralDungeonEditorSettings.h`: 插件相关的类和设置。
- `Tools/ProceduralDungeonEditorTool.h`: 编辑器工具基类。
- `Room.h`, `RoomLevel.h`, `RoomData.h`: 地牢核心类。

## 实现细节

### Construct

- `Construct(...)`: 这是 Widget 构建的核心。
  - **初始化**: 保存 `ParentToolkit`，获取 `FProceduralDungeonEdMode` 和关卡名称。
  - **字体设置**: 定义标题和副标题的字体样式。
  - **获取默认设置**: 从 `UProceduralDungeonEditorSettings` 获取 `VolumeMargins` 的默认值。
  - **UI 布局构建**:
    - 使用 `SNew(SVerticalBox)` 作为根布局。
    - **关卡名称显示**: 添加一个居中的大标题文本块显示关卡名称。
    - **关卡属性编辑器**: 添加一个 `SBorder` (`LevelPropertyContainer`)，其内容将动态设置为 `ARoomLevel` 的 "Data" 属性的单属性编辑器。其可见性由 `ShowDetails` 控制。
    - **错误信息显示**: 添加一个 `SErrorText` (`Error`) 用于显示错误。
    - **数据资产详情和工具**:
      - 一个带背景的 `SBorder`，其可见性由 `ShowDataDetails` 控制。
      - **标题栏**: 使用 `SOverlay` 创建一个包含数据资产名称（由 `GetDataAssetName` 提供）、"Edit" 按钮和 "Save" 按钮的行。
        - "Edit" 按钮 (`EditData`) 的启用状态由 `IsValidRoomData` 控制。
        - "Save" 按钮 (`SaveData`) 的启用状态由 `IsDataDirty` 控制，颜色由 `GetSaveButtonColor` 控制。
      - **属性滚动区域**: 添加一个 `SScrollBox` (`DataScrollBox`)。
      - **工具区域 ("Utilities")**:
        - **"Update Selected Volumes" 按钮**: (`UpdateSelectedVolumes`)，其启用状态由 Lambda 表达式 `[this]() { return SelectedVolumeCount > 0; }` 控制。
        - **边距设置 (Margins)**: 一系列 `STextBlock` 标签和 `SSpinBox<float>` 输入框，用于设置 X, Y, Z 轴的正负边距。`SSpinBox` 的值变化通过 Lambda 表达式直接更新 `VolumeMargins` 成员变量。
        - **"Remove All Invalid Doors" 按钮**: (`RemoveInvalidDoors`)。
    - **"Reparent" 提示**: 一个 `SBorder`，其可见性由 `ShowNote` 控制，包含 "Reparent Level Blueprint" 按钮 (`ReparentLevelActor`)，按钮颜色由 `GetReparentButtonColor` 控制。
  - **创建数据属性视图**: 使用 `FPropertyEditorModule` 创建一个 `IDetailsView` (`DataContentWidget`)，用于显示 `URoomData` 的所有属性。将其添加到 `DataScrollBox` 中。为 `DataContentWidget` 的 `OnFinishedChangingProperties` 事件添加 Lambda 回调 `UpdateErrorText`。
  - **初始化**: 调用 `OnLevelChanged()` 初始化 UI 内容。调用 `RegisterSelectionDelegate(true)` 注册选择变化委托，并立即调用 `OnSelectedActorsChanged(nullptr)` 更新 `SelectedVolumeCount`。

### 析构函数

- `~SProceduralDungeonEdModeWidget()`: 注销选择委托，重置对 `CachedData` 和 `CachedLevel` 的缓存。

### OnLevelChanged

- `OnLevelChanged()`: 当编辑的关卡改变时调用。
  - 获取 `FProceduralDungeonEdMode`。
  - 调用 `ResetCachedLevel()` 清除旧的关卡缓存。
  - 调用 `IsValidRoomLevel` 检查并获取新的 `ARoomLevel` 实例到 `CachedLevel`。
  - 如果无效，则调用 `UpdateErrorText()` 并返回。
  - 为 `CachedLevel` 的 `OnPropertiesChanged` 事件添加 Lambda 回调，当关卡属性改变时调用 `OnDataAssetChanged`。
  - 使用 `FPropertyEditorModule::CreateSingleProperty` 为 `CachedLevel` 的 "Data" 属性创建一个单属性编辑器，并将其设置为 `LevelPropertyContainer` 的内容。
  - 调用 `OnDataAssetChanged()` 初始化数据相关的 UI。

### OnDataAssetChanged

- `OnDataAssetChanged()`: 当关卡的 `Data` 属性改变时调用。
  - 获取 `FProceduralDungeonEdMode`。
  - 调用 `ResetCachedData()` 清除旧的数据缓存。
  - 调用 `IsValidRoomData` 检查并获取新的 `URoomData` 实例到 `CachedData`。
  - 如果数据有效：
    - 调用 `DataContentWidget->SetObject(CachedData.Get())` 将数据对象设置到属性视图中进行显示。
    - 检查 `CachedData->Level` 是否为空，如果为空则将其设置为当前编辑的世界，并记录日志。
    - 为 `CachedData` 的 `OnPropertiesChanged` 事件添加 Lambda 回调，当数据属性改变时调用 `UpdateErrorText`。
  - 调用 `UpdateErrorText()` 更新错误状态。
  - 调用 `EdMode->SetDefaultTool()` 尝试设置默认工具。
  - 获取当前激活的工具，并调用其 `OnDataChanged(CachedData.Get())` 通知工具数据已更新。

### 按钮回调方法

- `ReparentLevelActor()`: 
  - 获取当前世界的关卡蓝图。
  - 将其 `ParentClass` 设置为 `ARoomLevel::StaticClass()`。
  - 使用 `FKismetEditorUtilities::CompileBlueprint` 编译蓝图。
  - 调用 `EdMode->UpdateLevelBlueprint()` 更新编辑器模式内部对关卡蓝图的缓存。
- `EditData()`: 
  - 获取 `UAssetEditorSubsystem`。
  - 检查并获取当前有效的 `URoomData`。
  - 调用 `AssetEditorSubsystem->OpenEditorForAsset(Data.Get())` 打开数据资产的编辑器。
- `SaveData()`: 
  - 检查并获取当前有效的 `URoomData`。
  - 调用 `FEditorFileUtils::PromptForCheckoutAndSave` 尝试检出并保存数据资产所在的包。
- `UpdateSelectedVolumes()`: 
  - 检查并获取 `URoomData` 和 `ARoomLevel`。
  - 根据 `URoomData` 的边界和 `VolumeMargins` 计算目标体积边界。
  - 开始编辑器事务。
  - 遍历当前选中的所有 Actor。
  - 如果 Actor 是 `AVolume` 且其 `BrushBuilder` 是 `UCubeBuilder`：
    - 修改 Volume Actor 和其 CubeBuilder。
    - 设置 Volume 的位置为中心点。
    - 设置 CubeBuilder 的 X, Y, Z 尺寸为边长的两倍。
    - 调用 `CubeBuilder->Build` 重新构建体积。
  - 结束编辑器事务。
- `RemoveInvalidDoors()`: 
  - 检查并获取当前有效的 `URoomData`。
  - 开始编辑器事务。
  - 遍历 `URoomData->Doors` 数组（从后往前）。
  - 如果门定义无效 (`Data->IsDoorValid(i)`) 或重复 (`Data->IsDoorDuplicate(i)`)，则将其从数组中移除。
  - 结束编辑器事务。

### UI 状态与辅助方法

- `GetSaveButtonColor()`: 如果数据脏了，返回一个动态计算的高亮绿色；否则返回白色。
- `GetReparentButtonColor()`: 返回一个动态计算的高亮绿色。
- `UpdateErrorText()`: 按优先级检查各种错误条件（关卡无效、数据无效、数据与关卡不匹配），并通过 `Error->SetError` 显示相应的错误信息或清空错误。
- `ResetCachedData()`: 如果 `CachedData` 有效，则移除其属性变化委托并重置指针。
- `ResetCachedLevel()`: 如果 `CachedLevel` 有效，则移除其属性变化委托并重置指针。
- `GetEditorMode()`: 通过 `ParentToolkit` 获取 `FProceduralDungeonEdMode` 实例。
- `RegisterSelectionDelegate(bool)`: 注册或注销 `GEditor->GetSelectedActors()->SelectionChangedEvent` 委托，回调函数是 `OnSelectedActorsChanged`。
- `OnSelectedActorsChanged(UObject*)`: 更新 `SelectedVolumeCount` 为当前选中的 `AVolume` 数量。
- `IsValidRoomLevel(...)`: 检查 `EdMode->GetLevel()` 是否返回一个有效的 `ARoomLevel`。
- `IsValidRoomData(...)`: 先检查关卡是否有效，再检查关卡的 `Data` 属性是否有效。
- `MatchingDataLevel(...)`: 检查 `URoomData->Level` 的路径是否与当前世界路径匹配。
- `IsDataDirty(...)`: 检查 `URoomData` 所在的 `UPackage` 是否为脏。
- `ShowDetails()`: 如果关卡有效则显示，否则隐藏。
- `ShowDataDetails()`: 如果数据有效则显示，否则隐藏。
- `ShowNote()`: 如果关卡无效则显示（提示需要 Reparent），否则隐藏。
- `GetDataAssetName()`: 返回数据资产名称，如果脏了则加上 "*"。
- `GetHighlightButtonColor(...)`: 静态工具函数，使用余弦函数和当前时间计算一个在 `NormalColor` 和 `HighlightColor` 之间插值的颜色，产生闪烁效果。