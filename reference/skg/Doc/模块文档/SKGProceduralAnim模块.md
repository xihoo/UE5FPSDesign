# SKGProceduralAnim 模块文档

## 模块概述

SKGProceduralAnim 模块是 SKGShooterFramework 的核心动画系统，提供了完整的程序化动画解决方案。该模块通过数据驱动的方式实现武器瞄准、后坐力、摆动、延迟等效果，无需手动制作大量动画，大大提高了开发效率并增强了游戏的真实感。

## 核心组件

### USKGProceduralAnimComponent

**类路径**: `Source/SKGProceduralAnim/Public/Components/SKGProceduralAnimComponent.h`

这是程序化动画系统的核心组件，统一管理所有程序化动画效果。

#### 主要功能
- **瞄准系统**: 支持多点瞄准和点射瞄准
- **后坐力模拟**: 真实的武器后坐力效果
- **移动摆动**: 基于角色移动的武器摆动
- **延迟效果**: 移动和旋转延迟模拟
- **姿态管理**: 多种持枪姿态支持
- **数据驱动**: 完全通过数据资产配置
- **网络同步**: 支持瞄准点的网络同步

#### 核心数据结构

##### FSKGProceduralAimingSettings
```cpp
struct FSKGProceduralAimingSettings
{
    bool bCanAim;                    // 是否可以瞄准
    float AimSpeed;                  // 瞄准速度
    float AimInTime;                 // 进入瞄准时间
    float AimOutTime;                // 退出瞄准时间
    // 其他瞄准相关参数
};
```

##### FSKGFirstAndThirdPersonBasePoseOffset
```cpp
struct FSKGFirstAndThirdPersonBasePoseOffset
{
    FTransform FirstPersonOffset;    // 第一人称偏移
    FTransform ThirdPersonOffset;    // 第三人称偏移
    // 其他姿态参数
};
```

##### FSKGMovementSwaySettings
```cpp
struct FSKGMovementSwaySettings
{
    float SwaySpeed;                 // 摆动速度
    float SwayAmount;                // 摆动幅度
    FVector SwayAxis;                // 摆动轴向
    // 其他摆动参数
};
```

#### 核心属性

##### 基础配置
```cpp
// 程序化网格组件名称
FName ProceduralMeshName {"StaticMesh"};

// 瞄准插槽名称数组
TArray<FName> ProceduralAimSocketNames {"S_Aim"};

// 左右手主导点射瞄准索引
int32 RightHandDominatePointAimAimSocketIndex {INDEX_NONE};
int32 LeftHandDominatePointAimAimSocketIndex {INDEX_NONE};

// 是否支持左手主导
bool bCanUseLeftHandDominate {false};

// 程序化动画标签
FGameplayTag ProceduralAnimGameplayTag;
```

##### 数据资产
```cpp
// 瞄准设置数据资产
UPROPERTY(EditDefaultsOnly, Replicated, Category = "SKGProceduralAnim")
TObjectPtr<USKGPDAAimingSettings> AimingSettingsDataAsset;

// 程序化偏移数据资产
UPROPERTY(EditDefaultsOnly, Replicated, Category = "SKGProceduralAnim")
TObjectPtr<USKGPDAProceduralOffsets> ProceduralOffsetsDataAsset;

// 循环瞄准点数据资产
UPROPERTY(EditDefaultsOnly, Replicated, Category = "SKGProceduralAnim")
TObjectPtr<USKGPDACycleAimingPointSettings> CycleAimingPointSettingsDataAsset;

// 移动摆动数据资产
UPROPERTY(EditDefaultsOnly, Replicated, Category = "SKGProceduralAnim")
TObjectPtr<USKGPDAMovementSwaySettings> MovementSwaySettingsDataAsset;

// 其他数据资产...
```

#### 主要方法

##### 瞄准系统
```cpp
// 获取瞄准设置
UFUNCTION(BlueprintPure, Category = "SKGProceduralAnimComponent|Aim")
const FSKGProceduralAimingSettings& GetProceduralAimingSettings() const;

// 检查是否可以瞄准
UFUNCTION(BlueprintPure, Category = "SKGProceduralAnimComponent|Aim")
bool CanAim() const;

// 设置瞄准能力
UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|Aim")
void SetCanAim(bool bCanAim);

// 获取瞄准世界变换
UFUNCTION(BlueprintPure, Category = "SKGProceduralAnimComponent|Aim")
FTransform GetAimWorldTransform() const;

// 循环瞄准插槽
UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|Aim")
bool CycleAimSocket();

// 开始点射瞄准
UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|Aim")
bool StartPointAiming(bool bRightHandDominant);

// 停止点射瞄准
UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|Aim")
void StopPointAiming();

// 更新瞄准偏移
UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnim|Aim")
void UpdateAimOffset(UMeshComponent* ComponentRelativeTo, FVector BaseDistance = FVector::ZeroVector);
```

##### 姿态管理
```cpp
// 获取姿态数据
UFUNCTION(BlueprintPure, Category = "SKGProceduralAnim|Pose")
bool GetPose(FGameplayTag Tag, FSKGToFromCurveSettings& PoseData);
```

##### 枪口变换计算
```cpp
// 获取瞄准枪口变换（可重写）
UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGProceduralAnimComponent|Muzzle")
FTransform GetAimMuzzleTransform();
```

##### 数据资产设置
```cpp
// 设置瞄准设置
UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
void SetAimingSettingsSettings(USKGPDAAimingSettings* DataAsset);

// 恢复默认瞄准设置
UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
void RevertAimingSettings();

// 其他数据资产设置方法...
```

#### 事件委托

```cpp
// 瞄准能力变化事件
UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
FOnCanAimChanged OnCanAimChanged;

// 点射瞄准状态变化事件
UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
FOnPointAimStateChanged OnPointAimStateChanged;

// 瞄准插槽循环事件
UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
FOnAimSocketCycled OnAimSocketCycled;

// 瞄准插槽网络同步事件
UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
FOnAimSocketCycledReplicated OnAimSocketCycledReplicated;

// 数据资产变化事件
UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
FOnProceduralDataAssetsChanged OnProceduralDataAssetsChanged;
```

## 使用方法

### 基础设置

1. **添加组件**: 在武器或角色 Actor 上添加 `USKGProceduralAnimComponent`
2. **配置网格**: 设置 `ProceduralMeshName` 指向目标网格组件
3. **定义瞄准点**: 配置 `ProceduralAimSocketNames` 数组定义可用瞄准点
4. **设置数据资产**: 配置各种程序化动画数据资产

### 瞄准系统使用

```cpp
// 获取程序化动画组件
USKGProceduralAnimComponent* ProceduralComponent = Weapon->FindComponentByClass<USKGProceduralAnimComponent>();

// 检查是否可以瞄准
if (ProceduralComponent->CanAim())
{
    // 获取瞄准变换
    FTransform AimTransform = ProceduralComponent->GetAimWorldTransform();
    
    // 更新瞄准偏移
    ProceduralComponent->UpdateAimOffset(WeaponMesh);
}

// 循环瞄准点
ProceduralComponent->CycleAimSocket();

// 开始点射瞄准
ProceduralComponent->StartPointAiming(true); // 右手主导
```

### 事件监听

```cpp
// 绑定瞄准能力变化事件
ProceduralComponent->OnCanAimChanged.AddDynamic(this, &AMyWeapon::OnCanAimChanged);

// 处理瞄准能力变化
void AMyWeapon::OnCanAimChanged(const USKGProceduralAnimComponent* Component, const bool bCanAim)
{
    // 更新UI或播放动画
    UpdateAimingUI(bCanAim);
}

// 绑定瞄准插槽变化事件
ProceduralComponent->OnAimSocketCycled.AddDynamic(this, &AMyWeapon::OnAimSocketCycled);

// 处理瞄准插槽变化
void AMyWeapon::OnAimSocketCycled(const FName& NewSocket)
{
    // 更新瞄准点指示器
    UpdateAimPointIndicator(NewSocket);
}
```

### 姿态系统使用

```cpp
// 获取特定姿态
FSKGToFromCurveSettings PoseData;
if (ProceduralComponent->GetPose(FGameplayTag::RequestGameplayTag("Pose.Crouching"), PoseData))
{
    // 应用姿态偏移
    ApplyPoseOffset(PoseData);
}
```

## 高级功能

### 数据资产动态切换

```cpp
// 运行时切换瞄准设置
ProceduralComponent->SetAimingSettingsSettings(NewAimingSettings);

// 恢复默认设置
ProceduralComponent->RevertAimingSettings();

// 恢复所有默认设置
ProceduralComponent->RevertAllSettingsToDefault();
```

### 网络同步

瞄准插槽支持网络同步：

```cpp
// 服务器设置瞄准插槽
UFUNCTION(Server, Reliable, WithValidation)
void Server_SetAimSocketIndex(uint8 Index);

// 客户端复制
UPROPERTY(ReplicatedUsing = OnRep_AimSocketIndex)
uint8 AimSocketIndex {0};
```

### 左右手支持

```cpp
// 检查是否支持左手主导
if (ProceduralComponent->CanUseLeftHandDominate())
{
    // 开始左手点射瞄准
    ProceduralComponent->StartPointAiming(false);
}
```

## 技术细节

### 数据驱动架构

所有动画参数都通过数据资产配置：

```cpp
UCLASS()
class USKGPDAAimingSettings : public UPrimaryDataAsset
{
    // 瞄准参数配置
};

UCLASS()
class USKGPDARecoilSettings : public UPrimaryDataAsset
{
    // 后坐力参数配置
};

UCLASS()
class USKGPDAMovementSwaySettings : public UPrimaryDataAsset
{
    // 摆动参数配置
};
```

### 性能优化

- **缓存机制**: 缓存网格组件和变换数据
- **事件驱动**: 使用委托系统减少轮询
- **数据复用**: 支持数据资产的动态切换和重用

### 网络架构

```cpp
// 服务器授权模式
UFUNCTION(Server, Reliable, WithValidation)
void Server_SetAimSocketIndex(uint8 Index);

// 状态复制
UPROPERTY(ReplicatedUsing = OnRep_AimSocketIndex)
uint8 AimSocketIndex;
```

## 配置示例

### 标准武器配置

```cpp
// 基础配置
ProceduralMeshName = "WeaponMesh";
ProceduralAimSocketNames = {"S_Aim", "S_Aim_2", "S_Aim_3"};
RightHandDominatePointAimAimSocketIndex = 1;
LeftHandDominatePointAimAimSocketIndex = 2;
bCanUseLeftHandDominate = true;

// 数据资产配置
AimingSettingsDataAsset = LoadObject<USKGPDAAimingSettings>(...);
RecoilSettingsDataAsset = LoadObject<USKGPDARecoilSettings>(...);
MovementSwaySettingsDataAsset = LoadObject<USKGPDAMovementSwaySettings>(...);
```

### 光学设备配置

```cpp
// 光学设备专用配置
ProceduralAnimGameplayTag = FGameplayTag::RequestGameplayTag("Procedural.Optic");
ProceduralAimSocketNames = {"S_AimPoint"};
RightHandDominatePointAimAimSocketIndex = INDEX_NONE; // 光学设备不支持点射
```

## 最佳实践

1. **数据资产组织**: 合理组织数据资产，便于管理和复用
2. **性能考虑**: 避免频繁的瞄准点切换，合理设置更新频率
3. **网络优化**: 合理设置网络复制频率，避免状态同步问题
4. **用户体验**: 提供清晰的瞄准点指示和切换反馈
5. **兼容性**: 确保与附件系统的正确集成

## 常见问题

### Q: 瞄准偏移不更新？
A: 检查是否正确调用了 `UpdateAimOffset()` 方法并传入了正确的组件

### Q: 瞄准插槽不循环？
A: 确保配置了多个瞄准插槽并且组件已正确初始化

### Q: 网络同步问题？
A: 检查服务器授权和客户端复制设置

### Q: 数据资产不生效？
A: 确保数据资产已正确配置并设置了复制属性

## 相关模块

- [SKGAnimGraphNodes 模块](SKGAnimGraphNodes模块.md) - 动画图表节点
- [SKGAttachment 模块](SKGAttachment模块.md) - 附件管理系统
- [SKGOptic 模块](SKGOptic模块.md) - 光学瞄准镜系统
- [SKGShooterFrameworkCore 模块](SKGShooterFrameworkCore模块.md) - 核心框架功能

## 扩展应用

### 自定义动画效果

通过继承和扩展可以实现：
- 武器特定动画效果
- 环境影响的动画变化
- 技能相关的动画修改
- AI行为的动画表现

### 性能分析

内置性能统计：
```cpp
DECLARE_STATS_GROUP(TEXT("SKGProceduralAnimComponent"), STATGROUP_SKGProceduralAnimComponent, STATCAT_Advanced);
```

### 调试支持

支持运行时调试：
- 瞄准点可视化
- 偏移量实时显示
- 数据资产变化监控
- 网络状态检查