# SimpleGuidComponent.cpp

该文件包含了 `USimpleGuidComponent` 类的实现，该类是一个简单的GUID组件，用于检索编辑器中的ActorGuid以便在打包游戏中保存/加载。

## 主要功能

- **GUID管理**：存储和提供Actor的GUID。
- **保存控制**：控制Actor是否应该在地牢中保存。
- **生命周期日志**：在调试模式下记录组件的生命周期事件。

## 详细实现

### 构造函数

- `USimpleGuidComponent`：构造函数。
  - 设置组件不需要每帧tick。
  - 如果启用了 `GUID_DEBUG_ENABLED`，则记录构造信息和GUID。

### 组件注册

- `OnRegister`：组件注册时的回调函数。
  - 如果启用了 `GUID_DEBUG_ENABLED`，则记录注册信息。
  - 调用父类的 `OnRegister`。
  - 如果GUID无效且拥有者Actor有效：
    - 在编辑器中，从Actor获取GUID。
    - 在非编辑器中，记录无法获取GUID的警告。

### GUID接口

- `GetGuid`：实现 `IRoomActorGuid` 接口的 `GetGuid` 函数，返回存储的GUID。
- `ShouldSaveActor`：实现 `IRoomActorGuid` 接口的 `ShouldSaveActor` 函数，返回 `bSaveActorInDungeon` 的值。

### 序列化

- `Serialize(FArchive&)`：序列化函数，记录序列化信息（如果启用了调试）。
- `Serialize(FStructuredArchive::FRecord)`：结构化序列化函数，记录序列化信息（如果启用了调试）。

### 调试生命周期函数

- `PostInitProperties`：属性初始化后调用，记录信息。
- `PreSave`：保存前调用，记录信息。
- `PostLoad`：加载后调用，记录信息。
- `OnComponentCreated`：组件创建时调用，记录信息。
- `InitializeComponent`：组件初始化时调用，记录信息。
- `BeginPlay`：游戏开始时调用，记录信息。

这个组件主要用于在编辑器中为Actor分配一个唯一的GUID，并在打包游戏中使用这个GUID来保存和加载Actor的状态。它只适用于放置在关卡中的Actor，不适用于运行时生成的Actor。