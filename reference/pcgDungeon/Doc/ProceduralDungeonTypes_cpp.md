# ProceduralDungeonTypes.cpp

该文件包含了 `ProceduralDungeon` 插件中使用的核心类型和结构体的实现，包括门方向枚举、门定义结构体和边界框结构体。

## 主要功能

- **门方向枚举操作符重载**：为 `EDoorDirection` 枚举提供了各种操作符重载，方便进行方向计算。
- **方向转换函数**：提供了将 `EDoorDirection` 转换为 `FIntVector`、`FVector`、`FQuat` 和角度的函数。
- **坐标变换函数**：提供了旋转和平移坐标的函数。
- **FDoorDef 结构体**：定义了门的基本属性和相关操作。
- **FBoxMinAndMax 结构体**：定义了用于表示整数边界框的结构体。

## 详细实现

### 门方向枚举操作符重载

- 重载了 `!`、`+`、`-`、`+=`、`-=`、`++`、`--`、`-`（取反）、`~`（取对）等操作符，用于对 `EDoorDirection` 枚举进行各种运算。

### 方向转换函数

- `ToIntVector`：将 `EDoorDirection` 转换为 `FIntVector`。
- `ToVector`：将 `EDoorDirection` 转换为 `FVector`。
- `ToQuaternion`：将 `EDoorDirection` 转换为 `FQuat`。
- `ToAngle`：将 `EDoorDirection` 转换为角度。

### 坐标变换函数

- `Rotate`：旋转 `FIntVector` 或 `FVector`。
- `Transform`：对坐标进行旋转和平移变换。
- `InverseTransform`：对坐标进行逆向变换。

### FDoorDef 结构体

- **成员变量**：
  - `Position`：门的位置（单元格坐标）。
  - `Direction`：门的方向。
  - `Type`：门的类型（`UDoorType`）。
- **构造函数**：`FDoorDef(const FIntVector& InPosition, EDoorDirection InDirection, UDoorType* InType = nullptr)`。
- **IsValid**：检查门定义是否有效。
- **operator==**：比较两个门定义是否相等。
- **AreCompatible**：检查两个门定义是否兼容（类型相同）。
- **GetDoorSize**：获取门的大小。
- **GetDoorOffset**：获取门的偏移。
- **GetDoorColor**：获取门的颜色。
- **GetTypeName**：获取门类型的名称。
- **ToString**：将门定义转换为字符串。
- **GetOpposite**：获取对面的门定义。
- **GetBounds**：获取门的边界框。
- **GetRealDoorPosition`：获取门在世界坐标中的实际位置。
- **GetRealDoorRotation`：获取门在世界坐标中的实际旋转。
- **Transform`：对门定义进行变换。
- **InverseTransform`：对门定义进行逆向变换。
- **DrawDebug`：在编辑器中绘制门的调试信息。

### FBoxMinAndMax 结构体

- **成员变量**：
  - `Min`：边界框的最小点。
  - `Max`：边界框的最大点。
- **构造函数**：`FBoxMinAndMax(const FIntVector& A, const FIntVector& B)`。
- **GetSize`：获取边界框的大小。
- **ToCenterAndExtent`：将边界框转换为 `FBoxCenterAndExtent`。
- **IsInside`：检查点或另一个边界框是否在当前边界框内。
- **Rotate`：旋转边界框。
- **Extend`：扩展边界框以包含另一个边界框。
- **ToString`：将边界框转换为字符串。
- **GetClosestPoint`：获取边界框内离指定点最近的点。
- **Overlap`：检查两个边界框是否重叠。
- **operator+=`、`operator-=`、`operator+`、`operator-`、`operator==`、`operator!=`：重载了各种操作符。
- **Rotate`（全局函数）：旋转边界框。

这个文件提供了插件中使用的核心数据结构和操作，是实现地牢生成和管理功能的基础。