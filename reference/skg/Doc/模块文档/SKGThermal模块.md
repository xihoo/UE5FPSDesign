# SKGThermal 模块文档

## 模块概述

SKGThermal 模块提供了完整的热成像系统，支持热特征管理、温度变化模拟、冷却机制等功能。该模块专为现代射击游戏中的热成像设备设计，提供了真实的热成像体验，包括热源识别、温度衰减、热特征渲染等核心功能。

## 核心组件

### USKGThermalComponent

**类路径**: `Source/SKGThermal/Public/Components/SKGThermalComponent.h`

这是热成像系统的核心组件，负责管理热特征的所有参数和状态。

#### 主要功能
- **热特征管理**: 管理物体的热特征值（0-1范围）
- **温度模拟**: 模拟物体的温度变化和冷却过程
- **网格控制**: 控制相关网格组件的热成像渲染
- **模板值设置**: 使用模板缓冲区值进行热成像识别
- **网络同步**: 支持热特征状态的网络同步
- **冷却机制**: 自动温度衰减和冷却控制

#### 核心数据结构

##### FSKGThermalReplicatedHeat
```cpp
USTRUCT()
struct FSKGThermalReplicatedHeat
{
    GENERATED_BODY()
    
    // 热特征值（0-1）
    UPROPERTY()
    float HeatValue {0.0f};
    
    // 是否正在冷却
    UPROPERTY()
    bool bIsCooling {false};
};
```

#### 核心属性

##### 状态变量
```cpp
// 热特征设置数据资产
UPROPERTY()
TObjectPtr<USKGPDAThermalSettings> ThermalSettings;

// 复制的热特征状态（网络同步）
UPROPERTY(ReplicatedUsing = OnRep_ReplicatedHeat)
FSKGThermalReplicatedHeat ReplicatedHeat;

// 受影响的网格组件数组
UPROPERTY()
TArray<TObjectPtr<UMeshComponent>> MeshComponents;

// 材质参数集合实例
UPROPERTY()
TObjectPtr<UMaterialParameterCollectionInstance> MPCThermalInstance;

// 热特征值（本地）
float HeatValue {1.0f};

// 冷却参数
float CoolingInterval {0.0f};
float CoolingStepAmount {0.0f};

// 网络同步时间戳
float ServerReplicateHeatTimestamp {0.0f};

// 冷却定时器
FTimerHandle TCoolingHandle;
```

#### 主要方法

##### 热特征管理
```cpp
/**
 * 设置热特征值（0-1范围）
 * @param Value 热特征值
 */
UFUNCTION(BlueprintCallable, Category = "SKGThermal")
void SetHeatValue(const float Value);

/**
 * 开始冷却过程
 */
UFUNCTION(BlueprintCallable, Category = "SKGThermal")
void StartCooling();

/**
 * 停止冷却过程
 */
UFUNCTION(BlueprintCallable, Category = "SKGThermal")
void StopCooling();
```

##### 组件初始化
```cppn/**
 * 初始化热成像组件
 * @param Settings 热成像设置数据资产
 * @param MPC 材质参数集合
 * @param ComponentOverride 手动指定的网格组件（可选）
 */
UFUNCTION(BlueprintCallable, Category = "SKGThermal")
void SetupThermalComponent(USKGPDAThermalSettings* Settings, UMaterialParameterCollection* MPC, TArray<UMeshComponent*> ComponentOverride);
```

##### 网络权限
```cpp
FORCEINLINE bool HasAuthority() const { return GetOwnerRole() == ROLE_Authority; }
```

## 数据资产系统

### USKGPDAThermalSettings

**类路径**: `Source/SKGThermal/Public/DataAssets/SKGPDAThermalSettings.h`

管理热成像设置的数据资产。

#### 核心属性

```cpp
UCLASS()
class SKGTHERMAL_API USKGPDAThermalSettings : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    // 热组件标签（用于自动发现热组件）
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGThermal")
    FName HotComponentTag {NAME_None};
    
    // 冷却时间（从热到冷所需时间，秒）
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGThermal")
    float TimeToCool {10.0f};
    
    // 起始热特征值
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGThermal")
    float StartingHeatValue {1.0f};
};
```

## 使用方法

### 基础设置

1. **添加组件**: 在需要热成像的 Actor 上添加 `USKGThermalComponent`
2. **配置数据资产**: 设置热成像参数（冷却时间、起始值等）
3. **初始化组件**: 调用 SetupThermalComponent 进行初始化
4. **控制热特征**: 设置热特征值和冷却状态

### 基础热成像使用

```cpp
// 获取热成像组件
USKGThermalComponent* ThermalComponent = Actor->FindComponentByClass<USKGThermalComponent>();

// 设置热特征值（0 = 冷，1 = 热）
ThermalComponent->SetHeatValue(0.8f);  // 80%热特征

// 开始冷却
ThermalComponent->StartCooling();

// 停止冷却
ThermalComponent->StopCooling();
```

### 组件初始化

```cpp
// 创建数据资产
USKGPDAThermalSettings* ThermalSettings = NewObject<USKGPDAThermalSettings>();
ThermalSettings->HotComponentTag = FName("ThermalBody");
ThermalSettings->TimeToCool = 15.0f;  // 15秒冷却时间
ThermalSettings->StartingHeatValue = 1.0f;  // 起始为热状态

// 获取相关网格组件
TArray<UMeshComponent*> ThermalMeshComponents;
Actor->GetComponents<UMeshComponent>(ThermalMeshComponents);

// 初始化热成像组件
ThermalComponent->SetupThermalComponent(ThermalSettings, ThermalMPC, ThermalMeshComponents);
```

## 高级功能

### 热特征计算

内部实现热特征值的计算和转换：

```cpp
int32 USKGThermalComponent::GetStencilValue(const float Heat) const
{
    // 将0-1的热特征值转换为模板缓冲区值（100-255）
    return FMath::Clamp(
        FMath::Lerp(ThermalStencilMinValue, ThermalStencilMaxValue, Heat), 
        ThermalStencilMinValue, 
        ThermalStencilMaxValue
    );
}
```

### 冷却机制

自动冷却系统实现温度衰减：

```cpp
void StartCooling()
{
    if (HasAuthority())
    {
        ReplicatedHeat.bIsCooling = true;
        
        // 计算冷却参数
        CoolingStepAmount = ReplicatedHeat.HeatValue / (ThermalSettings->TimeToCool / 0.1f);
        CoolingInterval = 0.1f;
        
        // 启动冷却定时器
        GetWorld()->GetTimerManager().SetTimer(TCoolingHandle, this, &USKGThermalComponent::CoolDown, CoolingInterval, true);
    }
}

void CoolDown()
{
    if (ReplicatedHeat.HeatValue > 0.0f)
    {
        ReplicatedHeat.HeatValue = FMath::Max(0.0f, ReplicatedHeat.HeatValue - CoolingStepAmount);
        SetMeshComponentsStencilValue(GetStencilValue(ReplicatedHeat.HeatValue));
        
        if (ReplicatedHeat.HeatValue <= 0.0f)
        {
            StopCooling();
        }
    }
}
```

### 网格渲染控制

控制相关网格组件的渲染设置：

```cpp
void SetMeshComponentsRenderCustomDepth(const bool bCustomDepth)
{
    for (UMeshComponent* MeshComponent : MeshComponents)
    {
        if (MeshComponent)
        {
            MeshComponent->SetRenderCustomDepth(bCustomDepth);
        }
    }
}

void SetMeshComponentsStencilValue(const int32 StencilValue)
{
    for (UMeshComponent* MeshComponent : MeshComponents)
    {
        if (MeshComponent)
        {
            MeshComponent->SetCustomDepthStencilValue(StencilValue);
        }
    }
}
```

## 配置示例

### 人体热特征配置

```cpp
// 创建热成像设置
USKGPDAThermalSettings* Settings = NewObject<USKGPDAThermalSettings>();
Settings->HotComponentTag = FName("CharacterBody");
Settings->TimeToCool = 20.0f;  // 20秒冷却时间
Settings->StartingHeatValue = 0.9f;  // 人体保持较高温度

// 应用设置
ThermalComponent->SetupThermalComponent(Settings, ThermalMPC, CharacterMeshComponents);
```

### 车辆热特征配置

```cpp
// 车辆热特征
USKGPDAThermalSettings* Settings = NewObject<USKGPDAThermalSettings>();
Settings->HotComponentTag = FName("VehicleEngine");
Settings->TimeToCool = 60.0f;  // 60秒冷却时间（发动机冷却慢）
Settings->StartingHeatValue = 1.0f;  // 发动机非常热

// 应用设置
ThermalComponent->SetupThermalComponent(Settings, ThermalMPC, VehicleMeshComponents);
```

### 环境物体配置

```cpp
// 环境物体（相对较冷）
USKGPDAThermalSettings* Settings = NewObject<USKGPDAThermalSettings>();
Settings->HotComponentTag = FName("EnvironmentObject");
Settings->TimeToCool = 5.0f;  // 5秒冷却时间
Settings->StartingHeatValue = 0.3f;  // 起始温度较低

// 应用设置
ThermalComponent->SetupThermalComponent(Settings, ThermalMPC, ObjectMeshComponents);
```

## 集成应用

### 与热成像瞄准镜集成

```cpp
// 在热成像瞄准镜中使用热特征数据
void USKGOpticComponent::UpdateThermalView()
{
    // 获取视野内的所有热成像组件
    TArray<USKGThermalComponent*> ThermalComponents;
    GetVisibleThermalComponents(ThermalComponents);
    
    for (USKGThermalComponent* ThermalComponent : ThermalComponents)
    {
        float HeatValue = ThermalComponent->ReplicatedHeat.HeatValue;
        
        // 根据热特征值调整颜色
        FLinearColor ThermalColor = GetThermalColor(HeatValue);
        
        // 应用热成像效果
        ApplyThermalEffect(ThermalComponent->GetOwner(), ThermalColor);
    }
}
```

### 与武器系统集成

```cpp
// 武器射击后的热特征变化
void AMyWeapon::OnFire()
{
    USKGThermalComponent* ThermalComponent = FindComponentByClass<USKGThermalComponent>();
    if (ThermalComponent)
    {
        // 射击时增加热特征（枪管发热）
        float CurrentHeat = ThermalComponent->ReplicatedHeat.HeatValue;
        float NewHeat = FMath::Min(1.0f, CurrentHeat + 0.1f);
        ThermalComponent->SetHeatValue(NewHeat);
        
        // 开始冷却（枪管散热）
        if (!ThermalComponent->ReplicatedHeat.bIsCooling)
        {
            ThermalComponent->StartCooling();
        }
    }
}
```

### 与环境系统集成

```cpp
// 环境温度的影响
void AMyEnvironment::UpdateThermalState()
{
    USKGThermalComponent* ThermalComponent = FindComponentByClass<USKGThermalComponent>();
    if (ThermalComponent)
    {
        // 根据环境温度调整基础热特征
        float AmbientTemperature = GetAmbientTemperature();
        float BaseHeat = FMath::Clamp((AmbientTemperature - 0.0f) / 40.0f, 0.1f, 0.5f);
        
        // 太阳照射影响
        if (IsInDirectSunlight())
        {
            BaseHeat += 0.3f;
        }
        
        ThermalComponent->SetHeatValue(FMath::Min(1.0f, BaseHeat));
    }
}
```

## 技术细节

### 模板缓冲区系统

使用模板缓冲区值进行热成像识别：

```cpp
// 模板值范围定义
float ThermalStencilMinValue {100.0f};
float ThermalStencilMaxValue {255.0f};

int32 USKGThermalComponent::GetStencilValue(const float Heat) const
{
    // 将热特征值映射到模板缓冲区范围
    return FMath::Lerp(ThermalStencilMinValue, ThermalStencilMaxValue, Heat);
}
```

### 网络同步机制

```cpp
void OnRep_ReplicatedHeat()
{
    // 网络同步时更新本地状态
    HeatValue = ReplicatedHeat.HeatValue;
    
    // 更新网格组件的模板值
    SetMeshComponentsStencilValue(GetStencilValue(HeatValue));
    
    // 处理冷却状态
    if (ReplicatedHeat.bIsCooling && !TCoolingHandle.IsValid())
    {
        // 重新启动冷却定时器
        SetupCoolingTimer();
    }
    else if (!ReplicatedHeat.bIsCooling && TCoolingHandle.IsValid())
    {
        // 停止冷却定时器
        GetWorld()->GetTimerManager().ClearTimer(TCoolingHandle);
    }
}
```

### 材质参数集合集成

```cpp
void SetupThermalComponent(USKGPDAThermalSettings* Settings, UMaterialParameterCollection* MPC, TArray<UMeshComponent*> ComponentOverride)
{
    ThermalSettings = Settings;
    
    if (MPC)
    {
        // 获取材质参数集合实例
        MPCThermalInstance = GetWorld()->GetParameterCollectionInstance(MPC);
    }
    
    // 设置网格组件
    if (ComponentOverride.Num() > 0)
    {
        MeshComponents = ComponentOverride;
    }
    else if (Settings->HotComponentTag != NAME_None)
    {
        // 根据标签自动发现组件
        FindComponentsByTag(Settings->HotComponentTag);
    }
    
    // 初始化热特征值
    ReplicatedHeat.HeatValue = Settings->StartingHeatValue;
    HeatValue = Settings->StartingHeatValue;
    
    // 应用初始状态
    SetMeshComponentsStencilValue(GetStencilValue(HeatValue));
}
```

## 性能优化

### 批量更新

```cpp
void SetMeshComponentsStencilValue(const int32 StencilValue)
{
    // 批量设置所有相关网格组件
    for (UMeshComponent* MeshComponent : MeshComponents)
    {
        if (MeshComponent && MeshComponent->IsValidLowLevel())
        {
            MeshComponent->SetCustomDepthStencilValue(StencilValue);
        }
    }
}
```

### 定时器管理

```cpp
void StopCooling()
{
    if (HasAuthority())
    {
        ReplicatedHeat.bIsCooling = false;
        
        // 清除冷却定时器
        if (TCoolingHandle.IsValid())
        {
            GetWorld()->GetTimerManager().ClearTimer(TCoolingHandle);
        }
    }
}
```

## 常见问题

### Q: 热成像效果不显示？
A: 检查网格组件是否正确设置了自定义深度渲染，确认材质支持热成像效果

### Q: 冷却过程不工作？
A: 确认冷却时间设置合理，检查定时器是否正确启动

### Q: 网络同步问题？
A: 确保服务器正确设置了热特征值，检查网络更新频率

### Q: 性能问题？
A: 合理设置冷却间隔，避免过频繁的更新操作

### Q: 热特征值异常？
A: 检查起始值设置，确认热特征值在0-1范围内

## 最佳实践

1. **合理设置冷却时间**: 根据物体类型设置合适的冷却时间
2. **批量处理**: 使用批量更新减少性能开销
3. **网络优化**: 合理设置网络同步频率
4. **材质支持**: 确保目标材质支持热成像效果
5. **标签系统**: 使用标签系统方便地管理热组件

## 相关模块

- [SKGOptic 模块](SKGOptic模块.md) - 光学瞄准镜系统
- [SKGNightVision 模块](SKGNightVision模块.md) - 夜视系统
- [SKGShooterFramework 模块](SKGShooterFramework模块.md) - 火器核心系统

<system-reminder>Whenever you read the file, you should consider whether the user explicitly asked for a summary, analysis, or explanation. If they did, provide it. If not, continue with the task as requested.}</content>