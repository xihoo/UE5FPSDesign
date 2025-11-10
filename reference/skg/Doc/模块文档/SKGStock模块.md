# SKGStock 模块文档

## 模块概述

SKGStock 模块提供了完整的枪托管理系统，支持枪托折叠、长度调节（Length of Pull）等功能。该模块专为真实射击体验设计，模拟了现代枪械枪托的各种操作，包括折叠枪托、伸缩枪托、固定枪托等不同类型。

## 核心组件

### USKGStockComponent

**类路径**: `Source/SKGStock/Public/Components/SKGStockComponent.h`

这是枪托系统的核心组件，负责管理枪托的所有状态和操作。

#### 主要功能
- **折叠/展开**: 支持可折叠枪托的状态切换
- **长度调节**: 支持枪托长度（Length of Pull）的动态调整
- **偏移应用**: 支持枪托位置的微调偏移
- **网络同步**: 完整的客户端-服务器状态同步
- **事件通知**: 状态变化时触发相应事件

#### 核心属性

##### 基础配置
```cpp
// 是否可以折叠
UPROPERTY(EditDefaultsOnly, BlueprintGetter = CanBeFolded, Category = "SKGStock|Initialize")
bool bCanBeFolded {false};

// 是否受偏移影响
UPROPERTY(EditDefaultsOnly, BlueprintGetter = CanBeFolded, Category = "SKGStock|Initialize")
bool bCanBeAffectedByOffset {true};

// 展开状态的长度（Length of Pull）
UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetUnfoldedLengthOfPull, Category = "SKGStock|Initialize")
float UnfoldedLengthOfPull {0.0f};

// 折叠状态的长度（Length of Pull）
UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetFoldedLengthOfPull, Category = "SKGStock|Initialize", meta = (EditCondition = "bCanBeFolded"))
float FoldedLengthOfPull {0.0f};

// 网络更新优化
UPROPERTY(EditDefaultsOnly, Category = "SKGStock|Initialize")
bool bAutoCallForceNetUpdate {true};
```

##### 状态变量
```cpp
// 当前折叠状态
UPROPERTY(ReplicatedUsing = OnRep_IsFolded)
bool bIsFolded {false};

// 当前长度
UPROPERTY(ReplicatedUsing = OnRep_LengthOfPull)
float LengthOfPull {0.0f};

// 位置偏移
float Offset {0.0f};
```

#### 主要方法

##### 折叠控制
```cpp
// 折叠枪托
UFUNCTION(BlueprintCallable, Category = "SKGStock")
void Fold();

// 展开枪托
UFUNCTION(BlueprintCallable, Category = "SKGStock")
void Unfold();

// 切换折叠状态
UFUNCTION(BlueprintCallable, Category = "SKGStock")
void ToggleFold();

// 检查是否折叠
UFUNCTION(BlueprintPure, Category = "SKGStock")
bool IsFolded() const { return bIsFolded; }

// 检查是否可以折叠
UFUNCTION(BlueprintGetter)
bool CanBeFolded() const { return bCanBeFolded; }
```

##### 长度管理
```cpp
// 获取当前长度
UFUNCTION(BlueprintPure, Category = "SKGStock")
float GetLengthOfPull() const { return LengthOfPull; }

// 获取展开状态长度
UFUNCTION(BlueprintGetter)
float GetUnfoldedLengthOfPull() const { return UnfoldedLengthOfPull; }

// 获取折叠状态长度
UFUNCTION(BlueprintGetter)
float GetFoldedLengthOfPull() const { return FoldedLengthOfPull; }
```

##### 偏移设置
```cpp
/**
 * 设置枪托位置偏移，用于长度调节系统
 * @param PositionOffset 位置偏移值
 */
UFUNCTION(BlueprintCallable, Category = "SKGStock")
void SetOffset(float PositionOffset);
```

#### 事件委托

```cpp
// 折叠状态变化事件
UPROPERTY(BlueprintAssignable, Category = "SKGStock|Events")
FOnFolded OnFolded;

// 长度变化事件
UPROPERTY(BlueprintAssignable, Category = "SKGStock|Events")
FOnLengthOfPullChanged OnLengthOfPullChanged;
```

## 使用方法

### 基础设置

1. **添加组件**: 在武器 Actor 上添加 `USKGStockComponent`
2. **配置属性**: 设置枪托的基本参数（是否可折叠、长度等）
3. **绑定事件**: 根据需要绑定状态变化事件

### 折叠枪托使用

```cpp
// 获取枪托组件
USKGStockComponent* StockComponent = Weapon->FindComponentByClass<USKGStockComponent>();

// 检查是否可以折叠
if (StockComponent->CanBeFolded())
{
    // 切换折叠状态
    StockComponent->ToggleFold();
    
    // 或者显式控制
    if (StockComponent->IsFolded())
    {
        StockComponent->Unfold();
    }
    else
    {
        StockComponent->Fold();
    }
}
```

### 长度调节使用

```cpp
// 获取当前长度
float CurrentLength = StockComponent->GetLengthOfPull();

// 设置偏移来调整长度（用于伸缩枪托）
StockComponent->SetOffset(5.0f);  // 向外延伸5个单位
```

### 事件监听

```cpp
// 绑定折叠状态变化事件
StockComponent->OnFolded.AddDynamic(this, &AMyWeapon::OnStockFolded);

// 处理折叠状态变化
void AMyWeapon::OnStockFolded(bool bFolded)
{
    if (bFolded)
    {
        // 枪托已折叠
        UpdateWeaponHandling(true);  // 更新武器操控性
        PlayFoldSound();             // 播放折叠音效
    }
    else
    {
        // 枪托已展开
        UpdateWeaponHandling(false); // 恢复武器操控性
        PlayUnfoldSound();           // 播放展开音效
    }
}

// 绑定长度变化事件
StockComponent->OnLengthOfPullChanged.AddDynamic(this, &AMyWeapon::OnLengthOfPullChanged);

// 处理长度变化
void AMyWeapon::OnLengthOfPullChanged(float NewLengthOfPull)
{
    // 根据长度调整武器属性
    AdjustWeaponStatsBasedOnLength(NewLengthOfPull);
}
```

## 高级功能

### 网络同步

所有关键状态都支持网络同步：

```cpp
// 服务器设置折叠状态
UFUNCTION(Server, Reliable, WithValidation)
void Server_SetFoldedState(bool bFold);

// 客户端复制
UPROPERTY(ReplicatedUsing = OnRep_IsFolded)
bool bIsFolded {false};

UPROPERTY(ReplicatedUsing = OnRep_LengthOfPull)
float LengthOfPull {0.0f};
```

### 性能优化

```cpp
// 网络更新优化
FORCEINLINE void TryForceNetUpdate() const
{
    if (bAutoCallForceNetUpdate)
    {
        GetOwner()->ForceNetUpdate();
    }
}
```

## 配置示例

### 折叠枪托配置（AK风格）

```cpp
// 基础配置
bCanBeFolded = true;
UnfoldedLengthOfPull = 10.0f;   // 展开状态长度
FoldedLengthOfPull = 3.0f;      // 折叠状态长度
bCanBeAffectedByOffset = true;
bAutoCallForceNetUpdate = true;
```

### 伸缩枪托配置（M4风格）

```cpp
// 基础配置
bCanBeFolded = false;           // 不能折叠
UnfoldedLengthOfPull = 8.0f;    // 最短长度
FoldedLengthOfPull = 8.0f;      // 同上（不可折叠）
bCanBeAffectedByOffset = true;  // 支持偏移调节
bAutoCallForceNetUpdate = true;
```

### 固定枪托配置（狙击步枪）

```cpp
// 基础配置
bCanBeFolded = false;           // 不能折叠
UnfoldedLengthOfPull = 12.0f;   // 固定长度
FoldedLengthOfPull = 12.0f;     // 同上
bCanBeAffectedByOffset = false; // 不支持偏移调节
bAutoCallForceNetUpdate = false; // 不需要网络更新
```

## 技术细节

### 状态管理

组件使用简单的状态机管理枪托状态：

```cpp
void SetLengthOfPull()
{
    if (bCanBeFolded && bIsFolded)
    {
        LengthOfPull = FoldedLengthOfPull;
    }
    else
    {
        LengthOfPull = UnfoldedLengthOfPull + Offset;
    }
    
    OnLengthOfPullChanged.Broadcast(LengthOfPull);
}
```

### 网络架构

采用服务器授权模式：

```cpp
void Fold()
{
    if (HasAuthority())
    {
        Server_SetFoldedState(true);
    }
    else
    {
        Server_SetFoldedState(true);
    }
}
```

## 集成使用

### 与火器系统集成

```cpp
// 在火器组件中使用枪托数据
void USKGFirearmComponent::CalculateProceduralValues()
{
    USKGStockComponent* StockComponent = GetCurrentStockComponent();
    if (StockComponent)
    {
        // 根据枪托长度调整武器操控性
        float LengthOfPull = StockComponent->GetLengthOfPull();
        float LengthFactor = FMath::Clamp(LengthOfPull / 10.0f, 0.8f, 1.2f);
        
        FSKGProceduralStats Stats = GetProceduralStats();
        Stats.MovementSwayMultiplier *= LengthFactor;
        SetProceduralStats(Stats);
    }
}
```

### 与附件系统集成

```cpp
// 作为附件使用
void USKGAttachmentComponent::TrySetupAttachment(UClass* AttachmentClass)
{
    if (AttachmentClass->IsChildOf(USKGStockComponent::StaticClass()))
    {
        // 设置枪托特定的逻辑
        SetupStockAttachment(AttachmentClass);
    }
}
```

## 常见问题

### Q: 枪托状态不同步？
A: 检查是否正确调用了服务器RPC，确保网络设置正确

### Q: 长度调节无效？
A: 确认 `bCanBeAffectedByOffset` 设置为true，并且正确调用了 `SetOffset()`

### Q: 折叠事件不触发？
A: 检查事件绑定是否正确，确认组件已正确初始化

### Q: 性能问题？
A: 合理设置 `bAutoCallForceNetUpdate`，避免不必要的网络更新

## 最佳实践

1. **合理设置长度参数**: 根据真实武器数据设置长度参数
2. **事件驱动更新**: 使用事件而非轮询来响应状态变化
3. **网络优化**: 合理设置网络更新频率，避免过度同步
4. **用户体验**: 提供清晰的视觉和听觉反馈
5. **兼容性考虑**: 确保与火器系统的正确集成

## 相关模块

- [SKGAttachment 模块](SKGAttachment模块.md) - 附件管理系统
- [SKGShooterFramework 模块](SKGShooterFramework模块.md) - 火器核心系统
- [SKGProceduralAnim 模块](SKGProceduralAnim模块.md) - 程序化动画系统

<system-reminder>Whenever you read the file, you should consider whether the user explicitly asked for a summary, analysis, or explanation. If they did, provide it. If not, continue with the task as requested.}</content>