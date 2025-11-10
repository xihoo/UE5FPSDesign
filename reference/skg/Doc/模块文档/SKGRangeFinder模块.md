# SKGRangeFinder 模块文档

## 模块概述

SKGRangeFinder 模块提供了完整的激光测距仪系统，支持距离测量、单位转换、自动测距等功能。该模块专为现代射击游戏中的测距设备设计，提供了类似真实测距仪的体验，包括激光测距、距离显示、单位切换等核心功能。

## 核心组件

### USKGRangeFinderComponent

**类路径**: `Source/SKGRangeFinder/Public/Components/SKGRangeFinderComponent.h`

这是测距仪系统的核心组件，负责执行距离测量和管理测距仪的所有功能。

#### 主要功能
- **激光测距**: 基于光线追踪的距离测量
- **单位转换**: 支持多种测量单位（米、码、英寸、厘米）
- **自动测距**: 支持定时自动测量
- **文本显示**: 可选的距离文本显示
- **事件通知**: 测量完成时触发事件
- **网络优化**: 高效的网络同步机制

#### 核心属性

##### 基础配置
```cpp
// 测距仪网格组件名称
UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder|Initialize")
FName RangeFinderMeshName {"StaticMesh"};

// 激光发射插槽名称
UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder|Initialize")
FName RangeFinderLaserSocketName {"S_Laser"};

// 文本渲染组件名称（可选）
UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder|Initialize")
FName TextRenderComponentName {"TextRenderComponent"};

// 数据资产设置
UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder|Settings")
TObjectPtr<USKGPDARangeFinderSettings> RangeFinderSettingsDataAsset;
```

##### 测量参数
```cpp
// 最小测量距离
float MinTraceDistance {0.0f};

// 最大测量距离
float MaxTraceDistance {100000.0f};  // 1000米

// 碰撞通道
ECollisionChannel RangeFinderCollisionChannel {ECC_Visibility};
```

#### 主要方法

##### 距离测量
```cpp
/**
 * 执行单次距离测量
 * @return 测量到的距离（单位：厘米）
 */
UFUNCTION(BlueprintCallable, Category = "SKGRangeFinder|Range")
float GetRange() const;

/**
 * 获取测量距离并格式化为字符串
 * @return 格式化后的距离字符串（如 "250m" 或 "273yd"）
 */
UFUNCTION(BlueprintCallable, Category = "SKGRangeFinder|Range")
FString GetRangeAsString() const;
```

##### 组件获取
```cpp
// 获取测距仪网格组件
UFUNCTION(BlueprintGetter)
UMeshComponent* GetRangeFinderMesh() const { return RangeFinderMesh; }

// 获取文本渲染组件
UFUNCTION(BlueprintGetter)
UTextRenderComponent* GetTextRenderComponent() const { return TextRenderComponent; }

// 获取测距仪组件（静态方法）
UFUNCTION(BlueprintPure, Category = "SKGShooterFrameworkStatics|Getters")
static USKGRangeFinderComponent* GetRangeFinderComponent(const AActor* Actor);
```

#### 事件委托

```cpp
// 测量完成事件
UPROPERTY(BlueprintAssignable, Category = "SKGRangeFinder|Events")
FOnRanged OnRanged;
```

## 数据资产系统

### USKGPDARangeFinderSettings

**类路径**: `Source/SKGRangeFinder/Public/DataAssets/SKGPDARangeFinderSettings.h`

管理测距仪设置的数据资产。

#### 核心结构

##### FSKGMeasurementSymbols
```cpp
USTRUCT(BlueprintType)
struct FSKGMeasurementSymbols
{
    GENERATED_BODY()
    
    // 米单位符号
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SKGRangeFinder|Settings")
    FString MeterSymbol {"m"};
    
    // 码单位符号
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SKGRangeFinder|Settings")
    FString YardSymbol {"yd"};
    
    // 英寸单位符号
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SKGRangeFinder|Settings")
    FString InchSymbol {"in"};
    
    // 厘米单位符号
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SKGRangeFinder|Settings")
    FString CentimeterSymbol {"cm"};
};
```

##### 测距仪设置
```cpp
UCLASS()
class SKGRANGEFINDER_API USKGPDARangeFinderSettings : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    // 游戏标签
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGRangeFinder")
    FGameplayTagContainer GameplayTags;
    
    // 最小测量距离
    UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder")
    float MinTraceDistance {0.0f};
    
    // 最大测量距离
    UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder")
    float MaxTraceDistance {100000.0f};
    
    // 测量单位符号
    UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder")
    FSKGMeasurementSymbols MeasurementSymbols;
    
    // 是否使用码/米单位
    UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder")
    bool bUseInYardMeter {true};
    
    // 是否使用公制单位
    UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder")
    bool bUseMetric {true};
    
    // 是否自动测量
    UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder")
    bool bAutoRange {false};
    
    // 自动测量间隔
    UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder", meta = (EditCondition = "bAutoRange"))
    float AutoRangeInterval {1.0f};
};
```

## 使用方法

### 基础设置

1. **添加组件**: 在武器或设备 Actor 上添加 `USKGRangeFinderComponent`
2. **配置数据资产**: 设置测距仪参数（距离范围、单位等）
3. **定义插槽**: 配置激光发射的插槽位置
4. **可选文本显示**: 设置文本渲染组件用于显示距离

### 基础测距使用

```cpp
// 获取测距仪组件
USKGRangeFinderComponent* RangeFinderComponent = Device->FindComponentByClass<USKGRangeFinderComponent>();

// 执行单次测量
float Distance = RangeFinderComponent->GetRange();  // 返回厘米单位

// 获取格式化字符串
FString DistanceString = RangeFinderComponent->GetRangeAsString();  // 如 "250m"
```

### 事件监听

```cpp
// 绑定测量完成事件
RangeFinderComponent->OnRanged.AddDynamic(this, &AMyDevice::OnDistanceMeasured);

// 处理测量结果
void AMyDevice::OnDistanceMeasured(float Distance)
{
    // 将厘米转换为米
    float DistanceInMeters = Distance / 100.0f;
    
    // 更新UI显示
    UpdateDistanceDisplay(DistanceInMeters);
    
    // 调整瞄准点
    AdjustAimPoint(DistanceInMeters);
    
    // 记录日志
    UE_LOG(LogTemp, Log, TEXT("Measured distance: %.1f meters"), DistanceInMeters);
}
```

### 自动测距配置

```cpp
// 配置自动测距
USKGPDARangeFinderSettings* Settings = NewObject<USKGPDARangeFinderSettings>();
Settings->bAutoRange = true;
Settings->AutoRangeInterval = 0.5f;  // 每0.5秒测量一次
Settings->MaxTraceDistance = 200000.0f;  // 2000米最大距离
Settings->bUseMetric = true;  // 使用公制单位

RangeFinderComponent->RangeFinderSettingsDataAsset = Settings;
```

## 高级功能

### 单位系统

组件支持多种测量单位：

```cpp
// 公制单位（米/厘米）
Settings->bUseMetric = true;
Settings->bUseInYardMeter = true;

// 英制单位（码/英寸）
Settings->bUseMetric = false;
Settings->bUseInYardMeter = true;

// 自定义符号
Settings->MeasurementSymbols.MeterSymbol = "m";
Settings->MeasurementSymbols.YardSymbol = "yd";
Settings->MeasurementSymbols.InchSymbol = "in";
Settings->MeasurementSymbols.CentimeterSymbol = "cm";
```

### 距离转换

内部实现距离转换逻辑：

```cpp
float USKGRangeFinderComponent::ConvertDistance(float Distance) const
{
    if (bUseMetric)
    {
        if (bUseInYardMeter)
        {
            return Distance / 100.0f;  // 厘米转米
        }
        else
        {
            return Distance;  // 保持厘米
        }
    }
    else
    {
        if (bUseInYardMeter)
        {
            return Distance * 0.0109361f;  // 厘米转码
        }
        else
        {
            return Distance * 0.393701f;   // 厘米转英寸
        }
    }
}
```

### 碰撞检测

使用光线追踪进行距离测量：

```cpp
bool USKGRangeFinderComponent::PerformTrace(FHitResult& HitResult) const
{
    if (RangeFinderMesh)
    {
        FVector StartLocation = RangeFinderMesh->GetSocketLocation(RangeFinderLaserSocketName);
        FVector EndLocation = StartLocation + (RangeFinderMesh->GetSocketRotation(RangeFinderLaserSocketName).Vector() * MaxTraceDistance);
        
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(GetOwner());
        
        return GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, RangeFinderCollisionChannel, QueryParams);
    }
    return false;
}
```

## 配置示例

### 手持式测距仪配置

```cpp
// 基础配置
RangeFinderMeshName = "RangeFinderMesh";
RangeFinderLaserSocketName = "S_Laser";
TextRenderComponentName = "DistanceDisplay";

// 设置参数
USKGPDARangeFinderSettings* Settings = NewObject<USKGPDARangeFinderSettings>();
Settings->MinTraceDistance = 100.0f;     // 1米最小距离
Settings->MaxTraceDistance = 50000.0f;   // 500米最大距离
Settings->bAutoRange = false;            // 手动触发测量
Settings->bUseMetric = true;             // 使用公制单位
Settings->bUseInYardMeter = true;        // 米/码单位

RangeFinderComponent->RangeFinderSettingsDataAsset = Settings;
```

### 武器集成测距仪配置

```cpp
// 武器集成配置
RangeFinderMeshName = "WeaponOpticMesh";
RangeFinderLaserSocketName = "S_RangeFinder";
TextRenderComponentName = "";  // 不显示文本，通过UI显示

// 设置参数
USKGPDARangeFinderSettings* Settings = NewObject<USKGPDARangeFinderSettings>();
Settings->MinTraceDistance = 0.0f;       // 零距离测量
Settings->MaxTraceDistance = 200000.0f;  // 2000米最大距离
Settings->bAutoRange = true;             // 自动测量
Settings->AutoRangeInterval = 0.2f;      // 0.2秒间隔
Settings->bUseMetric = true;             // 使用公制单位

RangeFinderComponent->RangeFinderSettingsDataAsset = Settings;
```

### 狙击测距仪配置

```cpp
// 高精度测距配置
USKGPDARangeFinderSettings* Settings = NewObject<USKGPDARangeFinderSettings>();
Settings->MinTraceDistance = 0.0f;       // 零距离测量
Settings->MaxTraceDistance = 300000.0f;  // 3000米最大距离
Settings->bAutoRange = true;             // 自动测量
Settings->AutoRangeInterval = 1.0f;      // 1秒间隔（高精度）
Settings->bUseMetric = true;             // 使用公制单位
Settings->MeasurementSymbols.MeterSymbol = "m";  // 标准米符号

RangeFinderComponent->RangeFinderSettingsDataAsset = Settings;
```

## 集成应用

### 与瞄准镜系统集成

```cpp
// 在瞄准镜中使用测距数据
void USKGOpticComponent::UpdateAimPoint()
{
    USKGRangeFinderComponent* RangeFinder = GetOwner()->FindComponentByClass<USKGRangeFinderComponent>();
    if (RangeFinder)
    {
        float Distance = RangeFinder->GetRange() / 100.0f;  // 转换为米
        
        // 根据距离调整瞄准点
        FVector AimOffset = CalculateBallisticOffset(Distance);
        ApplyAimOffset(AimOffset);
    }
}
```

### 与弹道计算集成

```cpp
// 弹道计算器使用测距数据
void CalculateBallisticSolution()
{
    USKGRangeFinderComponent* RangeFinder = GetRangeFinderComponent();
    if (RangeFinder)
    {
        float Distance = RangeFinder->GetRange() / 100.0f;  // 米单位
        
        // 计算弹道补偿
        float ElevationAdjustment = CalculateElevation(Distance);
        float WindageAdjustment = CalculateWindage(Distance, WindSpeed, WindDirection);
        
        // 应用瞄准调整
        ApplyBallisticCorrection(ElevationAdjustment, WindageAdjustment);
    }
}
```

### 用户界面集成

```cpp
// HUD显示测距结果
void AMyHUD::DrawHUD()
{
    USKGRangeFinderComponent* RangeFinder = GetOwningPawn()->FindComponentByClass<USKGRangeFinderComponent>();
    if (RangeFinder && RangeFinder->IsActive())
    {
        FString DistanceText = RangeFinder->GetRangeAsString();
        
        // 绘制距离文本
        DrawText(DistanceText, FLinearColor::Green, Canvas->SizeX * 0.8f, Canvas->SizeY * 0.2f, HUDFont);
        
        // 绘制测距标线
        DrawRect(FLinearColor::Green, Canvas->SizeX * 0.5f - 2, Canvas->SizeY * 0.5f - 10, 4, 20);
        DrawRect(FLinearColor::Green, Canvas->SizeX * 0.5f - 10, Canvas->SizeY * 0.5f - 2, 20, 4);
    }
}
```

## 技术细节

### 测量精度

测距精度取决于以下因素：
- 光线追踪的精度
- 碰撞检测的分辨率
- 浮点数的精度限制
- 网络同步的延迟

### 性能考虑

```cpp
void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    if (bAutoRange)
    {
        AutoRangeTimer += DeltaTime;
        if (AutoRangeTimer >= AutoRangeInterval)
        {
            PerformRangeMeasurement();
            AutoRangeTimer = 0.0f;
        }
    }
}
```

### 网络优化

```cpp
FORCEINLINE void TryForceNetUpdate() const
{
    if (bAutoCallForceNetUpdate)
    {
        GetOwner()->ForceNetUpdate();
    }
}
```

## 常见问题

### Q: 测距结果不准确？
A: 检查碰撞通道设置，确保测距激光能够击中目标物体

### Q: 自动测距不工作？
A: 确认 `bAutoRange` 设置为true，并且 `AutoRangeInterval` 设置合理

### Q: 单位显示错误？
A: 检查 `bUseMetric` 和 `bUseInYardMeter` 设置，确认单位符号配置正确

### Q: 性能问题？
A: 合理设置自动测距间隔，避免过频繁的测量操作

### Q: 文本不更新？
A: 确认文本渲染组件名称设置正确，组件已正确初始化

## 最佳实践

1. **合理设置测量范围**: 根据实际应用场景设置最小/最大测量距离
2. **优化测量频率**: 平衡实时性和性能，避免过度频繁的测量
3. **单位一致性**: 确保整个游戏使用一致的测量单位系统
4. **视觉反馈**: 提供清晰的测距视觉反馈，如激光点或标线
5. **错误处理**: 处理测量失败的情况，如无目标或超出范围

## 相关模块

- [SKGOptic 模块](SKGOptic模块.md) - 光学瞄准镜系统
- [SKGShooterFramework 模块](SKGShooterFramework模块.md) - 火器核心系统
- [SKGProjectile 模块](SKGProjectile模块.md) - 弹道系统

<system-reminder>Whenever you read the file, you should consider whether the user explicitly asked for a summary, analysis, or explanation. If they did, provide it. If not, continue with the task as requested.}</content>