# ProceduralDungeonEdTypes.cpp

## 概述

`ProceduralDungeonEdTypes.cpp` 是 `ProceduralDungeonEdTypes.h` 的实现文件。它包含了为 `EAxisList::Type` 枚举定义的运算符重载的实现，以及 `FMargin3D` 结构体中 `Apply` 方法的实现。

## 实现细节

### EAxisList::Type 运算符重载

- `EAxisList::Type operator~(const EAxisList::Type& This)`:
  - 实现按位取反操作。
  - 通过 `static_cast<EAxisList::Type>(EAxisList::All - This)` 计算结果。这里假设 `EAxisList::All` 代表所有位都为 1 的值，减去 `This` 可以得到取反的效果（这是一种特定于枚举值设计的实现方式）。

- `EAxisList::Type& operator&=(EAxisList::Type& This, const EAxisList::Type& Other)`:
  - 实现按位与并赋值操作。
  - 直接对 `This` 和 `Other` 进行位与操作 (`This & Other`)，然后将结果强制转换回 `EAxisList::Type` 并赋值给 `This`。
  - 返回修改后的 `This` 引用。

### FMargin3D::Apply

- `FBoxCenterAndExtent FMargin3D::Apply(const FBoxCenterAndExtent& Bounds) const`:
  - **功能**: 根据 `FMargin3D` 的值，对输入的 `Bounds` 进行扩展或收缩，并可能调整其中心位置，最后返回新的 `FBoxCenterAndExtent`。
  - **实现**:
    - 创建一个 `NewBounds`，初始化为输入的 `Bounds`。
    - **调整 Extent (大小)**:
      - 计算每个轴向的总扩展量：`XAxis.X + XAxis.Y`（正向扩展 + 负向扩展）。
      - 将总扩展量的一半 (`0.5f * ...`) 加到 `NewBounds.Extent` 上。这会均匀地扩大或缩小边界框。
    - **调整 Center (中心)**:
      - 计算每个轴向的偏移量：`XAxis.X - XAxis.Y`（正向扩展 - 负向扩展）。
      - 将偏移量的一半 (`0.5f * ...`) 加到 `NewBounds.Center` 上。如果正负方向扩展不一致，这会导致边界框中心发生偏移。
    - 返回调整后的新边界框 `NewBounds`。