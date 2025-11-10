# RoomVisibilityComponent 类

## 概述

`URoomVisibilityComponent` 类是一个 Actor 组件，用于管理地牢中 Actor 的可见性。如果 Actor 能够在房间之间移动，请使用此组件。如果 Actor 始终保持在同一个房间内，请改用 `UStaticRoomVisibilityComponent`。它继承自 `UStaticRoomVisibilityComponent`，并实现了 `IRoomVisitor` 接口。

## 成员函数

### 构造函数

- `URoomVisibilityComponent()`: 构造函数。

### IRoomVisitor 接口

- `OnRoomEnter_Implementation(ARoomLevel* RoomLevel)`: 进入房间时调用。
- `OnRoomExit_Implementation(ARoomLevel* RoomLevel)`: 退出房间时调用。