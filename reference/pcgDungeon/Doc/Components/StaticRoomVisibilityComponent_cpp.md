# StaticRoomVisibilityComponent.cpp

该文件包含了 `UStaticRoomVisibilityComponent` 类的实现，该类是一个组件，用于管理地牢中Actor的可见性。

## 主要功能

- **可见性管理**：根据房间的可见性状态控制Actor的可见性。
- **可见性模式**：支持多种可见性模式（默认、强制隐藏、强制可见、自定义）。
- **事件广播**：当房间可见性改变时广播事件。

## 详细实现

### 构造函数

- `UStaticRoomVisibilityComponent`：构造函数。
  - 设置组件不需要每帧tick。

### 游戏开始和结束

- `BeginPlay`：游戏开始时的回调函数。
  - 调用父类的 `BeginPlay`。
  - 调用 `UpdateVisibility` 更新可见性。
  - 调用 `RegisterVisibilityDelegate` 注册可见性委托。
- `EndPlay`：游戏结束时的回调函数。
  - 调用父类的 `EndPlay`。
  - 调用 `RegisterVisibilityDelegate` 注销可见性委托。

### 可见性控制

- `IsVisible`：检查Actor是否可见。
  - 如果启用了遮挡剔除和动态Actor遮挡，则根据 `VisibilityEnablers` 的数量判断。
  - 否则始终返回 `true`。
- `SetVisible`：设置可见性启用器。
  - 添加或移除可见性启用器。
  - 如果可见性状态改变，则调用 `UpdateVisibility` 并广播 `OnRoomVisibilityChanged` 事件。
- `ResetVisible`：重置可见性，等同于 `SetVisible(Owner, false)`。
- `SetVisibilityMode`：设置可见性模式并更新可见性。

### 工具函数

- `GetOwnerRoomLevel`：获取拥有者Actor所在的房间级别。
- `UpdateVisibility`：更新Actor的可见性。
  - 清理无效的可见性启用器。
  - 根据可见性模式设置根组件的可见性。
- `RegisterVisibilityDelegate`：注册或注销房间可见性改变事件的委托。
- `RoomVisibilityChanged`：房间可见性改变时的回调函数。
  - 调用 `SetVisible` 更新可见性。
- `CleanEnablers`：清理无效的可见性启用器。

这个组件适用于那些始终位于同一个房间内的Actor。它通过监听房间的可见性变化来控制Actor的可见性，并支持多种可见性模式供开发者自定义。