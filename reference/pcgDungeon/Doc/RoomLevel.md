# RoomLevel 类

## 概述

`ARoomLevel` 类是房间实例的关卡脚本 Actor。此类用于管理房间实例及其边界。它继承自 `ALevelScriptActor`。

## 成员变量

- `Data` (URoomData*): 房间数据。
- `Room` (URoom*): 房间实例。
- `bIsDungeonEditorMode` (bool): 标志，用于知道地牢编辑器模式是否处于活动状态（仅在编辑器中）。
- `bIsInit` (bool): 是否已初始化。
- `DungeonTransform` (FTransform): 地牢变换。
- `Bounds` (FBoxCenterAndExtent): 边界。
- `RoomTrigger` (UBoxComponent*): 房间触发器。
- `Visitors` (TSet<TWeakObjectPtr<UObject>>): 访问者。

## 成员函数

### 构造函数

- `ARoomLevel(const FObjectInitializer& ObjectInitializer)`: 构造函数。

### 虚函数重写

- `BeginPlay()`: 开始游戏时调用。
- `EndPlay()`: 结束游戏时调用。
- `Tick()`: 每帧调用。
- `ShouldTickIfViewportsOnly()`: 如果仅在视口中运行，是否应该调用 Tick。

### 公共函数

- `Init(URoom* Room)`: 初始化房间。
- `SetActorsVisible(bool Visible)`: 设置 Actor 是否可见。
- `IsInit()`: 是否已初始化。
- `IsPlayerInside()`: 玩家是否在房间内（已弃用）。
- `IsVisible()`: 房间是否可见（已弃用）。
- `IsLocked()`: 房间是否被锁定（已弃用）。
- `Lock(bool lock)`: 锁定或解锁房间（已弃用）。
- `GetRoom()`: 获取房间实例。
- `OnTriggerBeginOverlap(...)`: 触发器开始重叠时调用。
- `OnTriggerEndOverlap(...)`: 触发器结束重叠时调用。
- `GetBoundsCenter()`: 获取边界中心。
- `GetBoundsExtent()`: 获取边界范围。

### 事件

- `VisibilityChangedEvent`: 当房间的可见性被切换时通知的事件。
- `ActorEnterRoomEvent`: 当 Actor 进入房间边界时调用。
- `ActorExitRoomEvent`: 当 Actor 退出房间边界时调用。

### 私有函数

- `UpdateBounds()`: 更新边界。
- `UpdateVisitor(UObject* Visitor, bool IsInside)`: 更新访问者。
- `TriggerActor(AActor* Actor, bool IsInTrigger)`: 触发 Actor。
- `PostInitProperties()`: 初始化属性后调用。

### 编辑器函数

- `OnPropertiesChanged`: 属性更改事件。
- `PostEditChangeProperty(...)`: 属性更改后调用。


好的，我们来详细解释一下 ProceduralDungeon 插件中房间和 Actor
  的可见性管理机制。这是一个多层次、相互协作的系统，旨在优化性能（通过隐藏玩家看不到的房间）并提供灵活的控制。

  核心概念:

   * 房间可见性 (`URoom::bIsVisible`): 这是逻辑层面的可见性。一个房间被标记为可见，意味着它应该在游戏世界中显示出来。这个状态由地牢的遮挡剔除系统（Occlusion
     Culling）根据玩家位置和设置（如 OcclusionDistance）来计算和更新。
   * Actor 可见性 (`AActor::bHidden`): 这是渲染层面的可见性。一个 Actor 的 bHidden 标志决定了它是否会被渲染引擎绘制到屏幕上。这是最终影响玩家“看到”与否的状态。
   * 桥梁: ARoomLevel 扮演了将逻辑可见性（URoom::IsVisible()）转换为渲染可见性（AActor::SetActorHiddenInGame）的关键角色。

  详细流程:

   1. 遮挡剔除系统的驱动 (`ADungeonGeneratorBase::UpdateRoomVisibility`):
       * 这是整个可见性管理的源头。在 ADungeonGeneratorBase 的 Tick 函数中（如果处于 Play 状态），会定期调用 UpdateRoomVisibility。
       * 获取玩家: 它首先调用 GetVisibilityPawn() (一个蓝图可重写事件) 来确定用于计算可见性的“焦点”Pawn（通常是玩家控制的角色）。
       * 查找玩家所在房间: 它会找到这个 Pawn 当前位于哪个 URoom 实例中（通过 URoom::IsPlayerInside() 标志，这个标志由 ARoomLevel 的 RoomTrigger 更新）。
       * 计算可见房间集合:
           * 如果插件设置 OcclusionCulling 为 true：
               * 它会以玩家所在的房间为起点，使用图遍历算法（如 UDungeonGraph::TraverseRooms）找出所有在 OcclusionDistance 范围内的连接房间。
               * 这个房间集合 (TSet<URoom*> VisibleRooms) 就是理论上玩家“应该”能看到的房间。
           * 如果 OcclusionCulling 为 false：则所有房间都被认为是可见的。
       * 更新 `URoom::bIsVisible`:
           * 系统会遍历地牢中的所有 URoom 实例。
           * 对于每个 URoom，检查它是否在上面计算出的 VisibleRooms 集合中。
           * 调用 URoom::SetVisible(bool bVisible)。这个函数会：
              a.  更新 URoom 内部的 bIsVisible 标志。
              b.  如果可见性发生了改变（从可见变不可见，或反之），则触发 URoom::OnRep_IsVisible() (如果是网络复制) 或直接调用 URoom::UpdateVisibility()。

   2. `URoom::UpdateVisibility`: 同步到关卡 Actor:
       * 当 URoom 的逻辑可见性状态改变时，UpdateVisibility 会被调用。
       * 它的主要任务是通知关联的 ARoomLevel 实例这个变化。
       * 它会调用 ARoomLevel::SetActorsVisible(URoom::IsVisible())，将 URoom 的当前可见性状态传递给 ARoomLevel。

   3. `ARoomLevel::SetActorsVisible`: 应用到具体 Actor:
       * 这是最终执行可见性切换的地方。
       * 获取可见性状态: 接收来自 URoom 的 Visible 布尔值。
       * 检查全局设置: 首先检查插件设置 OcclusionCulling 是否启用。如果禁用，则函数可能直接返回，或者强制所有 Actor 可见。
       * 遍历关卡 Actor:
           * 调用 ARoomLevel 所在关卡的 Actors 列表 (TActorIterator<AActor> 或类似机制)。
           * 对关卡中的每一个 AActor 实例进行处理。
       * 过滤 Actor:
           * 复制的 Actor: 跳过 Actor->HasAnyFlags(RF_NetTemporary | RF_NetStartup) 标记的 Actor，因为它们通常由网络系统特殊处理。
           * 显式忽略剔除的 Actor: 检查 Actor 是否拥有 RoomObjectType (默认是 ECC_GameTraceChannel6) 的 Object Type。如果拥有，并且其
             GetCollisionResponseToChannel(RoomObjectType) 设置为 ECR_Ignore，则跳过该 Actor。这是一种让特定 Actor 不受房间剔除影响的方法（例如，某些全局特效或UI元素）。
       * 设置 Actor 隐藏状态:
           * 对于所有未被跳过的 Actor，调用 Actor->SetActorHiddenInGame(!Visible)。
           * 注意这里的逻辑：如果房间逻辑上是可见的 (Visible=true)，那么 Actor 应该不隐藏 (!true = false)；如果房间逻辑上是不可见的 (Visible=false)，那么 Actor 应该隐藏
             (!false = true)。
       * 广播事件: 最后，ARoomLevel 会广播其 VisibilityChangedEvent，允许其他监听者（如 URoomVisibilityComponent）得知该房间的可见性发生了变化。

   4. 动态 Actor 的可见性管理 (`URoomVisibilityComponent`):
       * 上述流程主要管理静态属于某个房间的 Actor（即关卡中一开始就存在的 Actor）。
       * 对于动态进入房间的 Actor（如玩家、怪物、掉落的物品），需要一种机制来确保它们的可见性与它们当前所在的房间同步。
       * `URoomVisibilityComponent`: 这是一个可以附加到动态 Actor 上的组件。
       * 实现 `IRoomVisitor`: 它实现了 IRoomVisitor 接口。当它所在的 Actor 通过 RoomTrigger 进入或离开一个房间时，它的 OnRoomEnter 和 OnRoomExit 方法会被 ARoomLevel
         调用。
       * 注册/注销监听:
           * OnRoomEnter: 当 Actor 进入新房间时，URoomVisibilityComponent 会调用 ARoomLevel->VisibilityChangedEvent.Add(...)，将自身的一个回调函数（例如
             OnRoomVisibilityChanged）注册到新房间的可见性变化事件上。同时，它可能会从旧房间（如果有的话）的事件中注销。
           * OnRoomExit: 当 Actor 离开房间时，它会从该房间的 VisibilityChangedEvent 中注销。
       * 响应房间可见性变化:
           * 当 Actor 所在的房间可见性发生变化时（例如，因为玩家走远了），ARoomLevel::VisibilityChangedEvent 会被广播，从而调用
             URoomVisibilityComponent::OnRoomVisibilityChanged。
           * 在这个回调函数中，组件会获取当前房间的可见性状态 (ARoomLevel->GetRoom()->IsVisible())，然后调用其所属 Actor 的 SetActorHiddenInGame(!IsRoomVisible)，确保动态
             Actor 的可见性与房间同步。

   5. 静态 Actor 的可见性管理 (`UStaticRoomVisibilityComponent`):
       * 对于那些永远不会移动、始终属于一个固定房间的动态 Actor（例如，放置在房间内但运行时生成的装饰品），使用 URoomVisibilityComponent
         会有些冗余（因为它需要监听进入/离开事件）。
       * `UStaticRoomVisibilityComponent`: 这个组件更简单。
       * 它通常在 BeginPlay 时获取它所在的 ARoomLevel，然后直接注册到该房间的 VisibilityChangedEvent 上。
       * 它的 OnRoomVisibilityChanged 回调逻辑与 URoomVisibilityComponent 类似，用于同步其所属 Actor 的隐藏状态。

  总结:

   6. 逻辑驱动: ADungeonGeneratorBase 根据玩家位置和设置计算出哪些房间在逻辑上应该是可见的，并更新 URoom::bIsVisible。
   7. 状态传递: URoom 将其可见性状态通过 UpdateVisibility 传递给其对应的 ARoomLevel。
   8. 渲染应用: ARoomLevel::SetActorsVisible 遍历其关卡内的所有 Actor，根据传入的可见性状态调用 SetActorHiddenInGame 来控制它们的渲染。
   9. 动态同步: URoomVisibilityComponent (用于移动 Actor) 和 UStaticRoomVisibilityComponent (用于静态 Actor) 通过监听 ARoomLevel::VisibilityChangedEvent
      来确保动态进入房间的 Actor 其可见性也能与房间保持一致。
   10. 可配置: 整个系统可以通过 UProceduralDungeonSettings 中的 OcclusionCulling, OcclusionDistance 等设置进行调整。

  这个系统有效地将复杂的可见性计算与最终的渲染控制分离开来，并通过事件驱动的方式处理动态 Actor，实现了高效且灵活的房间级可见性管理。