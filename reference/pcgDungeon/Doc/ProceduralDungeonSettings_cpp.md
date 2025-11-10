# ProceduralDungeonSettings.cpp

该文件包含了 `UProceduralDungeonSettings` 类的实现，该类用于管理 `ProceduralDungeon` 插件的全局设置。

## 主要功能

- **设置初始化**：在构造函数中初始化所有插件设置的默认值。
- **控制台变量注册**：将部分设置注册为控制台变量，以便在运行时通过控制台命令进行修改。

## 详细实现

### 构造函数

- `UProceduralDungeonSettings(const FObjectInitializer& ObjectInitializer)`：
  - 调用父类构造函数。
  - 初始化各种设置的默认值，包括：
    - **地牢设置**：房间单元大小、门的大小、门的偏移、是否允许循环、最大生成尝试次数、最大房间放置尝试次数、房间限制。
    - **遮挡设置**：是否启用遮挡剔除、遮挡距离、是否遮挡动态Actor。
    - **调试设置**：是否绘制调试信息、是否仅在编辑房间时绘制、是否显示房间原点、门箭头的绘制方向、门箭头的长度和头部大小、是否在屏幕上打印调试日志、调试日志的显示时长。

### 控制台变量注册

- 使用 `IConsoleManager::Get().RegisterConsoleVariableRef` 将以下设置注册为控制台变量：
  - `pd.Occlusion`：控制是否启用遮挡剔除。
  - `pd.Occlusion.Distance`：控制遮挡剔除的距离。
  - `pd.Occlusion.DynamicActors`：控制是否遮挡动态Actor。
  - `pd.Debug.Draw`：控制是否绘制调试信息。
  - `pd.Debug.Draw.EditingOnly`：控制是否仅在编辑房间时绘制调试信息。
  - `pd.Debug.Log.OnScreen`：控制是否在屏幕上打印调试日志。
  - `pd.Debug.Log.Duration`：控制调试日志的显示时长。

这个类与 `ProceduralDungeonSettings.h` 头文件配合使用，提供了插件的全局配置选项，用户可以在编辑器的项目设置中修改这些设置。