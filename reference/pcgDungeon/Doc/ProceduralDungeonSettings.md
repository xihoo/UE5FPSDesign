# ProceduralDungeonSettings 类

## 概述

`UProceduralDungeonSettings` 类持有插件的设置。它继承自 `UObject`，并使用 `Config = Game, DefaultConfig` 来存储游戏配置。

## 成员变量

### 常规设置

- `RoomUnit` (FVector): 房间单元的大小。数据资产中的房间大小将表示为此单元大小的倍数。
- `DoorSize` (FVector): 门的边界大小。它仅用于在 "Draw Debug" 设置为 true 时显示门的蓝色框。
- `DoorOffset` (float): 门的底部相对于房间底部的高度（以房间单元 Z 的百分比表示）。
- `CanLoop` (bool): 当为 true 时，只要门位于同一位置，就会连接。当为 false 时，仅连接前一个房间和新生成房间之间的门。**已弃用**：保持为 true 并改用 DungeonGenerator Actor 中的 CanLoop 设置。此项目范围的设置将在未来版本中删除。
- `RoomObjectType` (TEnumAsByte<ECollisionChannel>): 用于地牢房间触发盒的对象类型。默认为 Engine Trace Channel 6。你可以在项目设置的 Collision 选项卡下创建新的类型。
- `MaxGenerationTry` (int32): 地牢生成重试次数，超过此次数后生成器将放弃。
- `MaxRoomPlacementTry` (int32): 在特定门上放置房间的重试次数，超过此次数后生成器将放弃并继续处理下一个门。
- `RoomLimit` (int32): 房间数量限制。

### 遮挡剔除设置

- `OcclusionCulling` (bool): 当玩家不在房间内或不在相邻房间内时，房间的可见性将被关闭。
- `OcclusionDistance` (int32): 定义从玩家房间可见的连接房间数量（1 表示仅玩家所在的房间）。
- `OccludeDynamicActors` (bool): 跟踪动态 Actor 进入和离开房间，以便能够随房间一起显示/隐藏它们。

### 调试设置

- `DrawDebug` (bool): 在编辑器和开发版本中显示房间和门的轮廓。
- `bDrawOnlyWhenEditingRooms` (bool): 仅在编辑房间时绘制调试信息。
- `ShowRoomOrigin` (bool): 以洋红色显示房间原点。**已弃用**：此设置将在插件的未来版本中删除。
- `bFlipDoorArrowSide` (bool): 翻转显示门朝向方向的箭头。True 表示箭头进入房间（与门 Actor 的前向相反）。False 表示箭头离开房间（与门 Actor 的前向相同）。
- `DoorArrowLength` (float): 门的调试箭头长度。
- `DoorArrowHeadSize` (float): 门的调试箭头头部大小。
- `OnScreenPrintDebug` (bool): 在屏幕上显示一些日志。
- `PrintDebugDuration` (float): 屏幕日志的持续时间。

## 构造函数

- `UProceduralDungeonSettings(const FObjectInitializer& ObjectInitializer)`: 构造函数。