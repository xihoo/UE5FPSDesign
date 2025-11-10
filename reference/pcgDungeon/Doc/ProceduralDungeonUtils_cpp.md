# ProceduralDungeonUtils.cpp

该文件包含了 `ProceduralDungeon` 插件中使用的各种实用工具函数的实现。

## 主要功能

- **整数向量操作**：提供 `FIntVector` 的最小值、最大值和范围计算函数。
- **坐标转换**：提供世界坐标和房间单元坐标之间的转换函数。
- **插件设置访问**：提供访问插件设置的便捷函数。
- **随机数生成**：提供基于 GUID 和盐值生成种子的函数。
- **对象遍历**：提供遍历对象及其子对象的函数。

## 详细实现

### 整数向量操作

- `IntVector::Min`：返回两个 `FIntVector` 的分量最小值。
- `IntVector::Max`：返回两个 `FIntVector` 的分量最大值。
- `IntVector::MinMax`：同时计算两个 `FIntVector` 的分量最小值和最大值。

### 坐标转换

- `Dungeon::ToWorldLocation`：将房间单元坐标转换为世界坐标（带偏移）。
- `Dungeon::ToWorldVector`：将房间单元向量转换为世界向量（无偏移）。
- `Dungeon::ToWorld`：将 `FBoxMinAndMax` 或 `FBoxCenterAndExtent` 转换为世界坐标。
- `Dungeon::ToRoomLocation`：将世界坐标转换为房间单元坐标。
- `Dungeon::ToRoomVector`：将世界向量转换为房间单元向量（无偏移）。
- `Dungeon::SnapPoint`：将世界坐标点对齐到最近的房间单元点。

### 插件设置访问

- `Dungeon::RoomUnit`：获取房间单元大小。
- `Dungeon::DefaultDoorSize`：获取默认门大小。
- `Dungeon::DefaultDoorColor`：获取默认门颜色。
- `Dungeon::DoorOffset`：获取门偏移。
- `Dungeon::OcclusionCulling`：获取是否启用遮挡剔除。
- `Dungeon::UseLegacyOcclusion`：获取是否使用旧版遮挡剔除。
- `Dungeon::OcclusionDistance`：获取遮挡距离。
- `Dungeon::OccludeDynamicActors`：获取是否遮挡动态Actor。
- `Dungeon::DrawDebug`：获取是否绘制调试信息。
- `Dungeon::DrawOnlyWhenEditingRoom`：获取是否仅在编辑房间时绘制调试信息。
- `Dungeon::ShowRoomOrigin`：获取是否显示房间原点。
- `Dungeon::FlipDoorArrow`：获取是否翻转门箭头方向。
- `Dungeon::DoorArrowLength`：获取门箭头长度。
- `Dungeon::DoorArrowHeadSize`：获取门箭头头部大小。
- `Dungeon::CanLoop`：获取是否允许循环。
- `Dungeon::RoomObjectType`：获取房间对象类型。
- `Dungeon::MaxGenerationTryBeforeGivingUp`：获取最大生成尝试次数。
- `Dungeon::MaxRoomPlacementTryBeforeGivingUp`：获取最大房间放置尝试次数。
- `Dungeon::RoomLimit`：获取房间限制。
- `Dungeon::EnableOcclusionCulling`：启用或禁用遮挡剔除。
- `Dungeon::SetOcclusionDistance`：设置遮挡距离。

### 随机数生成

- `Random::Guid2Seed`：使用 PCG-RXS-M-XS 算法将 GUID 和盐值转换为 32 位种子。

### 对象遍历

- `ObjectUtils::DispatchToObjectAndSubobjects`：递归地对对象及其所有子对象执行指定函数。

这个文件提供了插件中使用的各种通用工具函数，简化了代码编写并提高了代码复用性。