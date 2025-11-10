# RoomVisitor 类

## 概述

`IRoomVisitor` 是一个接口，当 Actor 进入或退出地牢房间时，它会向 Actor（或组件）添加事件。

## 接口

### URoomVisitor

这是一个 Unreal Engine 接口类，标记为 `MinimalAPI` 和 `BlueprintType`。

### IRoomVisitor

这是 `IRoomVisitor` 接口的实际实现。

#### 成员函数

- `OnRoomEnter(ARoomLevel* RoomLevel)`: 当 Actor 进入地牢房间时调用此函数。`RoomLevel` 是此 Actor 已进入的房间关卡蓝图。你可以从这里使用 Room 访问器。
- `OnRoomExit(ARoomLevel* RoomLevel)`: 当 Actor 退出地牢房间时调用此函数。`RoomLevel` 是此 Actor 已退出的房间关卡蓝图。你可以从这里使用 Room 访问器。

IRoomVisitor 是一个接口（Interface），它定义了一套标准的事件，允许Actor（或其组件）在进入或离开地牢房间时得到通知。

  1. 定义与本质

   * 接口类: IRoomVisitor 是一个 Unreal Engine 接口类 (UInterface)。
   * 核心方法:
       * OnRoomEnter(ARoomLevel* RoomLevel): 当实现了此接口的 Actor 或其组件进入一个地牢房间的 RoomTrigger 体积时，此方法会被调用。参数 RoomLevel
         指向它刚刚进入的那个房间的 ARoomLevel 实例。
       * OnRoomExit(ARoomLevel* RoomLevel): 当实现了此接口的 Actor 或其组件离开一个地牢房间的 RoomTrigger 体积时，此方法会被调用。参数 RoomLevel
         指向它刚刚离开的那个房间的 ARoomLevel 实例。

  2. 如何工作

   * 实现接口: 任何你希望它能对进入/离开房间做出反应的 Actor 类（或 Actor Component 类）都需要在其类定义中声明并实现 IRoomVisitor 接口。
       * 对于 Actor: 在类声明中添加 implements(RoomVisitor) (C++) 或在蓝图类设置中添加 RoomVisitor 接口。
       * 对于 Component: 同理，在组件类声明中添加 implements(RoomVisitor) 或在蓝图组件设置中添加接口。
   * 触发机制:
       1. 当一个 Actor（或其上的组件）进入或离开一个 ARoomLevel 的 RoomTrigger (一个 UBoxComponent) 时，ARoomLevel 会检测到这个重叠事件。
       2. ARoomLevel::OnTriggerBeginOverlap (或 OnTriggerEndOverlap) 会被调用。
       3. 在这些函数内部，ARoomLevel 会检查发生重叠的 OtherActor（即进入/离开的 Actor）以及 OtherActor 上的所有组件。
       4. 对于 OtherActor 本身和它的每个组件，ARoomLevel 会调用 Cast<IRoomVisitor>(ActorOrComponent) 来检查它是否实现了 IRoomVisitor 接口。
       5. 如果转换成功（即该 Actor 或组件确实实现了接口），ARoomLevel 就会调用该接口的 OnRoomEnter 或 OnRoomExit 方法，并将自身的 ARoomLevel 指针作为参数传入。

  3. 用途与好处

   * 解耦: 它提供了一种解耦的方式。ARoomLevel 不需要知道具体是哪个 Actor 类或组件类需要响应房间事件，它只需要知道对方实现了 IRoomVisitor
     接口并调用相应方法即可。同样，Actor/Component 也不需要知道是谁触发了它进入房间，它只需要实现接口方法来定义自己的响应逻辑。
   * 标准化: 它定义了一个标准的、统一的方式来处理房间进出事件。任何实现了该接口的对象都能以一致的方式响应。
   * 灵活性: 开发者可以轻松地为任何 Actor 或组件添加“对房间进出有反应”的能力，只需实现这个接口。
   * 典型应用场景:
       * 玩家角色: 玩家进入新房间时，可能需要触发加载资源、播放音效、更新小地图等。
       * AI 怪物: 怪物进入房间时激活，离开时休眠或巡逻。
       * 动态光源: 进入房间时打开，离开时关闭以节省性能。
       * 触发器逻辑: 进入特定房间时触发剧情、陷阱或任务更新。
       * 音频区域: 根据玩家所在的房间播放不同的环境音效或背景音乐。

  4. 与 `RoomObserverComponent` 和 `StaticRoomObserverComponent` 的关系

  这两个组件是 IRoomVisitor 接口的具体实现。

   * `URoomObserverComponent`: 实现了 IRoomVisitor。当它所在的 Actor 进入/离开房间时，它会收到 OnRoomEnter/OnRoomExit 通知。然后，它会自动将自己绑定到那个房间的
     ActorEnterRoomEvent/ActorExitRoomEvent 上。这意味着它不仅能知道自己进了哪个房间，还能监听那个房间里其他任何 Actor 的进入和离开事件。适用于需要跟踪多个房间活动的
     Actor。
   * `UStaticRoomVisibilityComponent`: 也间接与 IRoomVisitor 相关（虽然它可能不直接实现接口，但其功能类似）。它用于管理那些始终位于一个房间内的 Actor 的可见性。它通常在
     BeginPlay 时自动绑定到所在房间的可见性变化事件上。
   * `URoomVisibilityComponent`: 这个组件直接实现了 IRoomVisitor。当它所在的 Actor（一个可移动的 Actor）进入/离开房间时，OnRoomEnter/OnRoomExit
     会被调用。在这些方法中，它会将自己注册到新进入房间的可见性变化事件上，并从离开的房间的可见性变化事件上注销。这样，它就能确保其所属的 Actor
     的可见性始终与其当前所在的房间的可见性同步。

  总结:

  IRoomVisitor 接口提供了一种强大而灵活的机制，让任何 Actor 或组件都能方便地响应进入和离开地牢房间的事件，而无需 ARoomLevel
  了解它们的具体类型。这是实现动态游戏逻辑、性能优化（如可见性管理）和交互触发的关键工具。