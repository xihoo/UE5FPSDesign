# SKGAttachment 模块文档

## 模块概述

SKGAttachment 模块是 SKGShooterFramework 的核心模块之一，提供了完整的武器附件管理系统。该模块支持各种武器附件的动态附加、移除、位置调整和兼容性检查，包括瞄准镜、枪口装置、激光指示器等。

## 核心组件

### USKGAttachmentComponent

**类路径**: `Source/SKGAttachment/Public/Components/SKGAttachmentComponent.h`

这是附件系统的核心组件，负责管理单个附件槽位的所有功能。

#### 主要功能
- **附件管理**: 动态附加和移除武器附件
- **位置调整**: 支持附件在导轨上的位置微调
- **兼容性检查**: 确保只有兼容的附件可以被附加
- **预览系统**: 支持附件预览功能
- **网络同步**: 完整的客户端-服务器同步支持

#### 核心属性

##### 基础配置
```cpp
// 附加目标配置
UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment|Initialize")
FName AttachToMeshName {"StaticMesh"};  // 目标网格组件名称

UPROPERTY(EditDefaultsOnly, Replicated, BlueprintGetter = GetAttachToSocket, Category = "SKGAttachment|Initialize")
FName AttachToSocket {NAME_None};  // 附加到的插槽名称

UPROPERTY(EditDefaultsOnly, Replicated, BlueprintGetter = GetComponentName, Category = "SKGAttachment|Initialize")
FName ComponentName {NAME_None};  // 组件名称（用于UI显示）
```

##### 附件兼容性
```cpp
// 传统兼容性系统
UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment|Attachment|Compatibility|Legacy")
TArray<TObjectPtr<USKGPDAAttachmentCompatibility>> CompatibleAttachments;

// GameplayTag 兼容性系统
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "SKGAttachment|Attachment|Compatibility|GameplayTag")
FGameplayTag SlotTag;  // 槽位标签
```

##### 位置调整系统
```cpp
UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetOffsetSettings, Category = "SKGAttachment|Attachment|Settings")
FSKGAttachmentOffsetSettings OffsetSettings;
```

#### 关键结构体

##### FSKGAttachmentOffsetSettings
```cpp
USTRUCT(BlueprintType)
struct FSKGAttachmentOffsetSettings
{
    GENERATED_BODY()
    UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment")
    bool bUseOffsetSystem {true};  // 是否启用位置调整
    
    UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment")
    bool bReplicateOffsetEachChange {false};  // 是否每次变化都复制
    
    UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment")
    TEnumAsByte<EAxis::Type> OffsetAxis {EAxis::Y};  // 调整轴
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment")
    float OffsetSnapDistance {1.046f};  // 吸附距离（1.046 =皮卡汀尼导轨）
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment")
    float MinimumOffsetAllowed {0.0f};  // 最小允许偏移
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment")
    float MaximumOffsetAllowed {0.0f};  // 最大允许偏移
};
```

#### 主要方法

##### 附件管理
```cpp
// 加载并设置附件
UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Attachment")
void TryLoadSetupAttachment(const TSoftClassPtr<AActor>& AttachmentToLoad);

// 设置已存在的Actor为附件
UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Attachment")
void TrySetupExistingAttachment(AActor* AttachmentToSetup, const bool bAttach = true);

// 移除当前附件
UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Attachment")
AActor* RemoveAttachment();

// 销毁当前附件
UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Attachment")
void DestroyAttachment();
```

##### 位置调整
```cpp
// 设置附件偏移
UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Offset")
void SetAttachmentOffset(float Offset);

// 递增偏移
UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Offset")
void IncrementOffset();

// 递减偏移
UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Offset")
void DecrementOffset();

// 获取吸附点
UFUNCTION(BlueprintPure, Category = "SKGAttachment|Attachment|Offset")
TArray<FVector> GetAttachSnapPoints() const;
```

##### 兼容性检查
```cpp
// 检查附件类是否兼容
UFUNCTION(BlueprintPure, Category = "SKGAttachment|Compatibility")
virtual bool IsAttachmentClassCompatible(const UClass* AttachmentClass);

// 检查附件对象是否兼容
UFUNCTION(BlueprintPure, Category = "SKGAttachment|Compatibility")
virtual bool IsAttachmentCompatible(const UObject* Object);
```

#### 事件委托

```cpp
// 附件添加事件
UPROPERTY(BlueprintAssignable, Category = "SKGAttachment|Events")
FOnAttachmentAdded OnAttachmentAdded;

// 附件移除事件
UPROPERTY(BlueprintAssignable, Category = "SKGAttachment|Events")
FOnAttachmentRemoved OnAttachmentRemoved;

// 附件销毁事件
UPROPERTY(BlueprintAssignable, Category = "SKGAttachment|Events")
FOnAttachmentDestroyed OnAttachmentDestroyed;

// 附件变化事件
UPROPERTY(BlueprintAssignable, Category = "SKGAttachment|Events")
FOnAttachmentChanged OnAttachmentChanged;

// 偏移变化事件
UPROPERTY(BlueprintAssignable, Category = "SKGAttachment|Events")
FOnOffsetChanged OnOffsetChanged;
```

### USKGAttachmentManagerComponent

**类路径**: `Source/SKGAttachment/Public/Components/SKGAttachmentManagerComponent.h`

附件管理器组件，负责管理多个附件组件，提供统一的附件管理接口。

#### 主要功能
- **多附件管理**: 统一管理武器上的所有附件
- **兼容性协调**: 确保附件之间的兼容性
- **状态同步**: 维护附件状态的一致性

## 使用方法

### 基础设置

1. **添加组件**: 在武器 Actor 上添加 `USKGAttachmentComponent`
2. **配置插槽**: 设置 `AttachToSocket` 和 `ComponentName`
3. **定义兼容性**: 配置兼容的附件列表或使用 GameplayTag 系统
4. **设置偏移**: 配置位置调整参数（如需要）

### 附件附加流程

```cpp
// 获取附件组件
USKGAttachmentComponent* AttachmentComponent = Weapon->FindComponentByClass<USKGAttachmentComponent>();

// 检查兼容性
if (AttachmentComponent->IsAttachmentClassCompatible(AttachmentClass))
{
    // 附加附件
    AttachmentComponent->TrySetupAttachment(AttachmentClass);
}
```

### 位置调整使用

```cpp
// 检查是否支持位置调整
if (AttachmentComponent->CanAttachmentBeOffset())
{
    // 递增位置
    AttachmentComponent->IncrementOffset();
    
    // 或者直接设置位置
    AttachmentComponent->SetAttachmentOffset(5.0f);
    
    // 获取吸附点
    TArray<FVector> SnapPoints = AttachmentComponent->GetAttachSnapPoints();
}
```

### 事件监听

```cpp
// 绑定附件变化事件
AttachmentComponent->OnAttachmentChanged.AddDynamic(this, &AMyWeapon::OnAttachmentChanged);

// 处理附件变化
void AMyWeapon::OnAttachmentChanged()
{
    // 更新武器属性
    UpdateWeaponStats();
}
```

## 高级功能

### 预览系统

支持在不实际附加的情况下预览附件效果：

```cpp
// 设置预览附件
AttachmentComponent->SetupPreviewAttachment(PreviewAttachmentClass);

// 销毁预览
AttachmentComponent->DestroyPreviewAttachment();
```

### 网络同步

所有关键操作都支持网络同步：
- 附件附加/移除
- 位置调整
- 预览状态

### 兼容性系统

支持两种兼容性检查方式：
1. **传统系统**: 基于兼容性数据资产
2. **GameplayTag 系统**: 基于标签的兼容性检查

## 技术细节

### 网络架构

```cpp
// 服务器授权
UFUNCTION(Server, Reliable, WithValidation)
void Server_TrySetupAttachment(UClass* AttachmentClass);

// 客户端复制
UPROPERTY(ReplicatedUsing = OnRep_Attachment)
TObjectPtr<AActor> Attachment;
```

### 性能优化

- **缓存机制**: 缓存兼容性数据避免重复计算
- **延迟初始化**: 组件在需要时才进行完整初始化
- **事件驱动**: 使用委托系统减少轮询开销

## 配置示例

### 皮卡汀尼导轨配置

```cpp
// 导轨参数
OffsetSettings.OffsetSnapDistance = 1.046f;  // 标准皮卡汀尼导轨间距
OffsetSettings.MinimumOffsetAllowed = -5.0f;
OffsetSettings.MaximumOffsetAllowed = 5.0f;
OffsetSettings.OffsetAxis = EAxis::Y;
```

### 兼容性配置

```cpp
// 使用 GameplayTag 系统
SlotTag = FGameplayTag::RequestGameplayTag("Attachment.Slot.TopRail");

// 或使用传统系统
CompatibleAttachments.Add(ScopeCompatibilityData);
CompatibleAttachments.Add(LaserCompatibilityData);
```

## 最佳实践

1. **合理命名**: 使用清晰的组件名称便于UI显示
2. **性能考虑**: 避免频繁的位置调整操作
3. **网络优化**: 合理设置复制频率避免网络拥堵
4. **错误处理**: 总是检查操作返回值确保成功
5. **内存管理**: 及时清理不再使用的预览附件

## 常见问题

### Q: 附件无法附加？
A: 检查兼容性配置和插槽名称是否正确

### Q: 位置调整不生效？
A: 确保启用了位置调整系统并正确设置了偏移范围

### Q: 网络同步问题？
A: 检查是否正确处理了服务器授权和客户端复制

## 相关模块

- [SKGOptic 模块](SKGOptic模块.md) - 光学瞄准镜系统
- [SKGLightLaser 模块](SKGLightLaser模块.md) - 激光指示器系统
- [SKGMuzzle 模块](SKGMuzzle模块.md) - 枪口装置系统
- [SKGShooterFrameworkCore 模块](SKGShooterFrameworkCore模块.md) - 核心框架功能