# DungeonOctree.cpp

该文件包含了 `FDungeonOctreeElement` 结构体的实现，该结构体用于在八叉树中存储房间信息。

## 主要功能

- **存储房间信息**：`FDungeonOctreeElement` 结构体用于存储指向 `URoom` 对象的指针以及该房间的边界框 (`FBoxCenterAndExtent`)。

## 详细实现

### FDungeonOctreeElement

- **构造函数**：`FDungeonOctreeElement(URoom* Room)` 构造函数接受一个 `URoom` 指针作为参数，并初始化结构体的成员变量。
  - `this->Room = Room;`：将传入的房间指针存储在结构体中。
  - `Bounds = Room->GetBounds();`：获取房间的边界框并存储在结构体中。

这个结构体的主要目的是为了方便地将房间信息插入到八叉树中，以便进行快速的空间查询和可见性管理。