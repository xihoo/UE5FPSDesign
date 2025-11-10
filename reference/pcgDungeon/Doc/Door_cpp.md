# Door.cpp

## 概述

`Door.cpp` 文件包含了 `ADoor` 类的实现。这个类是地牢中所有门 Actor 的基类。它负责管理门的可见性、锁定状态和打开状态，并处理与房间的连接。

## 函数实现

### ADoor()

```cpp
ADoor::ADoor()
```

构造函数。初始化门 Actor 的基本属性：

- 启用 Tick。
- 启用复制。
- 设置为始终相关，以防止当服务器上的玩家距离太远时门在客户端上消失。
- 设置网络休眠状态为 `DORM_DormantAll`。
- 创建并设置默认场景组件作为根组件。

### GetLifetimeReplicatedProps()

```cpp
void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
```

定义需要复制的属性。复制的属性包括：

- `bShouldBeLocked`: 门是否应该被锁定。
- `bShouldBeOpen`: 门是否应该打开。
- `RoomA`: 连接到此门的第一个房间。
- `RoomB`: 连接到此门的第二个房间。

### Tick()

```cpp
void ADoor::Tick(float DeltaTime)
```

每帧调用。处理门的逻辑：

1. **可见性管理**:
   - 如果门是由地牢生成器生成的（至少一个房间有效），则根据连接的房间的可见性更新门的可见性。
   - 门仅在满足以下所有条件时隐藏：
     - 启用了房间剔除。
     - 门不是“始终可见”。
     - 连接的两个房间都不可见。

2. **锁定状态管理**:
   - 更新门的锁定状态。门在满足以下所有条件时被锁定：
     - 门不是“始终未锁定”。
     - 用户指定门应该被锁定。
     - 门是由地牢生成器生成的，并且其中一个连接的房间被锁定或缺失。
   - 如果锁定状态发生变化，则调用相应的事件函数。

3. **打开状态管理**:
   - 更新门的打开状态。门在满足以下条件时打开：
     - 用户指定门应该打开。
     - 门未被锁定。
   - 如果打开状态发生变化，则调用相应的事件函数。

4. **调试绘制**:
   - 如果启用了调试绘制并且世界类型为 `EditorPreview`，则绘制门的调试信息。

### SetConnectingRooms()

```cpp
void ADoor::SetConnectingRooms(URoom* _RoomA, URoom* _RoomB)
```

设置连接到此门的两个房间。此函数只能在具有权威性的服务器上调用。

### Open()

```cpp
void ADoor::Open(bool bOpen)
```

打开或关闭门。此函数只能在具有权威性的服务器上调用。

### Lock()

```cpp
void ADoor::Lock(bool bLock)
```

锁定或解锁门。此函数只能在具有权威性的服务器上调用。