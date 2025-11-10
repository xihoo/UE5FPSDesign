# DeterministicRandomComponent.cpp

该文件包含了 `UDeterministicRandomComponent` 类的实现，该类是一个简单的组件，为放置在房间关卡中的任何Actor添加一个随机流。

## 主要功能

- **确定性随机数生成**：使用Actor的GUID和所属房间的ID生成一个确定性的初始种子。
- **随机数流管理**：提供一个 `FRandomStream` 对象，用于生成随机数。

## 详细实现

### 构造函数

- `UDeterministicRandomComponent`：构造函数。
  - 设置组件不需要每帧tick。

### 组件注册

- `OnRegister`：组件注册时的回调函数。
  - 调用父类的 `OnRegister`。
  - 获取拥有者Actor。
  - 调用 `GenerateDeterministicSeed` 生成确定性种子。
  - 使用生成的种子初始化 `Random` 对象。
  - 记录调试日志，显示初始种子。

### 确定性种子生成

- `GenerateDeterministicSeed`：生成确定性种子的静态函数。
  - 初始化 `Guid` 和 `Salt`。
  - 如果Actor实现了 `IRoomActorGuid` 接口，则获取其GUID。
  - 如果Actor有所属房间，则获取房间ID作为Salt。
  - 如果房间有生成器，则将生成器的GUID与Actor的GUID进行异或运算，并将生成器的种子与Salt进行异或运算。
  - 调用 `Random::Guid2Seed` 函数，使用GUID和Salt生成32位种子。

### 辅助函数

- `operator^` 和 `operator^=`：用于对两个 `FGuid` 对象进行异或运算的辅助函数。

这个组件确保了在相同地牢生成条件下，Actor的行为是可重现的，因为它们的随机数种子是基于地牢种子和Actor自身信息确定的。