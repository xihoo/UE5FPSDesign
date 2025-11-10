# GAS 集成 · 核心概念

## Ability System Component (ASC)
- 每个参与 GAS 的 Actor 都需要一个 ASC。
- 管理 Abilities、Effects、Attributes，并处理 GameplayTags。

## Gameplay Ability
- 定义可执行的游戏行为（技能、射击等）。
- 支持网络复制与预测，可被打断、冷却、消耗资源。

## Gameplay Effect (GE)
- 修改 Attributes 的机制。
- 支持即时、持续与周期性效果；可堆叠、可移除。

## Gameplay Tags
- 标记与分类游戏内容，支持层级结构与查询。
- 控制能力激活条件、效果条件、Cue 触发。

## Attribute Set
- 存储角色属性（生命、护盾、伤害等）。
- 自动网络复制，支持 Pre/Post 修改回调。