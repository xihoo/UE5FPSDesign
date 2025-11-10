# SProceduralDungeonEdModeWidget.h

## 概述

`SProceduralDungeonEdModeWidget.h` 定义了 `SProceduralDungeonEdModeWidget` 类。这是一个 Slate UI Widget，构成了“地牢房间编辑模式”用户界面的主要部分。它嵌入在 `FProceduralDungeonEdModeToolkit` 中，负责显示关卡信息、`URoomData` 资产的属性编辑器、错误信息以及提供一系列与地牢编辑相关的实用工具按钮。

## 类定义

### SProceduralDungeonEdModeWidget

此类继承自 `SCompoundWidget`，是编辑模式的主 UI 界面。

#### Slate 构造宏

- `SLATE_BEGIN_ARGS` / `SLATE_END_ARGS`: 用于定义 Slate Widget 的构造参数。此 Widget 没有自定义构造参数。

#### 构造与析构

- `void Construct(...)`: Slate Widget 的主要构造函数，用于构建整个 UI 布局。
- `~SProceduralDungeonEdModeWidget()`: 析构函数，负责清理注册的委托。

#### 公共方法

- `void OnLevelChanged()`: 当编辑的关卡发生变化时调用，用于更新 UI 内容。

#### 受保护方法

这些方法主要用于 UI 逻辑、数据验证和按钮回调。

- **数据验证与查询**:
  - `bool IsValidRoomLevel(...)`: 检查当前关卡是否是有效的 `ARoomLevel`。
  - `bool IsValidRoomData(...)`: 检查当前关卡是否有关联的有效 `URoomData`。
  - `bool IsValidRoomData()`: `IsValidRoomData` 的便捷重载。
  - `bool MatchingDataLevel(...)`: 检查 `URoomData` 中记录的关卡路径是否与当前编辑的世界匹配。
  - `bool IsDataDirty(...)`: 检查关联的 `URoomData` 资产是否已被修改（脏）。
  - `bool IsDataDirty()`: `IsDataDirty` 的便捷重载。
  - `EVisibility ShowDetails()`: 根据关卡有效性决定是否显示关卡属性部分。
  - `EVisibility ShowDataDetails()`: 根据数据有效性决定是否显示数据属性和工具部分。
  - `EVisibility ShowNote()`: 根据关卡有效性决定是否显示“Reparent”提示。
  - `FText GetDataAssetName()`: 获取 `URoomData` 资产的名称，如果脏了会加上 "*"。

- **UI 更新与辅助**:
  - `void OnDataAssetChanged()`: 当关卡的 `Data` 属性改变时调用，更新数据属性编辑器和相关状态。
  - `void UpdateErrorText()`: 根据当前状态更新错误信息显示。
  - `void ResetCachedData()`: 重置对 `URoomData` 的缓存和委托。
  - `void ResetCachedLevel()`: 重置对 `ARoomLevel` 的缓存和委托。
  - `FProceduralDungeonEdMode* GetEditorMode()`: 获取关联的 `FProceduralDungeonEdMode` 实例。
  - `void RegisterSelectionDelegate(bool Register)`: 注册或注销编辑器 Actor 选择变化的委托。
  - `void OnSelectedActorsChanged(UObject* NewSelectedObject)`: 当选择的 Actor 发生变化时调用，用于更新 `SelectedVolumeCount`。
  - `static FLinearColor GetHighlightButtonColor(...)`: 静态方法，用于计算按钮的高亮颜色（基于时间的淡入淡出效果）。

- **按钮回调**:
  - `FReply ReparentLevelActor()`: “Reparent Level Blueprint”按钮的回调，将当前关卡蓝图的父类设置为 `ARoomLevel`。
  - `FReply EditData()`: “Edit”按钮的回调，打开 `URoomData` 资产的编辑器。
  - `FReply SaveData()`: “Save”按钮的回调，保存 `URoomData` 资产。
  - `FReply UpdateSelectedVolumes()`: “Update Selected Volumes”按钮的回调，根据房间边界调整选中体积的大小和位置。
  - `FReply RemoveInvalidDoors()`: “Remove All Invalid Doors”按钮的回调，移除 `URoomData` 中所有无效或重复的门定义。
  - `FSlateColor GetSaveButtonColor()`: 获取“Save”按钮的颜色，如果数据脏了会高亮显示。
  - `FSlateColor GetReparentButtonColor()`: 获取“Reparent”按钮的颜色，会高亮显示。

#### 私有成员

- `TSharedPtr<class SErrorText> Error`: 指向显示错误信息的 Widget。
- `TSharedPtr<class IDetailsView> DataContentWidget`: 指向显示 `URoomData` 属性的详细信息视图。
- `TWeakPtr<FProceduralDungeonEdModeToolkit> ParentToolkit`: 指向父级工具包的弱指针。
- `FMargin3D VolumeMargins`: 存储用于 "Update Selected Volumes" 功能的边距设置。
- `TSharedPtr<class SBorder> LevelPropertyContainer`: 包含关卡 `Data` 属性编辑器的容器。
- `TWeakObjectPtr<URoomData> CachedData`: 缓存当前的 `URoomData` 对象。
- `TWeakObjectPtr<ARoomLevel> CachedLevel`: 缓存当前的 `ARoomLevel` 对象。
- `FDelegateHandle DataDelegateHandle`: `URoomData` 属性变化委托的句柄。
- `FDelegateHandle LevelDelegateHandle`: `ARoomLevel` 属性变化委托的句柄。
- `FDelegateHandle SelectionDelegateHandle`: 编辑器选择变化委托的句柄。
- `int32 SelectedVolumeCount`: 当前选中的 `AVolume` Actor 的数量。