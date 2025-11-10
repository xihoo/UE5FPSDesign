# TriggerDoor.cpp

该文件包含了 `ATriggerDoor` 类的实现，该类是一个当Actor进入触发器盒时打开的门。

## 主要功能

- **触发器管理**：使用 `UBoxComponent` 作为触发器，检测Actor的进入和离开。
- **门开关控制**：根据进入触发器的Actor数量自动开关门。
- **Actor有效性验证**：提供接口验证哪些Actor可以触发门的开关。

## 详细实现

### 构造函数

- `ATriggerDoor`：构造函数。
  - 创建 `UBoxComponent` 作为触发器盒。
  - 将触发器盒附加到根组件。

### 游戏开始

- `BeginPlay`：游戏开始时的初始化逻辑。
  - 调用父类的 `BeginPlay`。
  - 检查触发器盒是否有效。
  - 绑定触发器盒的重叠事件到 `OnTriggerEnter` 和 `OnTriggerExit`。

### 触发器事件

- `OnTriggerEnter`：Actor进入触发器时的回调函数。
  - 检查Actor是否已经在列表中。
  - 调用 `IsValidActor` 验证Actor是否有效。
  - 如果Actor有效且不在列表中，则添加到列表并调用 `UpdateOpenState`。
- `OnTriggerExit`：Actor离开触发器时的回调函数。
  - 检查Actor是否在列表中。
  - 如果在列表中，则从列表中移除并调用 `UpdateOpenState`。

### Actor有效性验证

- `IsValidActor`：验证Actor是否可以触发门的开关。
  - 默认实现检查Actor是否为 `ACharacter` 且组件是否为 `UCapsuleComponent`，并且该组件是角色的胶囊组件。

### 门开关更新

- `UpdateOpenState`：根据Actor列表中的Actor数量更新门的开关状态。
  - 如果列表中有Actor，则开门；否则关门。

这个类提供了一个基础的触发门实现，开发者可以通过重写 `IsValidActor` 函数来自定义哪些Actor可以触发门的开关。