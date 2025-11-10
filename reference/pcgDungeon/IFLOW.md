# Procedural Dungeon Plugin - iFlow Context

## 项目概述

这是一个用于Unreal Engine的插件，允许开发者创建丰富的地牢体验，结合了手工制作房间设计的创造力和精确性，以及程序化生成的不可预测性和刺激性。房间是手工制作的Unreal关卡，可以在世界中实例化。用户可以定义房间的大小、门以及其他参数，并通过蓝图或C++编写自己的规则，以生成符合需求的地牢。

## 构建和运行

### 项目结构
- `Source/ProceduralDungeon`: 运行时模块，包含核心功能。
- `Source/ProceduralDungeonEditor`: 编辑器模块，提供编辑器工具和功能。

### 构建说明
这是一个Unreal Engine插件项目，构建和运行依赖于Unreal Engine的构建系统。
1. 将插件放置在Unreal Engine项目的`Plugins`目录下。
2. 在Unreal Engine编辑器中启用插件。
3. 重新生成项目文件并编译。

### 运行说明
1. 在Unreal Engine编辑器中打开项目。
2. 确保插件已启用。
3. 按照[入门指南](https://benpyton.github.io/ProceduralDungeon/guides/Introduction)开始使用插件。

## 开发约定

### 代码风格
- C++代码遵循Unreal Engine的编码标准。
- 使用`#pragma once`作为头文件保护。
- 使用Unreal Engine的类型系统（如`FVector`, `FIntVector`等）。

### 模块结构
- `ProceduralDungeon`: 运行时模块，包含地牢生成的核心逻辑。
- `ProceduralDungeonEditor`: 编辑器模块，提供编辑器模式和工具。

### 核心类
- `ADungeonGenerator`: 地牢生成器的主要Actor，负责生成地牢并进行网络复制。
- `URoom`: 地牢中房间的实例，持有每个房间实例的特定数据。
- `URoomData`: 房间数据资产，定义房间的大小、门和其他参数。

### 接口
- `IReadOnlyRoom`: 提供对房间数据的只读访问。
- `IDungeonCustomSerialization`: 用于自定义序列化逻辑。
- `IDungeonSaveInterface`: 用于保存和加载地牢数据。

## 许可证

此插件根据获取来源的不同，提供不同的许可证：
- 从Fab市场获取时，适用[Fab EULA](https://fab.com/eula)。
- 从其他来源获取时，适用[CeCILL-C许可证](https://cecill.info/licences/Licence_CeCILL-C_V1-en.html)。