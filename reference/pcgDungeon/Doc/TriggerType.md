# TriggerType 类

## 概述

`UTriggerType` 类是一个触发器框，用于管理检测进入和退出触发器的 Actor。可以定期执行游戏逻辑，并且在激活前可以有可选的延迟。你可以通过设置 `ActorType` 属性来自定义可以激活触发器的 Actor 类型。它继承自 `UBoxComponent`。

## 成员变量

- `TickDuration` (float): 触发器滴答的持续时间。
- `ActivationDelay` (float): 激活前的延迟时间。
- `requiredActorCountToActivate` (uint8): 激活所需的 Actor 数量。
- `ActorType` (TSubclassOf<AActor>): 可以激活触发器的 Actor 类型。
- `bIsActivated` (bool): 触发器是否已激活。
- `ActorList` (TArray<AActor*>): 在触发器内的 Actor 列表。
- `TickTimer` (FTimerHandle): 滴答计时器。
- `ActivationTimer` (FTimerHandle): 激活计时器。

## 成员函数

### 构造函数

- `UTriggerType()`: 构造函数。

### 虚函数重写

- `BeginPlay()`: 开始游戏时调用。

### 蓝图可调用函数

- `IsActivated()`: 触发器是否已激活。
- `GetActorList()`: 获取在触发器内的 Actor 列表。

### 事件

- `OnActorEnter`: 当 Actor 进入时触发。
- `OnActorExit`: 当 Actor 退出时触发。
- `OnTriggerTick`: 当触发器滴答时触发。
- `OnActivation`: 当触发器激活时触发。
- `OnDeactivation`: 当触发器停用时触发。

### 私有函数

- `OnTriggerEnter(...)`: 触发器进入时调用。
- `OnTriggerExit(...)`: 触发器退出时调用。
- `TriggerTick()`: 触发器滴答时调用。
- `TriggerActivate()`: 触发器激活时调用。
- `TriggerDeactivate()`: 触发器停用时调用。