# SKGAnimGraphNodes 模块文档

## 模块概述

SKGAnimGraphNodes 模块提供了自定义的动画图表节点，用于在 Unreal Engine 的动画蓝图中实现高级动画逻辑。该模块主要包含基于 GameplayTag 的动画混合节点，允许开发者通过标签系统控制动画状态。

## 核心组件

### FSKGAnimNode_GameplayTagBlend

**类路径**: `Source/SKGAnimGraphNodes/Public/AnimGraphNodes/SKGAnimNode_GameplayTagBlend.h`

这是一个动画节点结构体，继承自 `FAnimNode_BlendListBase`，用于根据 GameplayTag 动态选择和混合动画。

#### 主要功能
- **标签驱动的动画选择**: 根据当前激活的 GameplayTag 选择对应的动画
- **动态姿势管理**: 在编辑器中自动刷新和管理动画姿势
- **平滑过渡**: 支持动画之间的平滑切换

#### 核心属性

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
FGameplayTagContainer GameplayTagContainer;  // 可用的标签容器

UPROPERTY(EditAnywhere, Category = "Settings", Meta = (PinShownByDefault))
FGameplayTag ActiveGameplayTag;  // 当前激活的标签

int32 CurrentIndex {0};  // 当前动画索引
```

#### 关键方法

```cpp
// 获取当前激活的子动画索引
virtual int32 GetActiveChildIndex() override;

// 编辑器中刷新姿势
void RefreshPoses();
```

#### 使用逻辑

1. **初始化**: 节点创建时设置默认状态
2. **标签检查**: 每次更新时检查激活标签是否变化
3. **索引计算**: 根据激活标签在标签容器中的位置计算动画索引
4. **动画切换**: 返回对应的动画索引进行混合

#### 编辑器支持

在编辑器模式下，节点提供以下功能：
- **自动姿势同步**: 根据标签数量自动调整动画姿势数量
- **实时预览**: 支持在动画蓝图中实时预览效果

### 模块管理类

**类路径**: `Source/SKGAnimGraphNodes/Public/SKGAnimGraphNodes.h`

标准的 Unreal Engine 模块管理类，负责模块的启动和关闭。

## 使用方法

### 在动画蓝图中使用

1. **添加节点**: 在动画图表中右键添加 "SKG Gameplay Tag Blend" 节点
2. **配置标签**: 设置 `GameplayTagContainer` 包含所有可能的标签
3. **连接动画**: 为每个标签连接对应的动画姿势
4. **设置激活标签**: 通过蓝图或代码设置 `ActiveGameplayTag`

### 代码集成

```cpp
// 在动画蓝图中获取节点
FSKGAnimNode_GameplayTagBlend* Node = GetAnimNode<FSKGAnimNode_GameplayTagBlend>();

// 设置激活标签
Node->ActiveGameplayTag = FGameplayTag::RequestGameplayTag("Combat.Standing");
```

### 标签配置示例

```cpp
// 定义标签容器
FGameplayTagContainer TagContainer;
TagContainer.AddTag(FGameplayTag::RequestGameplayTag("Combat.Standing"));
TagContainer.AddTag(FGameplayTag::RequestGameplayTag("Combat.Crouching"));
TagContainer.AddTag(FGameplayTag::Combat.Prone"));
```

## 技术细节

### 继承结构
```
FAnimNode_BlendListBase
└── FSKGAnimNode_GameplayTagBlend
```

### 性能考虑
- **轻量级更新**: 只在标签变化时重新计算索引
- **缓存优化**: 缓存前一个标签避免重复计算
- **编辑器优化**: 只在需要时刷新姿势

### 扩展性
该节点设计为可扩展的基础，可以通过继承创建更多特定的动画混合逻辑。

## 注意事项

1. **标签顺序**: 动画索引基于标签在容器中的顺序
2. **默认值**: 当没有匹配标签时返回索引 0（默认动画）
3. **编辑器同步**: 确保标签数量与动画姿势数量匹配
4. **网络同步**: 该节点本身不涉及网络，但激活标签可以通过游戏逻辑同步

## 相关文档

- [Unreal Engine 动画系统](https://docs.unrealengine.com/en-US/animating-characters-and-objects/index.html)
- [Gameplay Tags 系统](https://docs.unrealengine.com/en-US/gameplay-tags-in-unreal-engine/index.html)
- [动画图表节点开发](https://docs.unrealengine.com/en-US/animation-node-development-in-unreal-engine/index.html)