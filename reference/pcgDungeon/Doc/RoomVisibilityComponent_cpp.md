# RoomVisibilityComponent.cpp

该文件包含了 `URoomVisibilityComponent` 类的实现，该类是一个Actor组件，用于管理Actor在地牢中的可见性。

## 主要功能

- **房间访客接口实现**：实现 `IRoomVisitor` 接口，响应Actor进入和离开房间的事件。
- **可见性管理**：注册可见性委托，以便在房间可见性更改时更新Actor的可见性。

## 详细实现

### 构造函数

- `URoomVisibilityComponent`：构造函数，调用父类构造函数。

### 房间访客接口

- `OnRoomEnter`：当Actor进入房间时调用。
  - 记录调试日志。
  - 调用 `RegisterVisibilityDelegate` 注册可见性委托，参数为 `true` 表示进入房间。
- `OnRoomExit`：当Actor离开房间时调用。
  - 记录调试日志。
  - 调用 `RegisterVisibilityDelegate` 注册可见性委托，参数为 `false` 表示离开房间。

这个组件继承自 `UStaticRoomVisibilityComponent`，并实现了 `IRoomVisitor` 接口。它适用于那些能够在房间之间移动的Actor。如果Actor始终位于同一个房间内，则应使用 `UStaticRoomVisibilityComponent`。