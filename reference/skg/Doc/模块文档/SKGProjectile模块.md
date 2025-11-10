# SKGProjectile 模块文档

## 模块概述

SKGProjectile 模块提供了完整的弹道系统，支持各种类型投射物的物理模拟、碰撞检测、粒子效果和调试功能。该模块专为真实射击体验设计，包含了空气阻力、重力、风速等物理因素的计算，以及高效的碰撞检测和网络同步机制。

## 核心组件

### USKGPDAProjectile

**类路径**: `Source/SKGProjectile/Public/DataAssets/SKGPDAProjectile.h`

这是弹道数据资产，定义了投射物的基本物理属性和行为参数。

#### 主要功能
- **物理参数**: 速度、重量、空气阻力系数
- **生命周期**: 投射物存在时间
- **碰撞设置**: 碰撞通道和响应
- **粒子效果**: 轨迹和撞击效果
- **调试支持**: 路径和撞击点可视化

#### 核心属性

##### 物理属性
```cpp
// 投射物速度（厘米/秒）
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGProjectile")
double Velocity {75000.0};  // 默认 750 m/s

// 投射物重量（格令）
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGProjectile")
double Weight {55.0};  // 标准 5.56mm 子弹重量

// 空气阻力系数
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGProjectile")
double DragCoefficient {0.283};  // 标准弹形系数
```

##### 生命周期
```cpp
// 投射物存在时间（秒）
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGProjectile")
float Lifetime {6.0f};
```

##### 碰撞设置
```cpp
// 碰撞通道
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGProjectile")
TEnumAsByte<ECollisionChannel> CollisionChannel {ECC_Visibility};
```

#### 子系统支持

模块还包含专门的子系统来处理投射物的世界管理：

```cpp
// 世界子系统管理
class USKGProjectileWorldSubsystem : public UWorldSubsystem
{
    // 投射物生命周期管理
    // 性能监控
    // 批量处理优化
};
```

## 使用方法

### 基础配置

1. **创建数据资产**: 创建 `USKGPDAProjectile` 的子类数据资产
2. **配置物理参数**: 设置速度、重量、阻力系数
3. **设置碰撞**: 配置碰撞通道和响应
4. **添加效果**: 设置轨迹和撞击粒子效果

### 数据资产配置示例

```cpp
// 创建标准步枪弹数据资产
USKGPDAProjectile* RifleBullet = NewObject<USKGPDAProjectile>();
RifleBullet->Velocity = 94000.0f;        // 940 m/s (5.56mm NATO)
RifleBullet->Weight = 62.0f;             // 62格令
RifleBullet->DragCoefficient = 0.307f;   // 阻力系数
RifleBullet->Lifetime = 4.0f;            // 4秒生命周期
RifleBullet->CollisionChannel = ECC_Visibility;  // 可见性碰撞

// 配置粒子效果
RifleBullet->ParticleData.Particle = LoadObject<UNiagaraSystem>(...);  // 轨迹效果
RifleBullet->ParticleData.ParticleSpawnDelayDistance = 5000.0f;       // 50米延迟
```

### 高级弹道配置

#### 手枪弹配置
```cpp
// 9mm 手枪弹
PistolBullet->Velocity = 36000.0f;       // 360 m/s
PistolBullet->Weight = 124.0f;           // 124格令
PistolBullet->DragCoefficient = 0.165f;  // 较低阻力
PistolBullet->Lifetime = 2.0f;           // 较短距离
```

#### 狙击弹配置
```cpp
// .50 BMG 狙击弹
SniperBullet->Velocity = 85000.0f;       // 850 m/s
SniperBullet->Weight = 750.0f;           // 750格令
SniperBullet->DragCoefficient = 0.63f;   // 较高阻力
SniperBullet->Lifetime = 10.0f;          // 长距离
```

## 高级功能

### 粒子效果系统

支持延迟轨迹效果，模拟真实弹道：

```cpp
USTRUCT(BlueprintType)
struct FSKGProjectileParticleData
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGShooterFramework")
    TObjectPtr<UNiagaraSystem> Particle;           // 粒子系统
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGShooterFramework")
    float ParticleSpawnDelayDistance {0.0f};       // 延迟距离（厘米）
    
    operator bool () const
    {
        return Particle != nullptr;
    }
};
```

### 调试系统

内置调试功能便于开发和测试：

```cpp
USTRUCT(BlueprintType)
struct FSKGProjectileDebugData
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGProjectileDebugData")
    bool bDebugPath {false};           // 调试弹道路径
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGProjectileDebugData")
    float LineThickness {1.0f};        // 线条粗细
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGProjectileDebugData")
    bool bDebugImpact {false};         // 调试撞击点
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGProjectileDebugData")
    float ImpactRadius {5.0f};         // 撞击点半径
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGProjectileDebugData")
    float DebugLifetime {10.0f};       // 调试显示时间
};
```

### 物理计算

模块支持复杂的物理计算：

#### 空气阻力计算
```cpp
// 阻力 = 0.5 * 空气密度 * 速度² * 阻力系数 * 截面积
float DragForce = 0.5f * AirDensity * VelocitySquared * DragCoefficient * CrossSectionalArea;
```

#### 重力影响
```cpp
// 重力加速度应用于弹道轨迹
Velocity.Z -= Gravity * DeltaTime;
```

#### 风速影响（可选）
```cpp
// 风速对轻质量投射物的影响
Velocity += WindVelocity * WindEffectiveness * DeltaTime;
```

## 技术实现

### 投射物生命周期

```cpp
// 1. 初始化
void InitializeProjectile(const FTransform& SpawnTransform, USKGPDAProjectile* Data);

// 2. 物理更新
void TickPhysics(float DeltaTime);

// 3. 碰撞检测
void HandleCollision(const FHitResult& HitResult);

// 4. 效果生成
void SpawnImpactEffects(const FHitResult& HitResult);

// 5. 生命周期结束
void EndProjectileLife();
```

### 性能优化

#### 对象池
```cpp
// 使用对象池避免频繁创建销毁
class USKGProjectilePool : public UObject
{
    TArray<USKGProjectile*> AvailableProjectiles;
    TArray<USKGProjectile*> ActiveProjectiles;
};
```

#### LOD系统
```cpp
// 基于距离的LOD处理
void UpdateLOD(float DistanceToPlayer)
{
    if (DistanceToPlayer > LODDistance)
    {
        // 降低更新频率
        SetTickInterval(0.1f);
        
        // 简化物理计算
        bUseSimplifiedPhysics = true;
    }
}
```

### 网络同步

#### 关键数据同步
```cpp
USTRUCT()
struct FSKGProjectileNetworkData
{
    GENERATED_BODY()
    
    UPROPERTY()
    FVector_NetQuantize Location;      // 位置
    
    UPROPERTY()
    FVector_NetQuantizeNormal Velocity; // 速度方向
    
    UPROPERTY()
    uint8 ProjectileType;              // 投射物类型
    
    UPROPERTY()
    float Timestamp;                   // 时间戳
};
```

#### 预测和回滚
```cpp
// 客户端预测
void PredictProjectilePath(float DeltaTime);

// 服务器校正
void CorrectProjectilePath(const FSKGProjectileNetworkData& ServerData);
```

## 集成使用

### 与火器系统集成

```cpp
// 在火器组件中使用
void ASKGFirearm::FireWeapon()
{
    // 获取枪口变换
    FTransform MuzzleTransform = MuzzleComponent->GetMuzzleTransform();
    
    // 获取弹道数据
    USKGPDAProjectile* ProjectileData = GetCurrentAmmo()->ProjectileData;
    
    // 创建投射物
    USKGProjectile* Projectile = ProjectileSubsystem->SpawnProjectile(
        ProjectileData, 
        MuzzleTransform, 
        GetOwner()
    );
    
    // 应用初始速度
    Projectile->SetVelocity(MuzzleTransform.GetRotation().GetForwardVector() * ProjectileData->Velocity);
}
```

### 与附件系统集成

```cpp
// 枪口装置影响弹道
void ASKGMuzzleDevice::ModifyProjectile(USKGPDAProjectile* ProjectileData)
{
    // 消音器可能降低初速
    ProjectileData->Velocity *= 0.95f;
    
    // 制退器可能改变弹道特性
    ProjectileData->DragCoefficient *= 1.1f;
}
```

## 配置最佳实践

### 真实物理参数

```cpp
// 基于真实数据配置
RifleBullet->Velocity = 94000.0f;      // M4A1 初速
RifleBullet->Weight = 62.0f;           // M855 子弹重量
RifleBullet->DragCoefficient = 0.307f; // 流线型弹头

// 计算弹道系数（BC）
float BallisticCoefficient = Weight / 7000.0f / (Caliber * Caliber) / DragCoefficient;
```

### 游戏平衡考虑

```cpp
// 根据游戏类型调整参数
if (GameType == EGameType::Arcade)
{
    // 街机模式：降低物理真实性，提高可玩性
    ProjectileData->DragCoefficient *= 0.5f;  // 减少阻力
    ProjectileData->Lifetime = 3.0f;          // 缩短距离
}
else if (GameType == EGameType::Simulation)
{
    // 模拟模式：追求真实物理
    ProjectileData->DragCoefficient *= 1.0f;  // 真实阻力
    ProjectileData->Lifetime = 10.0f;         // 长距离
}
```

## 性能监控

### 统计信息

```cpp
DECLARE_STATS_GROUP(TEXT("SKGProjectile"), STATGROUP_SKGProjectile, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("Projectile Tick"), STAT_ProjectileTick, STATGROUP_SKGProjectile);
DECLARE_CYCLE_STAT(TEXT("Physics Update"), STAT_PhysicsUpdate, STATGROUP_SKGProjectile);
DECLARE_CYCLE_STAT(TEXT("Collision Detection"), STAT_CollisionDetection, STATGROUP_SKGProjectile);
```

### 调试命令

```cpp
// 控制台命令
skg.projectile.debug_path 1        // 显示弹道路径
skg.projectile.debug_impact 1      // 显示撞击点
skg.projectile.debug_stats 1       // 显示统计信息
skg.projectile.lifetime 10         // 设置调试生命周期
```

## 常见问题

### Q: 投射物穿墙？
A: 检查碰撞通道设置和碰撞响应配置

### Q: 弹道不准确？
A: 验证物理参数设置，检查重力和其他外力影响

### Q: 性能问题？
A: 使用对象池，启用LOD系统，优化更新频率

### Q: 网络同步延迟？
A: 优化网络数据大小，使用预测和回滚机制

## 相关模块

- [SKGMuzzle 模块](SKGMuzzle模块.md) - 枪口装置系统
- [SKGFirearmComponent](SKGShooterFramework模块.md) - 火器核心组件
- [SKGOptic 模块](SKGOptic模块.md) - 光学瞄准镜系统
- [SKGShooterFrameworkCore 模块](SKGShooterFrameworkCore模块.md) - 核心框架功能

## 扩展应用

### 特殊弹药类型

```cpp
// 曳光弹
TracerBullet->ParticleData.Particle = TracerEffect;
TracerBullet->ParticleData.ParticleSpawnDelayDistance = 0.0f;  // 立即显示

// 穿甲弹
APBullet->Velocity = 120000.0f;  // 更高初速
APBullet->Weight = 150.0f;       // 更重弹头

// 霰弹
ShotgunPellet->Velocity = 40000.0f;   // 较低初速
ShotgunPellet->DragCoefficient = 0.8f; // 高阻力
ShotgunPellet->Lifetime = 1.5f;       // 短距离
```

### 环境交互

```cpp
// 风力影响
void ApplyWindEffect(FVector WindVelocity, float WindStrength)
{
    Velocity += WindVelocity * WindStrength * DragCoefficient * DeltaTime;
}

// 温度影响
void ApplyTemperatureEffect(float Temperature)
{
    float TemperatureFactor = 1.0f + (Temperature - 15.0f) * 0.002f;  // 15°C为标准
    Velocity *= TemperatureFactor;
}
```

### 高级物理特性

```cpp
// 马格努斯效应（旋转影响）
void ApplyMagnusEffect(float SpinRate, FVector SpinAxis)
{
    FVector MagnusForce = FVector::CrossProduct(Velocity, SpinAxis) * SpinRate * MagnusCoefficient;
    Velocity += MagnusForce * DeltaTime;
}

// 科里奥利效应（长距离影响）
void ApplyCoriolisEffect(FVector Location, float Latitude)
{
    FVector CoriolisForce = CalculateCoriolisEffect(Velocity, Location, Latitude);
    Velocity += CoriolisForce * DeltaTime;
}
```