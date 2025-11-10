# RoomObserverComponent.cpp

该文件包含了 `URoomObserverComponent` 类的实现，该类是一个房间观察者组件，当Actor进入或离开地牢房间时自动绑定或解绑。

## 主要功能

- **房间访客接口实现**：实现 `IRoomVisitor` 接口，响应Actor进入和离开房间的事件。
- **事件绑定管理**：自动绑定和解绑房间级别的Actor进入和离开事件。
- **事件广播**：广播Actor进入和离开房间的事件。

## 详细实现

### 构造函数

- `URoomObserverComponent`：构造函数。
  - 设置组件不需要每帧tick。

### 房间访客接口

- `OnRoomEnter`：当Actor进入房间时调用。
  - 记录调试日志。
  - 调用 `BindToLevel` 绑定到房间级别，参数为 `true`。
- `OnRoomExit`：当Actor离开房间时调用。
  - 记录调试日志。
  - 调用 `BindToLevel` 解绑房间级别，参数为 `false`。

### 房间事件处理

- `OnActorEnterRoom`：当Actor进入房间时的回调函数。
  - 将调用转发给 `ActorEnterRoomEvent` 委托。
- `OnActorExitRoom`：当Actor离开房间时的回调函数。
  - 将调用转发给 `ActorExitRoomEvent` 委托。

### 事件绑定管理

- `BindToLevel`：绑定或解绑房间级别的事件。
  - 检查房间级别是否已经在绑定列表中。
  - 如果房间级别有效：
    - 如果是绑定操作，则添加事件监听器并将其添加到绑定列表。
    - 如果是解绑操作，则移除事件监听器并将其从绑定列表中移除。

这个组件允许Actor在进入房间时自动开始监听该房间内其他Actor的进入和离开事件，并在离开房间时停止监听。它适用于需要跟踪多个房间事件的Actor。