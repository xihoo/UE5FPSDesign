# ProceduralDungeonEdMode.h

## 概述

`ProceduralDungeonEdMode.h` 定义了 `FProceduralDungeonEdMode` 类。这是“地牢房间编辑模式”的核心类，它继承自 Unreal Engine 的 `FEdMode`。该类负责管理编辑模式的生命周期、处理用户输入、协调工具（如大小调整和门编辑工具）以及与当前编辑的 `ARoomLevel` 和其 `URoomData` 资产的交互。

## 类定义

### FProceduralDungeonEdMode

此类是整个“地牢房间编辑器”的基础。

#### 静态成员

- `const static FEditorModeID EM_ProceduralDungeon`: 定义此编辑器模式的唯一 ID。

#### 构造函数

- `FProceduralDungeonEdMode()`: 构造函数，初始化工具列表和设置对象。

#### FGCObject 接口

- `virtual void AddReferencedObjects(...) override`: 确保 `Settings` 对象在垃圾回收时不会被错误地回收。

#### 重写的 FEdMode 接口

- `virtual void Enter() override`: 当进入此编辑模式时调用。初始化工具包（Toolkit），更新关卡蓝图，并设置 `ARoomLevel::bIsDungeonEditorMode` 标志。
- `virtual void Exit() override`: 当退出此编辑模式时调用。注销委托，关闭工具包，退出当前工具，重置缓存指针，并清除 `ARoomLevel::bIsDungeonEditorMode` 标志。
- `virtual void Render(...) override`: 渲染调用，将渲染任务委托给当前激活的工具。
- `virtual void Tick(...) override`: 每帧调用，将 Tick 任务委托给当前激活的工具。
- `virtual bool HandleClick(...) override`: 处理鼠标点击，委托给当前工具。
- `virtual bool InputKey(...) override`: 处理键盘/鼠标按键，委托给当前工具。
- `virtual bool InputAxis(...) override`: 处理输入轴（如手柄摇杆），委托给当前工具。
- `virtual bool InputDelta(...) override`: 处理视口中的变换操作（拖拽、旋转、缩放），委托给当前工具。
- `virtual bool MouseMove(...) override`: 处理鼠标移动，委托给当前工具。
- `virtual bool ShowModeWidgets() const override`: 返回 `true`，指示应显示此模式的小部件（如工具栏）。
- `virtual bool ShouldDrawWidget() const override`: 返回 `true`，指示应绘制标准的变换控件（移动、旋转、缩放）。
- `virtual bool UsesTransformWidget() const override`: 查询是否使用变换控件，委托给当前工具。
- `virtual bool UsesTransformWidget(WidgetMode CheckMode) const`: 查询是否使用特定模式的变换控件，委托给当前工具。
- `virtual FVector GetWidgetLocation() const override`: 获取变换控件的位置，委托给当前工具。
- `virtual bool AllowWidgetMove() override`: 返回 `true`，允许通过变换控件移动。
- `virtual bool GetPivotForOrbit(...) const override`: 根据插件设置，确定摄像机环绕（Orbit）的中心点是否应为房间的中心。
- `virtual bool GetCursor(...) const override`: 获取鼠标光标类型，委托给当前工具。

#### 公共方法

- `bool GetTool(FName ToolName, ...)`: 根据名称查找工具实例。
- `FProceduralDungeonEditorTool* GetActiveTool()`: 获取当前激活的工具。
- `void SetActiveTool(FName ToolName)`: 根据名称激活一个工具。
- `void ResetActiveTool()`: 停用当前工具。
- `void SetDefaultTool()`: 如果没有激活的工具且条件满足，激活默认工具（"Tool_Size"）。
- `bool IsToolEnabled(FName ToolName)`: 检查一个工具是否可用（基于当前关卡和数据的有效性）。
- `ULevelScriptBlueprint* GetLevelBlueprint(...)`: 获取当前关卡的关卡蓝图。
- `TWeakObjectPtr<ARoomLevel> GetLevel()`: 获取当前关卡蓝图生成的默认 `ARoomLevel` 对象。
- `TWeakObjectPtr<ARoomLevel> GetLevelInstance()`: 获取当前世界中的 `ARoomLevel` 实例（即 `LevelScriptActor`）。
- `void UpdateLevelBlueprint()`: 更新对当前关卡蓝图的缓存，并注册/注销编译委托。
- `void OnLevelBlueprintCompiled(...)`: 当关卡蓝图编译完成时调用，更新关卡实例缓存，设置默认工具，并通知工具包和工具。
- `void RegisterLevelCompilationDelegate(bool Register)`: 注册或注销关卡蓝图编译完成的回调委托。

#### 受保护方法

- `void SetActiveTool(FProceduralDungeonEditorTool* NewTool)`: 设置新的激活工具，负责调用旧工具的 `ExitTool` 和新工具的 `EnterTool`。

#### 公共成员

- `GC_PTR(class UProceduralDungeonEditorObject) Settings`: 指向一个 `UProceduralDungeonEditorObject` 实例，用于存储和暴露编辑器模式的设置（如 DoorType）。

#### 私有成员

- `TArray<TUniquePtr<FProceduralDungeonEditorTool>> Tools`: 存储所有可用工具实例的数组。
- `FProceduralDungeonEditorTool* ActiveTool`: 指向当前激活的工具。
- `TWeakObjectPtr<ARoomLevel> CachedLevelInstance`: 缓存当前世界中的 `ARoomLevel` 实例。
- `TWeakObjectPtr<ULevelScriptBlueprint> CachedLevelBlueprint`: 缓存当前关卡的蓝图。
- `FDelegateHandle LevelBlueprintDelegateHandle`: 用于管理关卡蓝图编译完成委托的句柄。