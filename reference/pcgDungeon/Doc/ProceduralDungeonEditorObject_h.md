# ProceduralDungeonEditorObject.h

## 概述

`ProceduralDungeonEditorObject.h` 定义了 `UProceduralDungeonEditorObject` 类。这是一个非常简单的 `UObject` 派生类，主要用作在编辑器模式（`FProceduralDungeonEdMode`）中存储和暴露特定设置的容器。它允许用户在编辑器的细节面板（Details Panel）中直接修改这些设置。

## 类定义

### UProceduralDungeonEditorObject

此类继承自 `UObject`，用于持有编辑器模式的配置数据。

#### 属性

- `UPROPERTY(EditAnywhere, Category = "Procedural Dungeon Editor") class UDoorType* DoorType`: 这是一个可以通过编辑器 UI（`EditAnywhere`）进行编辑的属性。
  - **类型**: `class UDoorType*`，指向一个 `UDoorType` 资产的指针。
  - **用途**: 在“门编辑工具”（`FProceduralDungeonEditorTool_Door`）中，当用户添加新门时，会使用这个属性所指定的 `UDoorType` 作为新门的类型。这使得用户可以在不修改 `URoomData` 资产本身的情况下，临时指定要添加的门的类型。
  - **Category**: 在细节面板中，这个属性会归类在 "Procedural Dungeon Editor" 下。