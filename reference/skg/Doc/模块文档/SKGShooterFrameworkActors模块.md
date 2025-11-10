# SKGShooterFrameworkActors 模块文档

## 模块概述

SKGShooterFrameworkActors 模块提供了框架预定义的基础Actor类，包括武器、附件、瞄准设备等游戏对象的基类实现。该模块作为框架的对象层，为开发者提供了可直接使用或继承的基础Actor类，简化了射击游戏对象的创建过程。

## 核心架构

### 模块职责
- **基础Actor类**: 提供框架标准的Actor基类
- **武器系统**: 完整的武器Actor实现
- **附件系统**: 各类附件的基础Actor
- **数据驱动**: 支持通过数据资产进行配置
- **组件集成**: 预集成框架的核心组件

## 核心Actor类

### ASKGBaseActor

**类路径**: `Source/SKGShooterFrameworkActors/Public/Actors/SKGBaseActor.h`

这是所有框架Actor的基础类，提供了游戏标签接口和构造数据支持。

#### 核心功能
- **游戏标签支持**: 实现IGameplayTagAssetInterface
- **构造数据**: 支持通过数据资产进行构造
- **事件系统**: 提供构造完成事件
- **初始化管理**: 管理Actor的初始化状态

#### 核心属性

```cpp
// 游戏标签容器
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGGameplayTags")
FGameplayTagContainer GameplayTags;

// 初始化状态
bool bIsInitialized {false};

// 数据资产构造包加载状态
bool bDAConstructionBundlesLoaded {false};
```

#### 主要方法

```cpp
// 获取拥有的游戏标签
virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }

// 数据资产构造包加载事件
UFUNCTION(BlueprintImplementableEvent, Category = "SKGActor|Bundles")
void DAConstructionBundlesLoaded();

// 检查构造包是否已加载
UFUNCTION(BlueprintPure, Category = "SKGActor|Bundles")
bool AreBundlesLoaded() const { return bDAConstructionBundlesLoaded; }
```

#### 事件委托

```cpp
// 数据资产构造包加载完成事件
UPROPERTY(BlueprintAssignable, Category = "SKGActor|Bundles")
FOnSKGActorDAConstructionBundlesLoaded OnDAConstructionBundlesLoaded;
```

### ASKGFirearm

**类路径**: `Source/SKGShooterFrameworkActors/Public/Actors/SKGFirearm.h`

这是标准的火器Actor类，预集成了框架的所有核心组件。

#### 核心组件

```cpp
// 火器网格组件（骨骼网格）
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Firearm")
TObjectPtr<USkeletalMeshComponent> FirearmMeshComponent;

// 火器核心组件
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Firearm")
TObjectPtr<USKGFirearmComponent> FirearmComponent;

// 附件管理器组件
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Firearm")
TObjectPtr<USKGAttachmentManagerComponent> AttachmentManagerComponent;

// 程序化动画组件
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Firearm")
TObjectPtr<USKGProceduralAnimComponent> ProceduralAnimComponent;

// 枪口组件
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Firearm")
TObjectPtr<USKGMuzzleComponent> MuzzleComponent;

// 副手IK组件
UPROPERTY(BlueprintReadOnly, Category = "Firearm")
TObjectPtr<USKGOffhandIKComponent> OffhandIKComponent;
```

#### 数据驱动配置

```cpp
// 要加载的构造包
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm")
TArray<FName> BundlesToLoad {"Visual"};

// 构造数据资产
UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_DAConstruction, Category = "Firearm", meta = (ExposeOnSpawn = true))
TObjectPtr<USKGPDAFirearm> DAConstruction;
```

#### 核心功能

```cpp
// 设置构造数据资产
virtual void SetConstructionDataAsset(UDataAsset* DataAsset) override;

// 获取构造数据资产
virtual UPrimaryDataAsset* GetConstructionDataAsset() override;

// 网络同步处理
virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
```

### 其他专用Actor类

框架还提供了多种专用Actor类：

- **ASKGOptic**: 光学瞄准镜Actor
- **ASKGMuzzle**: 枪口装置Actor  
- **ASKGLightLaser**: 激光指示器Actor
- **ASKGGrip**: 握把Actor

## 数据类型系统

### FSKGAttachmentComponentData

**类路径**: `Source/SKGShooterFrameworkActors/Public/DataTypes/SKGShooterFrameworkActorDataTypes.h`

定义了附件组件的数据结构，支持复杂的附件配置。

#### 核心结构

```cpp
USTRUCT(BlueprintType)
struct FSKGAttachmentComponentData
{
    GENERATED_BODY()
    
    // 附件组件类
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGData")
    TSubclassOf<USKGAttachmentComponent> AttachmentComponentClass;
    
    // 附件组件名称
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGData")
    FName AttachmentComponentName {NAME_None};
    
    // Actor类
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGData")
    TSubclassOf<AActor> ActorClass;
    
    // Actor类定义数据资产
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGData")
    TObjectPtr<UDataAsset> ActorClassDefinition;
    
    // 要附加到的插槽
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGData")
    FName SocketToAttachTo {NAME_None};
    
    // 槽位标签
    UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment|Attachment|Compatibility|GameplayTag")
    FGameplayTag SlotTag;
    
    // 槽位兼容性覆盖
    UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment|Attachment|Compatibility|GameplayTag")
    FSKGAttachmentSlotCompatbility SlotCompatbilityOverride;
    
    // 子组件数组
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGData")
    TArray<FInstancedStruct> Children;
};
```

## 使用方法

### 基础武器创建

1. **创建武器类**: 继承自ASKGFirearm
2. **配置数据资产**: 设置武器的基础数据
3. **自定义行为**: 重写必要的方法
4. **添加到游戏**: 在游戏中实例化武器

### 武器类实现示例

```cpp
// 创建自定义武器类
UCLASS()
class MYGAME_API AMyRifle : public ASKGFirearm
{
    GENERATED_BODY()
    
public:
    AMyRifle();
    
protected:
    virtual void BeginPlay() override;
    virtual void FireWeapon() override;
    
private:
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float FireRate;
    
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float Damage;
};

// 构造函数
AMyRifle::AMyRifle()
{
    // 设置默认数据资产
    static ConstructorHelpers::FObjectFinder<USKGPDAFirearm> FirearmData(TEXT("/Game/DataAssets/MyRifleData"));
    if (FirearmData.Succeeded())
    {
        DAConstruction = FirearmData.Object;
    }
}

// 开始游戏时初始化
void AMyRifle::BeginPlay()
{
    Super::BeginPlay();
    
    // 等待数据资产加载完成
    if (AreBundlesLoaded())
    {
        // 执行自定义初始化
        InitializeCustomComponents();
    }
    else
    {
        // 监听数据加载完成事件
        OnDAConstructionBundlesLoaded.AddDynamic(this, &AMyRifle::OnDataLoaded);
    }
}

// 数据加载完成处理
void AMyRifle::OnDataLoaded()
{
    InitializeCustomComponents();
}
```

### 数据资产构造

```cpp
// 使用数据资产进行构造
void SetupWeaponFromData()
{
    if (DAConstruction)
    {
        // 从数据资产获取基础属性
        FirearmComponent->SetFirearmStats(DAConstruction->BaseStats);
        
        // 设置程序化动画参数
        ProceduralAnimComponent->SetProceduralOffsets(DAConstruction->ProceduralOffsets);
        
        // 配置附件兼容性
        AttachmentManagerComponent->SetCompatibilityData(DAConstruction->CompatibilityData);
    }
}
```

### 网络同步处理

```cpp
// 处理网络同步
void AMyRifle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    // 复制关键数据
    DOREPLIFETIME(AMyRifle, CurrentAmmo);
    DOREPLIFETIME(AMyRifle, bIsFiring);
    DOREPLIFETIME(AMyRifle, FireMode);
}

// 服务器授权处理
void AMyRifle::FireWeapon()
{
    if (HasAuthority())
    {
        // 服务器执行射击逻辑
        PerformFireLogic();
        
        // 同步到客户端
        OnRep_FiringState();
    }
    else
    {
        // 客户端请求服务器执行
        Server_FireWeapon();
    }
}
```

## 高级功能

### 自定义附件系统

```cpp
// 扩展附件数据
USTRUCT(BlueprintType)
struct FMyCustomAttachmentData : public FSKGAttachmentComponentData
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
    float CustomValue;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
    FLinearColor CustomColor;
};
```

### 动态组件添加

```cpp
// 运行时添加组件
void AddDynamicComponent()
{
    // 创建新组件
    USKGCustomComponent* NewComponent = NewObject<USKGCustomComponent>(this);
    NewComponent->RegisterComponent();
    
    // 注册到附件管理器
    if (AttachmentManagerComponent)
    {
        AttachmentManagerComponent->RegisterAttachmentComponent(NewComponent);
    }
}
```

### 自定义事件处理

```cpp
// 绑定框架事件
void SetupEventBindings()
{
    // 绑定附件变化事件
    if (AttachmentManagerComponent)
    {
        AttachmentManagerComponent->OnAttachmentAdded.AddDynamic(this, &AMyRifle::OnAttachmentAdded);
        AttachmentManagerComponent->OnAttachmentRemoved.AddDynamic(this, &AMyRifle::OnAttachmentRemoved);
    }
    
    // 绑定统计数据变化事件
    if (FirearmComponent)
    {
        FirearmComponent->OnFirearmStatsChanged.AddDynamic(this, &AMyRifle::OnStatsChanged);
    }
}
```

## 数据资产集成

### 构造数据资产

```cpp
// 创建武器构造数据资产
UCLASS()
class MYGAME_API UMyRifleData : public USKGPDAFirearm
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere, Category = "WeaponData")
    float BaseDamage;
    
    UPROPERTY(EditAnywhere, Category = "WeaponData")
    float EffectiveRange;
    
    UPROPERTY(EditAnywhere, Category = "WeaponData")
    TArray<FName> CompatibleAttachments;
    
    UPROPERTY(EditAnywhere, Category = "WeaponData")
    USoundBase* FireSound;
    
    UPROPERTY(EditAnywhere, Category = "WeaponData")
    UNiagaraSystem* MuzzleFlashEffect;
};
```

### 数据驱动构造

```cpp
// 实现构造接口
virtual void SetConstructionDataAsset(UDataAsset* DataAsset) override
{
    DAConstruction = Cast<USKGPDAFirearm>(DataAsset);
    
    if (DAConstruction)
    {
        // 应用数据资产配置
        ApplyConstructionData();
    }
}

virtual UPrimaryDataAsset* GetConstructionDataAsset() override
{
    return DAConstruction;
}

// 应用构造数据
void ApplyConstructionData()
{
    if (DAConstruction)
    {
        // 设置基础属性
        if (FirearmComponent)
        {
            FirearmComponent->SetFirearmStats(DAConstruction->FirearmStats);
        }
        
        // 设置程序化参数
        if (ProceduralAnimComponent)
        {
            ProceduralAnimComponent->SetProceduralOffsets(DAConstruction->ProceduralOffsets);
        }
        
        // 设置附件兼容性
        if (AttachmentManagerComponent)
        {
            AttachmentManagerComponent->SetCompatibilityData(DAConstruction->CompatibilityData);
        }
    }
}
```

## 技术细节

### 组件初始化顺序

```cpp
// 确保组件按正确顺序初始化
virtual void BeginPlay() override
{
    Super::BeginPlay();
    
    // 1. 等待数据资产加载
    if (AreBundlesLoaded())
    {
        InitializeComponents();
    }
    else
    {
        OnDAConstructionBundlesLoaded.AddDynamic(this, &AMyActor::OnBundlesLoaded);
    }
}

void OnBundlesLoaded()
{
    // 2. 应用构造数据
    ApplyConstructionData();
    
    // 3. 初始化组件
    InitializeComponents();
    
    // 4. 设置初始状态
    SetInitialState();
}
```

### 网络同步架构

```cpp
// 网络同步处理
virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    // 复制构造数据资产
    DOREPLIFETIME_CONDITION(AMyFirearm, DAConstruction, COND_None);
    
    // 复制关键状态
    DOREPLIFETIME_CONDITION(AMyFirearm, CurrentState, COND_SkipOwner);
}

// 构造数据同步
UFUNCTION()
void OnRep_DAConstruction()
{
    if (DAConstruction)
    {
        ApplyConstructionData();
    }
}
```

### 性能优化

```cpp
// 使用对象池优化组件创建
void SetupComponentPooling()
{
    // 预创建常用组件
    ComponentPool.Reserve(10);
    for (int32 i = 0; i < 10; ++i)
    {
        USKGComponent* PooledComponent = NewObject<USKGComponent>(this);
        PooledComponent->SetFlags(RF_Transient);
        ComponentPool.Add(PooledComponent);
    }
}

// 批量处理附件
void ProcessAttachmentsInBatch(const TArray<AActor*>& Attachments)
{
    // 批量处理减少函数调用开销
    for (AActor* Attachment : Attachments)
    {
        if (Attachment && Attachment->IsValidLowLevel())
        {
            ProcessAttachment(Attachment);
        }
    }
    
    // 一次性触发更新事件
    OnAttachmentsProcessed.Broadcast();
}
```

## 最佳实践

1. **继承层次**: 合理设计继承层次，避免过度复杂的类结构
2. **数据驱动**: 尽可能使用数据资产进行配置，减少硬编码
3. **组件解耦**: 保持组件间的松耦合，便于独立测试和维护
4. **网络优化**: 合理设计网络同步策略，减少带宽使用
5. **性能考虑**: 使用对象池和批量处理优化性能

## 常见问题

### Q: 数据资产不加载？
A: 检查数据资产路径是否正确，确认构造数据资产已正确设置

### Q: 组件初始化失败？
A: 确保组件按正确顺序初始化，检查组件间的依赖关系

### Q: 网络同步问题？
A: 验证网络同步条件设置，确保关键数据正确复制

### Q: 性能问题？
A: 使用对象池和批量处理，避免频繁的组件创建销毁

### Q: 附件系统不工作？
A: 检查附件兼容性配置，验证附件管理器组件状态

## 相关模块

- [SKGShooterFramework 模块](SKGShooterFramework模块.md) - 火器核心系统
- [SKGAttachment 模块](SKGAttachment模块.md) - 附件管理系统
- [SKGProceduralAnim 模块](SKGProceduralAnim模块.md) - 程序化动画系统

<system-reminder>Whenever you read the file, you should consider whether the user explicitly asked for a summary, analysis, or explanation. If you did, provide it. If not, continue with the task as requested.}</content>