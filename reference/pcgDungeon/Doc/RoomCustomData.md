# RoomCustomData 类

## 概述

`URoomCustomData` 类是嵌入到房间实例中的用户自定义数据的基类。它继承自 `UReplicableObject`，并实现了 `IDungeonCustomSerialization` 和 `IDungeonSaveInterface` 接口。

## 成员函数

### 公共函数

- `CreateLevelComponent(class ARoomLevel* LevelActor)`: 创建关卡组件。

### IDungeonCustomSerialization 接口

- `SerializeObject(FStructuredArchive::FRecord& Record, bool bIsLoading)`: 序列化对象。

### IDungeonSaveInterface 接口

- `PreSaveDungeon_Implementation()`: 保存地牢前调用。
- `PostLoadDungeon_Implementation()`: 加载地牢后调用。

## 成员变量

- `LevelComponent` (TSubclassOf<UActorComponent>): 要在实例化房间的关卡脚本 Actor 上创建和附加的组件。
- `LevelComponentInstance` (TWeakObjectPtr<UActorComponent>): 对创建的组件实例的引用。

## 私有结构体

### FSaveData

存储组件数据。

- `ComponentData` (TArray<uint8>): 组件数据。


URoomCustomData 是一个基类，用于向 URoom
  实例（即地牢中一个具体生成的房间）附加额外的、可自定义的数据和逻辑。它允许开发者为不同的房间实例定义独特的属性、状态或行为，而这些内容无法或不适合直接放在 URoomData
  模板或 ARoomLevel 关卡中。

  1. 定义与本质

   * 继承自 `UReplicableObject`: 这意味着 URoomCustomData 实例可以像 UObject 一样拥有属性，并且可以被网络复制。这对于多人游戏中同步房间的自定义状态至关重要。
   * 设计为可扩展: 它被设计成一个基类，开发者需要创建继承自 URoomCustomData 的具体子类来实现特定功能。

  2. 核心功能与用途

  URoomCustomData 的核心目的是为每个房间实例提供一个可以存储和管理特定数据及逻辑的容器。

   * 存储实例化数据:
       * URoomData 是模板，所有使用该模板生成的房间实例共享其定义（如大小、门的位置）。但有时你需要为某个特定的房间实例存储独一无二的信息。
       * 例如：
           * 一个“宝藏室”房间实例，你需要记录里面哪个宝箱已经被玩家拿走过。
           * 一个“谜题室”房间实例，你需要记录当前的谜题状态（已解决？进行到第几步？）。
           * 一个“商店”房间实例，你需要存储当前出售的物品种类和数量。
       * 这些信息属于运行时的、特定于实例的状态，不适合放在 URoomData 模板中。URoomCustomData 就是用来存放这些数据的理想位置。

   * 附加组件到房间关卡:
       * LevelComponent (TSubclassOf<UActorComponent>): 这是 URoomCustomData 的一个关键成员。
       * 它允许你指定一个 UActorComponent 类。当 URoom 的关卡（ARoomLevel）被加载和初始化时，系统会自动在 ARoomLevel Actor 上创建并附加这个指定的组件实例。
       * 用途:
           * 运行时逻辑: 你可以创建一个自定义组件（例如 UTreasureRoomLogicComponent），在里面编写管理宝箱、触发特殊事件的代码。通过 URoomCustomData
             将这个组件附加到对应的房间关卡上，就能让该房间拥有独特的运行时行为。
           * 数据存储: 组件本身也可以用来存储数据（通过其 UPROPERTY），并且可以方便地在蓝图中访问和修改。
           * 生命周期管理: 组件的生命周期与 ARoomLevel 绑定，当房间被卸载时，组件也会被自动销毁。

   * 网络复制与保存/加载:
       * 由于继承自 UReplicableObject 并实现了 IDungeonCustomSerialization 和 IDungeonSaveInterface，URoomCustomData 及其 LevelComponentInstance
         可以被正确地在网络上传输，并且可以参与游戏的保存和加载过程。这对于需要持久化或同步的自定义房间状态非常重要。

  3. 如何使用

   4. 创建自定义子类:
       * 在 C++ 中或蓝图中，创建一个继承自 URoomCustomData 的新类，例如 UTreasureRoomCustomData。
       * 在这个子类中，添加你需要的属性（UPROPERTY），例如 UPROPERTY() bool bTreasureTaken;。
       * 如果需要，重写 PreSaveDungeon 和 PostLoadDungeon 来处理保存/加载时的特殊逻辑。

   5. 关联到 `URoomData`:
       * 打开你想要使用这个自定义数据的 URoomData 资产（例如 TreasureRoom_Data）。
       * 在其 Details 面板中，找到 Custom Data (TSet<TSubclassOf<URoomCustomData>>) 属性。
       * 将你刚刚创建的 UTreasureRoomCustomData 类添加到这个集合中。

   6. 在生成时自动创建:
       * 当 ADungeonGenerator 生成一个使用 TreasureRoom_Data 的房间时，它会创建一个 URoom 实例。
       * 在 URoom::Init 或 URoom::CreateAllCustomData 过程中，系统会遍历 TreasureRoom_Data->CustomData 集合。
       * 对于集合中的每一个 URoomCustomData 子类（如 UTreasureRoomCustomData），系统会创建一个该类的实例，并将其添加到 URoom::CustomData 数组中。

   7. （可选）指定并附加组件:
       * 在 UTreasureRoomCustomData 的 Details 面板中，找到 Level Component 属性。
       * 将你想要附加到房间关卡的组件类（例如 UTreasureRoomLogicComponent）拖拽到此属性上。
       * 当该房间的关卡加载完毕并在 URoom::OnInstanceLoaded 中调用 URoomCustomData::CreateLevelComponent 时，这个组件会被创建并附加到 ARoomLevel Actor 上。

   8. 访问和使用:
       * 在 C++ 中：你可以在 ARoomLevel 或其他能访问到 URoom 的地方，通过 URoom::GetCustomData 或 URoom::HasCustomData 来获取或检查是否存在特定的 URoomCustomData
         实例，然后访问其属性或调用其方法。
       * 在蓝图中：URoom 提供了 GetCustomData (Blueprint) 节点，可以方便地在蓝图中获取自定义数据。

  总结:

  URoomCustomData 是一个强大的扩展机制。它允许你为地牢中每个具体的房间实例添加独特的、可复制的、可保存的数据和组件。它将与房间实例相关的特定逻辑和状态从 URoomData 模板和
  ARoomLevel 关卡中分离出来，使得系统更加模块化和灵活。当你需要为不同类型的房间实例赋予不同的运行时行为或状态时，它是一个非常有用的工具。