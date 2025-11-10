# ProceduralDungeonTypes 类

## 概述

`ProceduralDungeonTypes.h` 定义了 Procedural Dungeon 插件使用的核心类型和枚举，包括生成状态、门方向、生成类型、种子类型、可见性模式以及门定义和边界框结构。

## 枚举

### EGenerationState

表示地牢生成器的不同状态。

- `Idle`: 空闲状态。
- `Generation`: 正在生成地牢。
- `Load`: 正在加载地牢。
- `Initialization`: 正在初始化地牢。
- `Unload`: 正在卸载地牢。
- `Play`: 游戏进行中。
- `NbState`: 状态数量（隐藏）。

### EDoorDirection

表示门可以面向的不同方向。

- `North`: 北（旋转 0 度，正 X 轴）。
- `East`: 东（旋转 90 度，正 Y 轴）。
- `South`: 南（旋转 180 度，负 X 轴）。
- `West`: 西（旋转 270 度，负 Y 轴）。
- `NbDirection`: 方向数量（隐藏）。

### EGenerationType

表示不同的生成算法类型。

- `DFS`: 深度优先搜索（使地牢更线性）。
- `BFS`: 广度优先搜索（使地牢不那么线性）。
- `NbType`: 类型数量（隐藏）。

### ESeedType

表示每次生成时种子的更新类型。

- `Random`: 每次生成时使用随机种子。
- `AutoIncrement`: 获取初始种子并在每次生成时递增。
- `Fixed`: 始终使用初始种子（或通过蓝图手动设置）。
- `NbType`: 类型数量（隐藏）。

### EVisibilityMode

表示房间可见性组件的可见性模式。

- `Default`: 与房间相同。
- `ForceVisible`: 强制可见。
- `ForceHidden`: 强制隐藏。
- `Custom`: 自定义。
- `NbMode`: 模式数量（隐藏）。

## 结构体

### FDoorDef

定义一个门的结构。门由其位置、方向和类型定义。

#### 成员变量

- `Position` (FIntVector): 门的位置。
- `Direction` (EDoorDirection): 门的方向。
- `Type` (UDoorType*): 门的类型。

#### 成员函数

- `FDoorDef()`: 默认构造函数。
- `FDoorDef(const FIntVector& InPosition, EDoorDirection InDirection, class UDoorType* InType = nullptr)`: 构造函数。
- `IsValid()`: 检查门定义是否有效。
- `operator bool()`: 转换为布尔值。
- `operator==(const FDoorDef& Other)`: 比较两个门定义是否相等。
- `AreCompatible(const FDoorDef& A, const FDoorDef& B)`: 检查两个门定义是否兼容。
- `GetDoorSize()`: 获取门的大小。
- `GetDoorOffset()`: 获取门的偏移量。
- `GetDoorColor()`: 获取门的颜色。
- `GetTypeName()`: 获取门类型的名称。
- `ToString()`: 转换为字符串。
- `GetOpposite()`: 获取相反方向的门定义。
- `GetBounds(bool bIncludeOffset = true)`: 获取门的边界框。
- `GetRealDoorPosition(const FDoorDef& DoorDef, bool bIncludeOffset = true)`: 获取门的真实位置。
- `GetRealDoorPosition(FIntVector DoorCell, EDoorDirection DoorRot, float DoorOffset = 0.0f)`: 获取门的真实位置。
- `GetRealDoorRotation(const FDoorDef& DoorDef, bool bFlipped = false)`: 获取门的真实旋转。
- `Transform(const FDoorDef& DoorDef, FIntVector Translation, EDoorDirection Rotation)`: 变换门定义。
- `InverseTransform(const FDoorDef& DoorDef, FIntVector Translation, EDoorDirection Rotation)`: 逆变换门定义。
- `DrawDebug(...)`: 绘制调试信息（仅在非发布版本中可用）。

### FBoxMinAndMax

表示一个由最小和最大整数向量定义的边界框。

#### 成员变量

- `Min` (FIntVector): 最小点。
- `Max` (FIntVector): 最大点。

#### 成员函数

- `FBoxMinAndMax()`: 默认构造函数。
- `FBoxMinAndMax(const FIntVector& A, const FIntVector& B)`: 构造函数。
- `GetSize()`: 获取边界框的大小。
- `ToCenterAndExtent()`: 转换为中心点和范围。
- `IsInside(const FIntVector& Cell)`: 检查点是否在边界框内。
- `IsInside(const FBoxMinAndMax& Other)`: 检查另一个边界框是否在当前边界框内。
- `Rotate(const EDoorDirection& Rot)`: 旋边界框。
- `Extend(const FBoxMinAndMax& Other)`: 扩展边界框以包含另一个边界框。
- `ToString()`: 转换为字符串。
- `GetClosestPoint(const FIntVector& Point)`: 获取边界框内最接近给定点的点。
- `Overlap(const FBoxMinAndMax& A, const FBoxMinAndMax& B)`: 检查两个边界框是否重叠。
- 各种运算符重载（`+=`, `-=`, `+`, `-`, `==`, `!=`）。

## 函数

### EDoorDirection 相关函数

- `operator!(const EDoorDirection& Direction)`: 逻辑非运算符。
- `operator+(const EDoorDirection& A, const EDoorDirection& B)`: 加法运算符。
- `operator-(const EDoorDirection& A, const EDoorDirection& B)`: 减法运算符。
- `operator+=(EDoorDirection& A, const EDoorDirection& B)`: 加赋值运算符。
- `operator-=(EDoorDirection& A, const EDoorDirection& B)`: 减赋值运算符。
- `operator++(EDoorDirection& Direction)`: 前置递增运算符。
- `operator--(EDoorDirection& Direction)`: 前置递减运算符。
- `operator++(EDoorDirection& Direction, int)`: 后置递增运算符。
- `operator--(EDoorDirection& Direction, int)`: 后置递减运算符。
- `operator-(const EDoorDirection& Direction)`: 负号运算符。
- `operator~(const EDoorDirection& Direction)`: 按位取反运算符（获取相反方向）。
- `Opposite(const EDoorDirection& Direction)`: 获取相反方向。
- `ToIntVector(const EDoorDirection& Direction)`: 转换为整数向量。
- `ToVector(const EDoorDirection& Direction)`: 转换为向量。
- `ToQuaternion(const EDoorDirection& Direction)`: 转换为四元数。
- `ToAngle(const EDoorDirection& Direction)`: 转换为角度。
- `Rotate(const FIntVector& Pos, const EDoorDirection& Rot)`: 旋转向量。
- `Rotate(const FVector& Pos, const EDoorDirection& Rot)`: 旋转向量。
- `Transform(const FIntVector& Pos, const FIntVector& Translation, const EDoorDirection& Rotation)`: 变换位置。
- `InverseTransform(const FIntVector& Pos, const FIntVector& Translation, const EDoorDirection& Rotation)`: 逆变换位置。
- `Transform(const EDoorDirection& Direction, const EDoorDirection& Rotation)`: 变换方向。
- `InverseTransform(const EDoorDirection& Direction, const EDoorDirection& Rotation)`: 逆变换方向。

### FBoxMinAndMax 相关函数

- `Rotate(const FBoxMinAndMax& Box, const EDoorDirection& Rot)`: 旋转边界框。