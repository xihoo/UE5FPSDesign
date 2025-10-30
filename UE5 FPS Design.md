这是一个基于UE5的FPS游戏的总体设计文档

## 游戏流程概述

类似土豆兄弟，随机地牢生成，FPS击杀敌人，然后通过升级，商店购买Item来提升自己，战胜敌人，探索随机地牢，最后通关。

## 模块
1. Input
	1. Enhanced Input
2. UI
	1. menu
	2. HUD
3. c++基类 Character、GameMode、PlayerController等
	1. Pawn
	2. Character
	3. PlayerCharacter
	4. PlayerController
	5. PlayerState
	6. GameMode
	7. GameInstance
	8. GameState
	9. AIController
4. 库存组件 Inventory Component
	1. Item Definition
	2. Item Instance
5. 技能组件 Skill Component
	1. Skill Definition
	2. Skill Instance
6. SKGShooterFramework 射击框架
	1. SKGProceduralAnim
	2. SKGProjectile
	3. SKGOptic
	4. SKGMuzzle
	5. SKGStock
	6. SKGFirearmComponent
	7. SKGShooterPawnComponent
7. ProceduralDungeon 随机地牢生成
	1. DungeonGenerator
	2. Room
	3. RoomData
	4. Door
8. GAS集成
	1. 游戏伤害构成系统
	2. 元素反应
	3. 各种属性
	4. AbilitySystemComponent
9. 游戏角色动画
10. 游戏音效
11. 视觉特效
12. loadout、attachment系统
13. 游戏AI
14. 相机
15. 交互系统