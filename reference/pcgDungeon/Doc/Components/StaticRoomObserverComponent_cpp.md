# StaticRoomObserverComponent.cpp

该文件包含了 `UStaticRoomObserverComponent` 类的实现，该类是一个静态房间观察者组件，在BeginPlay和EndPlay时自动绑定和解绑。

## 主要功能

- **静态绑定**：在游戏开始和结束时自动绑定和解绑房间级别的Actor进入和离开事件。
- **事件广播**：广播Actor进入和离开房间的事件。

## 详细实现

### 构造函数

- `UStaticRoomObserverComponent`：构造函数。
  - 设置组件不需要每帧tick。

### 游戏开始和结束

- `BeginPlay`：游戏开始时的回调函数。
  - 调用父类的 `BeginPlay`。
  - 调用 `BindToLevel(true)` 绑定到房间级别。
- `EndPlay`：游戏结束时的回调函数。
  - 调用父类的 `EndPlay`。
  - 调用 `BindToLevel(false)` 解绑房间级别。

### 房间事件处理

- `OnActorEnterRoom`：当Actor进入房间时的回调函数。
  - 将调用转发给 `ActorEnterRoomEvent` 委托。
- `OnActorExitRoom`：当Actor离开房间时的回调函数。
  - 将调用转发给 `ActorExitRoomEvent` 委托。

### 事件绑定管理

- `BindToLevel`：绑定或解绑房间级别的事件。
  - 检查是否已经绑定或解绑。
  - 获取组件所在的关卡。
  - 获取关卡的脚本Actor并转换为 `ARoomLevel`。
  - 如果房间级别有效：
    - 如果是绑定操作，则添加事件监听器。
    - 如果是解绑操作，则移除事件监听器。
  - 更新 `bBound` 状态。

这个组件适用于那些放置在房间关卡中且不会在房间之间移动的Actor。它在游戏开始时自动开始监听房间内其他Actor的进入和离开事件，并在游戏结束时停止监听。