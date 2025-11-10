# ProceduralDungeonEditorTool.h

## 概述

`ProceduralDungeonEditorTool.h` 定义了 `FProceduralDungeonEditorTool` 基类。这个类是“地牢房间编辑模式”（Dungeon Room Editor Mode）中各种具体编辑工具（如调整大小工具、门编辑工具）的抽象基类。它继承自 `FGCObject` 和 `FSelfRegisteringEditorUndoClient`，以支持垃圾回收和编辑器撤销/重做功能。

## 类定义

### FProceduralDungeonEditorTool

这是一个抽象基类，为编辑器模式下的不同工具提供统一的接口。

#### 构造函数

- `FProceduralDungeonEditorTool(FProceduralDungeonEdMode* InEdMode)`: 构造函数，接受一个指向所属编辑器模式 (`FProceduralDungeonEdMode`) 的指针，并将其存储在受保护的成员变量 `EdMode` 中。

#### 纯虚函数 (需要子类实现)

- `virtual const TCHAR* GetToolName() = 0;`: 获取工具的内部名称（通常用于标识）。
- `virtual FText GetDisplayName() = 0;`: 获取工具的显示名称（用于 UI）。
- `virtual FText GetDisplayMessage() = 0;`: 获取工具激活时在编辑器视口中显示的消息。
- `virtual void EnterTool() = 0;`: 当工具被激活时调用。
- `virtual void ExitTool() = 0;`: 当工具被停用或切换时调用。

#### 虚函数 (可选重写)

- `virtual void Render(...)`: 用于在视口中渲染工具特定的可视化元素（如 Gizmo）。
- `virtual void Tick(...)`: 每帧调用，用于处理工具的更新逻辑。
- `virtual bool MouseMove(...)`: 处理鼠标在视口中的移动事件。
- `virtual bool HandleClick(...)`: 处理鼠标点击事件。
- `virtual bool InputKey(...)`: 处理键盘按键事件。
- `virtual bool InputAxis(...)`: 处理模拟输入轴事件（如手柄摇杆）。
- `virtual bool InputDelta(...)`: 处理视口中的变换操作（如拖拽、旋转、缩放）。
- `virtual bool UsesTransformWidget() const`: 指示工具是否使用标准的变换控件（移动、旋转、缩放）。
- `virtual FVector GetWidgetLocation() const`: 如果使用变换控件，返回控件的位置。
- `virtual bool GetCursor(...) const`: 获取鼠标光标类型。

#### FGCObject 接口

- `virtual void AddReferencedObjects(...) override`: 用于垃圾回收，标记工具引用的 UObject。在此基类中为空实现。
- `virtual FString GetReferencerName() const override`: 返回引用者的名称，用于调试。

#### 通知函数

- `virtual void OnLevelChanged(...)`: 当编辑的 `ARoomLevel` 发生变化时调用。
- `virtual void OnDataChanged(...)`: 当关联的 `URoomData` 发生变化时调用。

#### 受保护成员

- `FProceduralDungeonEdMode* EdMode`: 指向所属编辑器模式的指针，子类可以通过它访问模式的数据和功能。