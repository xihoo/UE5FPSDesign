# ProceduralDungeonEditor/Private/ProceduralDungeonEditorSettings.h

## 概述

`ProceduralDungeonEditorSettings.h` 定义了 ProceduralDungeon 插件在 Unreal Editor 中的可配置设置。这些设置允许用户自定义插件在编辑器中的行为，例如创建 `URoomData` 资产时的默认类，以及 "Room Editor Mode" 下的一些特定功能。该类继承自 `UObject`，并通过 `UCLASS(Config = Editor, DefaultConfig)` 宏标记，表明其属性可以被保存到编辑器的配置文件中。

## 代码解析

### 类声明: `UProceduralDungeonEditorSettings`

```cpp
UCLASS(Config = Editor, DefaultConfig)
class UProceduralDungeonEditorSettings : public UObject
```

- **继承关系**: 公开继承 `UObject`。虽然它是一个 `UObject`，但它不被设计为在关卡中使用或通过蓝图创建实例，更像是一个数据容器。
- **`UCLASS` 宏**:
    - `Config = Editor`: 指示这个类的配置属性将被保存到编辑器的配置文件（通常是 `Editor.ini`）中。
    - `DefaultConfig`: 表示这个类使用默认配置，其属性值将作为默认值提供。

#### 构造函数

```cpp
UProceduralDungeonEditorSettings(const FObjectInitializer& ObjectInitializer);
```

- **功能**: 标准的 `UObject` 构造函数，使用 `FObjectInitializer` 进行初始化。具体的初始化逻辑通常在 `.cpp` 文件中实现。

#### 成员变量 (UPROPERTY)

##### `DefaultRoomDataClass`

```cpp
UPROPERTY(Config, EditAnywhere, Category = "General", NoClear, meta = (AllowAbstract = false))
TSubclassOf<URoomData> DefaultRoomDataClass;
```

- **功能**: 定义在内容浏览器中创建新的 `URoomData` 资产时，"类选择器"（Class Picker）对话框中默认选中的 `URoomData` 子类。
- **元数据解释**:
    - `Config`: 该属性的值会被保存到配置文件中。
    - `EditAnywhere`: 该属性可以在任何地方编辑（例如，在项目设置的 UI 中）。
    - `Category = "General"`: 在编辑器 UI 中，该属性会被归类到 "General" 部分。
    - `NoClear`: 在编辑器 UI 中，该属性的值不能被设置为 "None"（空）。
    - `meta = (AllowAbstract = false)`: 在类选择器中，不允许选择抽象类。

##### `bUseDefaultIfNoChild`

```cpp
UPROPERTY(Config, EditAnywhere, Category = "General", meta = (DisplayName = "Use Automatically Default Class If No Child"))
bool bUseDefaultIfNoChild;
```

- **功能**: 一个布尔标志。如果 `DefaultRoomDataClass` 没有任何子类，并且此选项被勾选，那么在创建新 `URoomData` 资产时，将自动使用 `DefaultRoomDataClass` 而不显示类选择器对话框。
- **元数据解释**:
    - `DisplayName`: 在编辑器 UI 中显示的名称为 "Use Automatically Default Class If No Child"。

##### `DefaultMargins` (注释掉的 `bShowOnlyDefaultAndChildren`)

```cpp
UPROPERTY(Config, EditAnywhere, Category = "Room Editor Mode")
FMargin3D DefaultMargins;
```

- **功能**: 定义在 "Room Editor Mode" 下，用于更新体积（Volumes）的默认边距值。`FMargin3D` 是一个自定义结构体，表示在 X、Y、Z 三个轴向上的边距。
- **注意**: 代码中注释掉了一个名为 `bShowOnlyDefaultAndChildren` 的属性，其注释表明如果启用，类选择器将只显示默认的 `RoomData` 类及其子类。

##### `bUseRoomAsOrbitPivot`

```cpp
UPROPERTY(Config, EditAnywhere, Category = "Room Editor Mode", meta = (DisplayName = "Use the room bounds center as pivot for camera orbit"))
bool bUseRoomAsOrbitPivot {true};
```

- **功能**: 一个布尔标志，控制在 "Room Editor Mode" 激活时，编辑器的摄像机环绕旋转（Orbit）行为。
    - 如果为 `true`（默认值），摄像机将围绕房间边界（Bounds）的中心点旋转。
    - 如果为 `false`，摄像机将围绕当前选中的 Actor 旋转。
- **注意**: 此功能仅在编辑器偏好设置（Editor Preferences）中 "Orbit Camera Around Selection" 选项被勾选时才有效。

## 关键概念

- **可配置编辑器设置**: 通过继承 `UObject` 并使用 `Config` 和 `DefaultConfig`，插件可以提供持久化的、可通过 UI 修改的设置。
- **UPROPERTY 元数据**: `Category`, `DisplayName`, `NoClear`, `meta` 等元数据极大地增强了属性在编辑器 UI 中的呈现和行为。
- **用户体验定制**: 这些设置允许用户根据自己的工作流程调整插件的行为，例如简化资产创建流程或自定义编辑器模式下的交互。
- **与编辑器集成**: 通过 `IProceduralDungeonEditorModule::RegisterSettings`，这些设置被无缝集成到 Unreal Editor 的 "Project Settings" 面板中。