# DungeonOctree 类

## 概述

`FDungeonOctree` 是一个用于地牢系统的八叉树实现。它用于管理和查询地牢中房间的空间分布，以优化诸如遮挡剔除（occlusion culling）等操作的性能。

## 结构体

### FDungeonOctreeElement

表示八叉树中的一个元素，通常对应地牢中的一个房间。

- `Room` (class URoom*): 指向房间对象的指针。
- `Bounds` (FBoxCenterAndExtent): 房间的边界框（中心点和范围）。
- `FDungeonOctreeElement(URoom* Room)`: 构造函数，使用房间指针初始化元素。

### FDungeonOctreeSemantics

定义了八叉树的行为和配置。

- `MaxElementsPerLeaf`: 每个叶子节点中允许的最大元素数量（16）。
- `MinInclusiveElementsPerNode`: 每个节点中包含的最小元素数量（7）。
- `MaxNodeDepth`: 八叉树的最大深度（12）。
- `ElementAllocator`: 元素分配器类型，使用 `TInlineAllocator<MaxElementsPerLeaf>`。
- `GetBoundingBox(const FDungeonOctreeElement& Element)`: 获取元素的边界框。
- `AreElementsEqual(const FDungeonOctreeElement& A, const FDungeonOctreeElement& B)`: 比较两个元素是否相等。
- `SetElementId(const FDungeonOctreeElement& Element, FOctreeElementId Id)`: 设置元素的 ID（旧版八叉树）。
- `SetElementId(const FDungeonOctreeElement& Element, FOctreeElementId2 Id)`: 设置元素的 ID（新版八叉树）。
- `ApplyOffset(FDungeonOctreeElement& Element, FVector Offset)`: 对元素应用偏移量。

## 类型定义

- `FDungeonOctree`: 根据引擎版本使用 `TOctree` 或 `TOctree2` 实例化。

## 函数

### FindElementsWithBoundsTest

```cpp
template<typename IterateBoundsFunc>
inline void FindElementsWithBoundsTest(const FDungeonOctree& Octree, const FBoxCenterAndExtent& Bounds, const IterateBoundsFunc& Func)
```

在给定的边界框内查找八叉树元素，并对每个找到的元素执行指定的函数。

- `Octree`: 要查询的八叉树。
- `Bounds`: 用于查询的边界框。
- `Func`: 对每个找到的元素执行的函数。


FDungeonOctree 是 ProceduralDungeon 插件中用于优化房间可见性管理（遮挡剔除）性能的一个空间数据结构。

  1. 定义与本质

   * 八叉树 (Octree): 它是一种树形数据结构，用于在三维空间中组织和划分物体。每个内部节点最多有八个子节点，对应于将三维空间递归地划分为八个相等大小的子空间（八分体）。
   * `FDungeonOctree`: 这是插件对八叉树的具体实现（可能基于 UE 引擎提供的 TOctree 或 TOctree2 模板）。它被设计用来高效地存储和查询地牢中房间的空间信息。

  2. 核心功能与用途

  FDungeonOctree 的主要目的是加速“视锥体剔除”或“范围查询”，特别是在确定哪些房间在玩家附近、需要被考虑进行可见性计算时。

   * 为什么需要它？
       * 在一个大型地牢中，可能有成百上千个房间。如果每次计算可见性（例如，在 ADungeonGeneratorBase::UpdateRoomVisibility
         中）都需要遍历所有房间来检查它们是否在玩家周围一定距离内，这个操作会非常耗时（时间复杂度 O(N)）。
       * 八叉树提供了一种更高效的方法。通过将房间按空间位置组织起来，查询时只需要检查玩家所在或附近的几个八分体，而无需检查所有房间，从而大大减少了计算量（理想情况下接近
         O(log N)）。

   * 在 ProceduralDungeon 中的用途:
       * 存储房间: FDungeonOctree 存储的是 FDungeonOctreeElement，每个元素代表一个 URoom 实例及其边界框 (FBoxCenterAndExtent)。
       * 快速查询: 当需要找出玩家周围一定范围内的所有房间时（例如，用于 OcclusionDistance 的计算），系统可以使用八叉树进行范围查询
         (FindElementsWithBoundsTest)。这比遍历所有房间要快得多。
       * 与 `UpdateRoomVisibility` 的关系:
           * 虽然 UpdateRoomVisibility 的核心逻辑是基于图遍历 (UDungeonGraph::TraverseRooms)
             来查找连接的房间，但在某些情况下（或者作为优化），八叉树可以用来快速找到物理上靠近玩家的房间集合，作为进一步处理（如图遍历）的输入或进行快速的初步筛选。
           * 更直接地，八叉树可以用于视锥体剔除（Frustum
             Culling）：结合摄像机的视锥体，快速找出视锥体内的房间，这些房间才可能是可见的，从而进一步缩小需要进行复杂可见性计算（如图遍历）的范围。

  3. 工作流程

   4. 构建:
       * 当地牢生成完成或发生重大变化时（例如，在 ADungeonGeneratorBase::UpdateOctree 中），系统会遍历 UDungeonGraph 中的所有 URoom 实例。
       * 对于每个 URoom，创建一个 FDungeonOctreeElement，其中包含指向 URoom 的指针和该房间的 FBoxCenterAndExtent（通过 URoom::GetBounds() 获取）。
       * 将这个 FDungeonOctreeElement 插入到 FDungeonOctree 中。八叉树会根据元素的边界框自动将其放置在合适的节点和层级上。

   5. 查询:
       * 当需要进行空间查询时（例如，找出与某个边界框 QueryBounds 相交的所有房间）：
       * 调用 FindElementsWithBoundsTest (或类似函数)。
       * 传入 FDungeonOctree 实例和 QueryBounds。
       * 八叉树会高效地遍历其内部结构，只访问那些可能包含与 QueryBounds 相交元素的节点。
       * 对于找到的每个相关元素，执行指定的回调函数（例如，将房间添加到一个列表中）。

  6. 与 `DungeonGraph` 和 `UpdateRoomVisibility` 的关系

   * UDungeonGraph 是房间连接关系的管理者（图结构）。
   * FDungeonOctree 是房间空间位置的管理者（空间划分结构）。
   * ADungeonGeneratorBase::UpdateRoomVisibility 是可见性计算的逻辑中心。
   * 协同工作: UpdateRoomVisibility 首先需要知道玩家在哪里 (GetVisibilityPawn)，然后找到玩家所在的房间。这个“找到玩家在哪”的过程，或者后续查找附近房间的过程，就可以利用
     FDungeonOctree 来加速。虽然核心的可见性传播（OcclusionDistance）依赖于 UDungeonGraph 的连接，但八叉树可以作为一个高效的预筛选或辅助查询工具，提升性能。

  总结:

  FDungeonOctree 是一个用于优化空间查询的八叉树数据结构。在 ProceduralDungeon 中，它主要用于高效地存储和检索地牢房间的空间信息。虽然核心的房间可见性逻辑（基于连接）由
  UDungeonGraph 和 UpdateRoomVisibility 处理，但 FDungeonOctree
  作为一个性能优化工具，可以加速查找特定空间范围内的房间，从而提高整个可见性系统（尤其是处理大型地牢时）的效率。