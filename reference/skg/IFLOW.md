# iFlow 上下文信息 (IFLOW.md)

## 项目概述

此目录包含 **SKGShooterFramework** 的源代码，这是一个用于 Unreal Engine 5.6 的多人射击游戏框架插件（版本 1.4.0）。它由 Sneaky Kitty Game Dev 开发，旨在为开发者提供一套模块化、可扩展的组件和系统，用于构建射击游戏。

该框架被组织成 19 个模块，每个模块负责游戏中的特定功能，例如附件管理、光学设备、枪口装置等。这些模块通过组件（Components）和数据资产（Data Assets）进行交互。框架支持 Windows、Linux 和 Mac 平台。

## 目录结构

- `README.md`: 项目简要说明。
- `SKGShooterFramework.uplugin`: Unreal Engine 插件描述文件，定义了插件的元数据、模块和依赖项。
- `Source/`: 包含所有 C++ 源代码，按模块组织。
- `wiki/`: 包含英文 Markdown 格式的文档，解释了如何使用框架中的不同组件和模块。
- `Doc/`: 包含中文文档，包括框架概述、模块文档和学习路径。
- `Resources/`: 包含插件图标等资源文件。

## 核心模块与组件

根据 `SKGShooterFramework.uplugin` 和项目结构分析，核心模块和组件包括：

### 模块 (Modules)

#### 核心框架模块
- **SKGShooterFramework**: 主框架模块。
- **SKGShooterFrameworkCore**: 核心功能模块。
- **SKGShooterFrameworkEditor**: 编辑器相关功能（仅未编译版本）。
- **SKGShooterFrameworkActors**: 框架相关的 Actor 类。

#### 功能模块
- **SKGAttachment**: 武器附件系统。
- **SKGOptic**: 光学设备（瞄准镜）。
- **SKGMuzzle**: 枪口装置（消音器、制退器）。
- **SKGProjectile**: 投射物系统。
- **SKGProceduralAnim**: 程序化动画。
- **SKGPhysicalMaterialModule**: 物理材质相关。
- **SKGAnimGraphNodes**: 动画蓝图节点。
- **SKGStats**: 统计系统。
- **SKGStock**: 枪托系统。
- **SKGRangeFinder**: 测距仪。
- **SKGLightLaser**: 激光指示器。
- **SKGOptional**: 可选功能模块。

#### 专用功能模块
- **SKGNightVision**: 夜视功能。
- **SKGThermal**: 热成像功能。

### 组件 (Components)

- **SKGAttachmentManagerComponent**: 管理角色或武器上的附件。
- **SKGAttachmentComponent**: 代表一个具体的附件。
- **SKGOpticComponent**: 光学设备组件。
- **SKGMuzzleComponent**: 枪口装置组件。
- **SKGFirearmComponent**: 火器核心组件。
- **SKGShooterPawnComponent**: 射击角色组件。
- **SKGProceduralAnimComponent**: 程序化动画组件。
- **SKGLightLaserComponent**: 激光指示器组件。
- **SKGRangeFinderComponent**: 测距仪组件。
- **SKGStockComponent**: 枪托组件。
- **SKGCharacterMovementComponent**: 角色移动组件（可能为框架定制）。
- **SKGOffHandIKComponent**: 非主手 IK 组件。
- **SKGOpticSceneCaptureComponent**: 光学设备场景捕捉组件。
- **SKGFirearmAttachmentStatComponent**: 火器附件属性组件。

## 插件依赖

根据插件配置，SKGShooterFramework 依赖以下 Unreal Engine 插件：
- **Niagara**: 用于粒子效果系统
- **StructUtils**: 用于结构体工具
- **GameplayCameras**: 用于游戏相机系统

## 构建与运行

这是一个 Unreal Engine 5.6 插件项目。

1.  **环境**: 需要安装 Unreal Engine 5.6。
2.  **集成**: 将此插件文件夹放入你的 UE 项目的 `Plugins` 目录中。
3.  **启用**: 在 UE 编辑器中，转到 `Edit -> Plugins`，找到 `SKGShooterFramework` 并启用它。
4.  **编译**: 重新生成项目文件（例如，右键点击 `.uproject` 文件 -> "Generate Visual Studio project files"）并使用你的 IDE (如 Visual Studio 或 Rider) 编译项目。

## 文档资源

项目包含丰富的文档资源：

### 英文文档 (wiki/)
- 组件使用指南
- 设置教程
- 动画蓝图集成
- 自定义姿势创建

### 中文文档 (Doc/)
- 框架概述和技术架构综述
- 详细的模块文档（19个模块）
- 学习路径指南
- 具体技术分析（如SKGShooterPawnComponent）

## 开发约定

- **语言**: C++ (用于核心逻辑和模块) 和 Blueprint (用于游戏特定逻辑和配置)。
- **模块化**: 代码被组织成多个独立的模块，每个模块有明确的职责。
- **组件化**: 游戏对象的功能通过 `USceneComponent` 的子类（即 Components）来实现。
- **数据驱动**: 大量使用 Unreal Engine 的数据资产（Data Assets）来配置组件和模块的行为。
- **平台支持**: 支持 Windows 64位、Linux 和 Mac 平台。
- **网络优先**: 框架设计考虑了多人游戏的网络同步需求。

## 项目资源

- **官方网站**: https://www.youtube.com/channel/UCYcDJPGvIUFxBTmXOZWFswQ
- **文档**: https://github.com/SneakyKittyGameDev/SKGSFExample/wiki
- **市场**: com.epicgames.launcher://ue/marketplace/product/4986d4f5b73148deb154e1cd31dd9623
- **社区支持**: https://discord.gg/W5g6pZXfjh