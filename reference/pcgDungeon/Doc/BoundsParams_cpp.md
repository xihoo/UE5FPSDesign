# BoundsParams.cpp

## 概述

`BoundsParams.cpp` 文件包含了 `FBoundsParams` 结构体的实现。这个结构体用于定义地牢生成的边界限制。

## 函数实现

### GetBox()

```cpp
FBoxMinAndMax FBoundsParams::GetBox() const
```

此函数根据 `FBoundsParams` 结构体中的设置计算并返回一个 `FBoxMinAndMax` 对象，该对象表示地牢的边界框。

- 如果启用了某个轴的最小限制（例如 `bLimitMinX`），则该轴的最小值设置为负的限制值（例如 `-MinX`），否则设置为 `INT32_MIN`。
- 如果启用了某个轴的最大限制（例如 `bLimitMaxX`），则该轴的最大值设置为限制值加 1（例如 `MaxX + 1`），否则设置为 `INT32_MAX`。

这个边界框定义了地牢在各个轴向上的生成范围。