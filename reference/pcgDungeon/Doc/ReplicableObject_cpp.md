# ReplicableObject.cpp

该文件包含了 `UReplicableObject` 类的实现，该类是一个基类，用于支持网络复制的子对象。

## 主要功能

- **网络复制**：提供网络复制子对象的功能。
- **注册管理**：管理子对象在网络复制中的注册和注销。
- **权限检查**：检查拥有者Actor是否具有网络权限。
- **世界访问**：提供获取世界对象的接口。

## 详细实现

### 网络复制

- `ReplicateSubobject`：
  - 确保嵌套对象在网络复制之前被复制。
  - 调用 `ReplicateSubobjects` 复制嵌套子对象。
  - 调用 `Channel->ReplicateSubobject` 复制当前对象。

### 注册管理

- `RegisterAsReplicable`：
  - 获取拥有者Actor。
  - 检查Actor是否有效且具有权限。
  - 检查Actor是否使用注册的子对象列表。
  - 检查对象是否已经注册或注销。
  - 根据 `bRegister` 参数注册或注销子对象。
  - 支持不同的注销类型（`Unregister`、`Destroy`、`TearOff`）。
  - 调用 `RegisterReplicableSubobjects` 注册或注销嵌套的可复制子对象。

### Iris 支持

- `RegisterReplicationFragments`：
  - 为对象创建和注册复制片段。

### 嵌套子对象复制

- `ReplicateSubobjects`：
  - 虚函数，用于复制嵌套的子对象。默认返回 `false`。

### 世界访问

- `GetWorld`：
  - 获取对象所在的世界。

### 权限检查

- `HasAuthority`：
  - 检查拥有者Actor是否具有权限。
- `GetAuthorityName`：
  - 根据权限返回 "Server" 或 "Client"。

### 唤醒拥有者Actor

- `WakeUpOwnerActor`：
  - 唤醒拥有者Actor，使其从休眠状态中恢复，以便复制变量的变化。

这个类为需要网络复制功能的子对象提供了一个通用的基类，简化了网络复制的实现。