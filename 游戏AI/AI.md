# 游戏AI设计文档

## 概述

本文档定义了游戏中敌人AI的架构，包括行为树、感知系统、战斗AI和导航系统。AI系统负责提供具有挑战性和多样性的敌人行为。

## 设计原则

1. **行为树驱动** - 使用UE5的Behavior Tree系统
2. **感知系统** - 利用AI Perception组件
3. **多样化行为** - 不同敌人类型有不同行为模式
4. **小队协作** - 支持敌人间的简单协作
5. **性能优化** - LOD和更新频率控制

## 核心架构

### AI类层次

```
AFPSAIController
├── BehaviorTree
├── BlackboardComponent
├── AIPerceptionComponent
└── EQS Queries

AFPSAICharacter (敌人角色)
├── AI行为状态
├── 仇恨系统
├── 巡逻路径
└── 攻击模式
```

---

## 核心实现

### 1. AI控制器

```cpp
UCLASS()
class AFPSAIController : public AAIController
{
    GENERATED_BODY()
    
public:
    virtual void OnPossess(APawn* InPawn) override;
    
protected:
    UPROPERTY(EditDefaultsOnly)
    UBehaviorTree* BehaviorTree;
    
    // AI感知
    UPROPERTY(VisibleAnywhere)
    UAIPerceptionComponent* AIPerception;
    
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
    
    // 战斗逻辑
    void StartAttacking();
    void StopAttacking();
    void FindCover();
    void TakeCover();
};
```

### 2. 行为树结构

```
BT_Enemy (敌人行为树)
├── Selector: Combat or Patrol
│   ├── Sequence: Combat
│   │   ├── Has Target?
│   │   ├── Move to Combat Range
│   │   ├── Face Target
│   │   └── Attack
│   └── Sequence: Patrol
│       ├── Move to Patrol Point
│       └── Wait
└── Decorator: Is Alive
```

### 3. 黑板键

```
Blackboard Keys:
- TargetActor: Actor (当前目标)
- PatrolPoint: Vector (巡逻点)
- LastKnownPosition: Vector (最后已知位置)
- CoverLocation: Vector (掩体位置)
- bIsInCombat: Bool (战斗状态)
- bCanSeeTarget: Bool (是否看见目标)
```

---

## AI行为模式

### 1. 近战AI

```
行为特征:
- 快速接近玩家
- 使用近战攻击
- 被击退后重新冲锋

行为树:
- Chase Player
- Melee Attack
- Retreat if Low HP
```

### 2. 远程AI

```
行为特征:
- 保持中等距离
- 使用掩体
- 压制射击

行为树:
- Find Cover
- Move to Cover
- Peek and Shoot
- Retreat if Flanked
```

### 3. Boss AI

```
行为特征:
- 多阶段战斗
- 特殊技能
- 召唤小怪

行为树:
- Phase Check
- Special Attack Pattern
- Summon Minions
- Area Denial
```

---

## AI感知配置

```cpp
// 视觉配置
Sight Config:
- Sight Radius: 2000
- Lose Sight Radius: 2500
- Peripheral Vision: 90度
- Detection by Affiliation: Detect Enemies

// 听觉配置
Hearing Config:
- Hearing Range: 1500
- Detection by Affiliation: Detect Enemies
```

---

## 相关文档

- [基类设计文档](../基类/基类.md)
- [GAS集成文档](../GAS集成/GAS集成.md)
- [随机地图文档](../随机地图/随机地图.md)
