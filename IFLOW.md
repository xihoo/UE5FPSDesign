# UE5 FPS 游戏项目 - iFlow 上下文信息

## 项目概述

这是一个基于 Unreal Engine 5 的多人合作 PVE FPS 游戏设计文档项目。游戏设计灵感来源于"土豆兄弟"，采用随机地牢生成机制，玩家通过射击敌人、升级角色、购买物品来提升能力，最终探索并通关随机生成的地牢。

**重要说明**: 当前项目仅包含设计文档，尚未创建实际的 UE5 项目文件。需要先创建 UE5 项目才能开始开发实现。

## 项目结构

```
UE5FPSDesign/
├── README.md                    # 项目总体说明
├── UE5 FPS Design.md           # 游戏总体设计文档
├── CLAUDE.md                   # Claude AI 开发指南
├── Animation/                  # 角色动画系统设计
├── GAS集成/                    # Gameplay Ability System 集成方案
├── Input/                      # 增强输入系统设计
├── UI/                         # 用户界面设计
├── 交互系统/                   # 游戏交互机制设计
├── 基类/                       # 核心类层次结构设计
├── 射击框架/                   # 武器射击系统设计
├── 库存/                       # 物品库存系统设计
├── 技能/                       # 技能系统设计
├── 游戏AI/                     # AI 系统设计
├── 随机地图/                   # 程序化地牢生成设计
└── reference/                  # 参考框架和插件
    ├── pcgDungeon/             # 程序化地牢插件参考
    └── skg/                    # SKG 射击框架参考
```

## 核心设计原则

1. **GAS 框架集成** - 所有游戏机制必须使用 Unreal 的 Gameplay Ability System
2. **C++ 优先实现** - 核心系统使用 C++ 实现，设计密集型功能可使用蓝图
3. **数据驱动架构** - 使用 UE5 数据资产进行配置，避免硬编码数值
4. **多人游戏优先** - 从设计初期就考虑网络同步和复制
5. **SKG 框架参考** - 参考 SKG Shooter Framework 设计模式，但需适配 GAS 集成

## 核心系统架构

### 基类层次结构
```
GameInstance
├── GameMode
├── GameState
├── PlayerController
├── PlayerState
└── Character
    └── PlayerCharacter
        ├── InventoryComponent (库存组件)
        ├── SkillComponent (技能组件)
        ├── FirearmComponent (射击组件)
        ├── InteractionComponent (交互组件)
        └── AbilitySystemComponent (GAS组件)
```

### 主要系统模块

1. **增强输入系统** - 基于 UE5 Enhanced Input，支持多种输入设备，并通过 “InputAction -> GameplayTag -> Ability” 标签映射激活能力，实现预测与冷却统一
2. **库存系统** - 基于定义/实例模式的物品管理
3. **技能系统** - 与 GAS 深度集成的技能机制
4. **射击框架** - 模块化武器系统，参考 SKG 框架设计
5. **随机地牢生成** - 程序化房间布局和连接
6. **GAS 集成** - 伤害系统、元素反应、角色属性
7. **UI 系统** - 菜单、HUD、游戏界面
8. **动画系统** - 角色动画、武器动画、程序化动画
9. **AI 系统** - 敌人行为、路径查找、战斗 AI
10. **交互系统** - 物品拾取、环境交互、机关触发

## 参考框架

### SKG Shooter Framework
位于 `/reference/skg/`，提供以下参考：
- 模块化武器系统设计
- 程序化动画实现
- 组件化射击机制
- 数据资产配置模式
- 多人游戏网络同步

**核心模块**:
- SKGAttachment (附件系统)
- SKGFirearmComponent (火器核心)
- SKGShooterPawnComponent (射击角色)
- SKGProceduralAnim (程序化动画)
- SKGProjectile (投射物系统)

### Procedural Dungeon Plugin
位于 `/reference/pcgDungeon/`，提供以下参考：
- 房间基础生成系统
- 手工制作房间与程序化结合
- 地牢网络复制机制
- 门连接和房间规则

**核心类**:
- ADungeonGenerator (地牢生成器)
- URoom (房间实例)
- URoomData (房间数据资产)
- Door (门连接系统)

## 开发环境要求

- **Unreal Engine 5.6+**
- **C++ 支持**
- **Gameplay Ability System 插件**
- **Enhanced Input 插件**
- **Niagara 插件** (粒子效果)

## 项目创建步骤

1. 创建新的 UE5 C++ 项目
2. 设置基础文件夹结构:
   ```
   Source/[ProjectName]/
   ├── Public/
   ├── Private/
   └── [ProjectName].Build.cs
   Content/
   ├── Blueprints/
   ├── Data/
   ├── Materials/
   └── UI/
   ```
3. 配置多人游戏项目设置
4. 启用 GAS 插件集成
5. 设置基础输入映射
6. 创建输入-能力绑定数据资产（`UInputAbilityBindingData`），配置 Input.* 与 Ability.* 标签映射
7. 在 PlayerController 中实现通用输入到 Ability 激活桥接（Started 激活 / Completed 取消）

## 开发约定

### 语言和文档
- **所有文档和代码注释必须使用中文**
- 技术术语可适当使用英文 (如 "GAS", "UE5", "Component")
- 遵循现有文档的格式和风格

### 代码组织
- **基类**: `Source/[ProjectName]/Core/`
- **组件**: `Source/[ProjectName]/Components/`
- **GAS 类**: `Source/[ProjectName]/AbilitySystem/`
- **数据资产**: `Content/Data/`
- **UI 类**: `Source/[ProjectName]/UI/`

### GAS 集成要求
- 所有游戏机制(射击、技能、伤害)必须使用 GameplayAbilities
- 角色属性实现为 AttributeSet
- 使用 GameplayEffects 处理伤害、增益和减益效果
- 使用 GameplayTags 对能力和效果进行分类

### 多人游戏考虑
- 游戏关键变量使用 UPROPERTY(Replicated)
- 服务器权威操作使用 Server RPC
- 实现客户端预测以提升响应性
- 考虑频繁更新的带宽优化

### 数据资产模式
- 为物品、技能、武器创建基础 DataAsset 类
- 使用继承层次结构实现特化物品类型
- 所有平衡数值存储在数据资产中，而非代码
- 实现数据完整性验证函数

## 当前开发状态

- ✅ 完成综合设计文档
- ✅ 完成参考框架分析
- ❌ UE5 项目设置 (未开始)
- ❌ 基类实现 (未开始)
- ❌ GAS 集成设置 (未开始)

## 下一步开发计划

1. 创建具有适当设置的 UE5 项目
2. 实现基础 Character 和 Controller 类
3. 设置 GAS 基础 (AbilitySystemComponent, 基础 AttributeSet)
4. 基于文档创建模块化组件系统
5. 实现配置的数据资产框架

## 文档资源

每个主要系统在各自文件夹中都有专门的中文文档:
- `/基类/` - 基类规范和继承模式
- `/射击框架/` - 武器系统架构和组件
- `/GAS集成/` - 能力系统集成模式
- `/库存/` - 库存系统设计和物品定义
- `/技能/` - 技能系统架构和进程
- `/随机地图/` - 程序化生成算法和房间设计