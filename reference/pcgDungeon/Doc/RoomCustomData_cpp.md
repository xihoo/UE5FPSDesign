# RoomCustomData.cpp

该文件包含了 `URoomCustomData` 类的实现，该类是房间自定义数据的基类。

## 主要功能

- **关卡组件创建**：在房间关卡中创建和附加组件。
- **序列化**：支持自定义数据的保存和加载。

## 详细实现

### 关卡组件创建

- `CreateComponentOnInstance`：在Actor实例上创建组件的辅助函数。
  - 检查Actor实例和组件类是否有效。
  - 确保不在模板Actor上创建组件。
  - 对于复制的组件，仅在服务器上创建。
  - 处理 `USceneComponent` 的附加逻辑。
  - 注册组件。
- `CreateLevelComponent`：在房间关卡中创建关卡组件。
  - 检查 `LevelComponent` 是否有效。
  - 调用 `CreateComponentOnInstance` 创建组件实例。
  - 将组件实例存储在 `LevelComponentInstance` 中。

### 序列化

- `SerializeObject`：序列化或反序列化组件数据。
  - 如果没有组件，则直接返回。
  - 创建 `SavedData` 结构体。
  - 在保存时，序列化组件数据。
  - 在加载时，反序列化组件数据。
  - 保存后重置 `SavedData`。
- `PreSaveDungeon`：保存地牢前的回调函数，调用组件的 `PreSaveDungeon`。
- `PostLoadDungeon`：加载地牢后的回调函数，反序列化组件数据并调用组件的 `PostLoadDungeon`。

这个类允许用户为房间实例添加自定义数据和组件，并支持这些数据和组件的网络复制和序列化。