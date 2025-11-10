# TriggerType.cpp

该文件包含了 `UTriggerType` 类的实现，该类是一个触发器盒组件，用于管理Actor进入和离开触发器的检测。

## 主要功能

- **触发器管理**：检测Actor的进入和离开。
- **激活逻辑**：根据进入的Actor数量和延迟时间激活或停用触发器。
- **定时器**：定期执行触发器逻辑。
- **事件广播**：广播Actor进入、离开、触发器激活、停用和定时器事件。

## 详细实现

### 构造函数

- `UTriggerType`：构造函数。
  - 设置组件在游戏开始时初始化，并且每帧tick。
  - 初始化 `TickDuration` 为0.5秒。
  - 初始化 `ActivationDelay` 为0.0秒。

### 游戏开始

- `BeginPlay`：游戏开始时的初始化逻辑。
  - 调用父类的 `BeginPlay`。
  - 检查网络模式，如果不是客户端，则：
    - 绑定重叠事件到 `OnTriggerEnter` 和 `OnTriggerExit`。
    - 设置定时器 `TickTimer`，定期调用 `TriggerTick`。

### 触发器事件

- `OnTriggerEnter`：Actor进入触发器时的回调函数。
  - 检查Actor类型是否匹配 `ActorType`。
  - 如果Actor不在列表中，则添加到列表并广播 `OnActorEnter` 事件。
  - 如果进入的Actor数量达到 `requiredActorCountToActivate`，则：
    - 如果 `ActivationDelay` 大于0，则设置定时器 `ActivationTimer` 在延迟后调用 `TriggerActivate`。
    - 否则直接调用 `TriggerActivate`。
- `OnTriggerExit`：Actor离开触发器时的回调函数。
  - 检查Actor类型是否匹配 `ActorType`。
  - 如果Actor在列表中，则从列表中移除并广播 `OnActorExit` 事件。
  - 清除 `ActivationTimer` 定时器。
  - 调用 `TriggerDeactivate`。

### 定时器事件

- `TriggerTick`：定时器回调函数，广播 `OnTriggerTick` 事件。
- `TriggerActivate`：激活触发器，设置 `bIsActivated` 为 `true` 并广播 `OnActivation` 事件。
- `TriggerDeactivate`：停用触发器，设置 `bIsActivated` 为 `false` 并广播 `OnDeactivation` 事件。

这个类提供了一个灵活的触发器组件，开发者可以通过设置属性和绑定事件来自定义触发器的行为。