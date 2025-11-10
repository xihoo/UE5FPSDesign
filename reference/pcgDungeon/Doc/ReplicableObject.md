# ReplicableObject 类

## 概述

`UReplicableObject` 是一个基类，用于可以被复制的子对象。这个类不是蓝图可生成的，也不应该直接在蓝图中使用。

## 枚举

### EUnregisterSubObjectType

- `Unregister`: 从子对象列表中移除。远程的子对象将在权威端被垃圾回收时销毁。
- `Destroy`: 立即在远程销毁子对象。权威端稍后负责销毁它。隐含 Unregister。
- `TearOff`: 断开此子对象的复制，因此它在远程被视为本地子对象。隐含 Unregister。

## 结构体

### FRegisterSubObjectParams

- `NetCondition` (ELifetimeCondition): 注册子对象时的网络条件。在注销时未使用。
- `UnregisterType` (EUnregisterSubObjectType): 子对象如何注销。在注册时未使用。

## 成员函数

### UObject 接口

- `GetWorld()`: 获取世界。
- `IsSupportedForNetworking()`: 是否支持网络。
- `RegisterReplicationFragments(...)`: 注册复制片段（仅在 UE_WITH_IRIS 启用时）。

### 复制相关

- `ReplicateSubobject(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)`: 替代 Channel->ReplicateSubobject(...) 调用。被认为是“遗留的”（当所有者 Actor 中的“Use Registered Subobject List”为 false 时使用）。
- `RegisterAsReplicable(bool bRegister, FRegisterSubObjectParams Params = FRegisterSubObjectParams())`: 调用其所有者的 AddReplicatedSubObject。如果子对象是使用 CreateDefaultSubobject 创建的，则在 PostInitializeComponent 期间调用。当所有者 Actor 中的“Use Registered Subobject List”为 true 时使用。
- `ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)`: 复制可能的嵌套子对象。不应直接调用，除非在子类中重写的函数中调用。被认为是“遗留的”（当所有者 Actor 中的“Use Registered Subobject List”为 false 时使用）。
- `RegisterReplicableSubobjects(bool bRegister)`: 重写此函数以将此对象的子对象注册为可复制。当所有者 Actor 中的“Use Registered Subobject List”为 true 时使用。
- `HasAuthority()`: 返回拥有 Actor 是否具有权威性。
- `GetAuthorityName()`: 根据 HasAuthority() 结果返回“Server”或“Client”。
- `WakeUpOwnerActor()`: 从休眠中唤醒拥有 Actor，以指示复制变量将发生变化。你必须在修改复制变量*之前*使用它。