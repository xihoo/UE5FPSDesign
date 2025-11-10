# SKGPhysicalMaterialModule 模块文档

## 模块概述

SKGPhysicalMaterialModule 模块提供了增强的物理材质系统，支持基于物理材质的撞击效果、脚步声、粒子效果、贴花等高级功能。该模块扩展了Unreal Engine的标准物理材质系统，添加了游戏特定的物理交互效果，为射击游戏提供真实的材质反馈体验。

## 核心组件

### USKGPhysicalMaterial

**类路径**: `Source/SKGPhysicalMaterialModule/Public/PhysicalMaterial/SKGPhysicalMaterial.h`

这是增强的物理材质类，继承自UPhysicalMaterial并添加了游戏特定的效果系统。

#### 主要功能
- **撞击效果**: 支持多种撞击类型的视觉和声音效果
- **材质标签**: 通过GameplayTag系统标识材质类型
- **粒子效果**: 支持Niagara粒子系统和传统粒子系统
- **贴花效果**: 动态贴花生成，支持大小和生命周期控制
- **声音效果**: 基于物理材质的撞击声音，支持音速延迟
- **效果池**: 粒子系统池化优化，提高性能

#### 核心数据结构

##### FSKGPhysMatSoundSettings
```cpp
USTRUCT(BlueprintType)
struct FSKGPhysMatSoundSettings
{
    GENERATED_BODY()
    
    // 撞击声音
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGShooterFramework")
    TObjectPtr<USoundBase> Sound;
    
    // 音量倍数
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGShooterFramework")
    float VolumeMultiplier {1.0f};
    
    // 音调倍数
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGShooterFramework")
    float PitchMultiplier {1.0f};
    
    operator bool () const
    {
        return Sound != nullptr;
    }
};
```

##### FSKGPhysMatVisualSettings
```cpp
USTRUCT(BlueprintType)
struct FSKGPhysMatVisualSettings
{
    GENERATED_BODY()
    
    // 撞击粒子效果
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGShooterFramework")
    TObjectPtr<UNiagaraSystem> ImpactEffect;
    
    // 撞击贴花材质
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGShooterFramework")
    TObjectPtr<UMaterialInterface> ImpactDecal;
    
    // 池化方法
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGShooterFramework")
    EPSCPoolMethod PoolingMethod {EPSCPoolMethod::AutoRelease};
    
    // 是否使用粒子旋转
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGShooterFramework")
    bool bUseParticlesRotation {false};
    
    // 贴花大小
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGShooterFramework")
    FVector DecalSize {FVector(2.5f, 2.5f, 2.5f)};
    
    // 贴花生命周期
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGShooterFramework")
    float DecalLifeTime {8.0f};
    
    // 贴花淡出屏幕大小
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGShooterFramework")
    float DecalFadeScreenSize {0.001f};
    
    operator bool () const
    {
        return ImpactEffect != nullptr || ImpactDecal != nullptr;
    }
};
```

##### FSKGPhysMatEffect
```cpp
USTRUCT(BlueprintType)
struct FSKGPhysMatEffect
{
    GENERATED_BODY()
    
    // 效果标识标签（如：子弹撞击、脚步声等）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGShooterFramework")
    FGameplayTag Tag;
    
    // 视觉设置
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGShooterFramework")
    FSKGPhysMatVisualSettings ImpactVisualSettings;
    
    // 声音设置
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGShooterFramework")
    FSKGPhysMatSoundSettings ImpactSoundSettings;
    
    operator bool () const
    {
        return Tag.IsValid() && (ImpactVisualSettings || ImpactSoundSettings);
    }
};
```

#### 核心属性

```cpp
// 音速容忍度（低于此值的声音不延迟播放）
UPROPERTY(EditDefaultsOnly, Category = "SKGPhysicalMaterial|Initialize")
float SpeedOfSoundTolerance = 0.2f;

// 游戏标签
UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGPhysicalMaterial|Initialize")
FGameplayTagContainer GameplayTags;

// 效果数组（支持多种撞击类型）
UPROPERTY(EditDefaultsOnly, Category = "SKGPhysicalMaterial|Effects")
TArray<FSKGPhysMatEffect> Effects;
```

#### 主要方法

```cpp
/**
 * 播放指定类型的撞击效果
 * @param HitResult 撞击结果
 * @param bUseSpeedOfSound 是否使用音速延迟
 * @param ImpactEffectTag 效果标签（如子弹撞击、脚步声等）
 */
UFUNCTION(BlueprintCallable, Category = "SKGPhysicalMaterial")
void PlayEffect(const FHitResult& HitResult, const bool bUseSpeedOfSound, const FGameplayTag& ImpactEffectTag);

/**
 * 获取所有效果
 * @return 效果数组
 */
UFUNCTION(BlueprintPure, Category = "SKGPhysicalMaterial")
const TArray<FSKGPhysMatEffect>& GetEffects() const { return Effects; }

/**
 * 获取指定标签的效果
 * @param OUTEffect 输出效果
 * @param EffectTag 效果标签
 * @return 是否找到匹配的效果
 */
UFUNCTION(BlueprintPure, Category = "SKGPhysicalMaterial")
bool GetEffect(FSKGPhysMatEffect& OUTEffect, const FGameplayTag& EffectTag) const;

// 游戏标签接口实现
virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }
```

## 动画通知系统

### UUAnimNotify_SKGPlayFootstepSound

**类路径**: `Source/SKGPhysicalMaterialModule/Public/AnimationNotifies/UAnimNotify_SKGPlayFootstepSound.h`

专门的脚步声动画通知，用于在动画中触发基于物理材质的脚步声。

#### 核心属性

```cpp
// 脚部骨骼名称
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKG|AnimNotify|FootstepSound")
FName FootBoneName {"foot_r"};

// 物理材质效果标签
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKG|AnimNotify|FootstepSound")
FGameplayTag PhysicalMaterialEffectTag;

// 追踪通道
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKG|AnimNotify|FootstepSound")
TEnumAsByte<ECollisionChannel> TraceChannel {ECC_Visibility};
```

#### 主要方法

```cpp
// 设置脚部骨骼名称
UFUNCTION(BlueprintCallable, Category = "SKG|AnimNotify|FootstepSound")
void SetFootBoneName(const FName& BoneName) { FootBoneName = BoneName; }

// 设置物理材质效果标签
UFUNCTION(BlueprintCallable, Category = "SKG|AnimNotify|FootstepSound")
void SetPhysicalMaterialEffectTag(const FGameplayTag& Tag) { PhysicalMaterialEffectTag = Tag; }

// 设置追踪通道
UFUNCTION(BlueprintCallable, Category = "SKG|AnimNotify|FootstepSound")
void SetTraceChannel(TEnumAsByte<ECollisionChannel> Channel) { TraceChannel = Channel; }
```

## 使用方法

### 基础物理材质配置

1. **创建物理材质**: 创建基于USKGPhysicalMaterial的物理材质
2. **配置效果**: 设置撞击效果和脚步声效果
3. **分配标签**: 为材质分配适当的游戏标签
4. **应用到物体**: 将物理材质应用到场景中的物体

### 物理材质效果配置

```cpp
// 创建物理材质
USKGPhysicalMaterial* ConcreteMaterial = NewObject<USKGPhysicalMaterial>();

// 添加游戏标签
ConcreteMaterial->GameplayTags.AddTag(FGameplayTag::RequestGameplayTag("Material.Concrete"));

// 配置子弹撞击效果
FSKGPhysMatEffect BulletImpactEffect;
BulletImpactEffect.Tag = FGameplayTag::RequestGameplayTag("Effect.BulletImpact");

// 配置视觉特效
BulletImpactEffect.ImpactVisualSettings.ImpactEffect = LoadObject<UNiagaraSystem>(...); // 混凝土撞击粒子
BulletImpactEffect.ImpactVisualSettings.ImpactDecal = LoadObject<UMaterialInterface>(...); // 混凝土撞击贴花
BulletImpactEffect.ImpactVisualSettings.DecalSize = FVector(5.0f, 5.0f, 5.0f);
BulletImpactEffect.ImpactVisualSettings.DecalLifeTime = 10.0f;

// 配置声音效果
BulletImpactEffect.ImpactSoundSettings.Sound = LoadObject<USoundBase>(...); // 混凝土撞击声音
BulletImpactEffect.ImpactSoundSettings.VolumeMultiplier = 1.0f;
BulletImpactEffect.ImpactSoundSettings.PitchMultiplier = 1.0f;

// 添加到效果数组
ConcreteMaterial->Effects.Add(BulletImpactEffect);

// 配置脚步声效果
FSKGPhysMatEffect FootstepEffect;
FootstepEffect.Tag = FGameplayTag::RequestGameplayTag("Effect.Footstep");
FootstepEffect.ImpactSoundSettings.Sound = LoadObject<USoundBase>(...); // 混凝土脚步声
FootstepEffect.ImpactVisualSettings.ImpactEffect = LoadObject<UNiagaraSystem>(...); // 脚步声粒子

ConcreteMaterial->Effects.Add(FootstepEffect);
```

### 撞击效果播放

```cpp
// 在撞击处理中播放效果
void HandleImpact(const FHitResult& HitResult, const FGameplayTag& ImpactTag)
{
    if (HitResult.PhysMaterial.IsValid())
    {
        USKGPhysicalMaterial* PhysMat = Cast<USKGPhysicalMaterial>(HitResult.PhysMaterial.Get());
        if (PhysMat)
        {
            // 播放指定类型的撞击效果
            PhysMat->PlayEffect(HitResult, true, ImpactTag);
        }
    }
}
```

### 脚步声系统使用

```cpp
// 在动画蓝图中使用脚步声通知
// 1. 在动画序列中添加UAnimNotify_SKGPlayFootstepSound通知
// 2. 设置适当的物理材质效果标签
// 3. 系统会自动检测地面材质并播放相应声音

// 或者在代码中手动触发
void PlayFootstepSound(USkeletalMeshComponent* MeshComponent, const FName& FootBoneName)
{
    // 从脚部位置向下追踪
    FVector FootLocation = MeshComponent->GetSocketLocation(FootBoneName);
    FVector TraceEnd = FootLocation - FVector(0, 0, 50);
    
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(MeshComponent->GetOwner());
    
    if (MeshComponent->GetWorld()->LineTraceSingleByChannel(HitResult, FootLocation, TraceEnd, ECC_Visibility, QueryParams))
    {
        if (HitResult.PhysMaterial.IsValid())
        {
            USKGPhysicalMaterial* PhysMat = Cast<USKGPhysicalMaterial>(HitResult.PhysMaterial.Get());
            if (PhysMat)
            {
                // 播放脚步声效果
                PhysMat->PlayEffect(HitResult, false, FGameplayTag::RequestGameplayTag("Effect.Footstep"));
            }
        }
    }
}
```

## 高级功能

### 多效果类型支持

```cpp
// 为同一材质配置多种效果类型
void SetupMultiEffectMaterial()
{
    USKGPhysicalMaterial* MultiEffectMaterial = NewObject<USKGPhysicalMaterial>();
    
    // 子弹撞击效果
    FSKGPhysMatEffect BulletImpact;
    BulletImpact.Tag = FGameplayTag::RequestGameplayTag("Effect.BulletImpact");
    // ... 配置子弹撞击效果
    
    // 爆炸撞击效果
    FSKGPhysMatEffect ExplosionImpact;
    ExplosionImpact.Tag = FGameplayTag::RequestGameplayTag("Effect.ExplosionImpact");
    // ... 配置爆炸撞击效果（更大的粒子、更长的贴花寿命）
    
    // 脚步声效果
    FSKGPhysMatEffect Footstep;
    Footstep.Tag = FGameplayTag::RequestGameplayTag("Effect.Footstep");
    // ... 配置脚步声效果
    
    // 载具撞击效果
    FSKGPhysMatEffect VehicleImpact;
    VehicleImpact.Tag = FGameplayTag::RequestGameplayTag("Effect.VehicleImpact");
    // ... 配置载具撞击效果
    
    MultiEffectMaterial->Effects.Add(BulletImpact);
    MultiEffectMaterial->Effects.Add(ExplosionImpact);
    MultiEffectMaterial->Effects.Add(Footstep);
    MultiEffectMaterial->Effects.Add(VehicleImpact);
}
```

### 音速延迟系统

```cpp
void PlayEffect(const FHitResult& HitResult, const bool bUseSpeedOfSound, const FGameplayTag& ImpactEffectTag)
{
    FSKGPhysMatEffect Effect;
    if (GetEffect(Effect, ImpactEffectTag))
    {
        // 播放视觉特效（无延迟）
        if (Effect.ImpactVisualSettings)
        {
            PlayVisualEffect(GetWorld(), HitResult, Effect.ImpactVisualSettings);
        }
        
        // 播放声音效果（可选音速延迟）
        if (Effect.ImpactSoundSettings)
        {
            PlaySoundEffect(GetWorld(), HitResult, bUseSpeedOfSound, SpeedOfSoundTolerance, Effect.ImpactSoundSettings);
        }
    }
}

static void PlaySoundEffect(const UWorld* World, const FHitResult& HitResult, const bool bUseSpeedOfSound, const float SpeedOfSoundTolerance, const FSKGPhysMatSoundSettings& Settings)
{
    if (Settings.Sound)
    {
        float Delay = 0.0f;
        
        if (bUseSpeedOfSound)
        {
            // 计算声音传播时间
            float Distance = FVector::Dist(HitResult.Location, HitResult.TraceStart);
            float SpeedOfSound = 343.0f; // 米/秒
            Delay = Distance / (SpeedOfSound * 100.0f); // 转换为秒
            
            // 如果延迟太小，直接播放
            if (Delay < SpeedOfSoundTolerance)
            {
                Delay = 0.0f;
            }
        }
        
        // 播放声音（带延迟）
        UGameplayStatics::PlaySoundAtLocation(World, Settings.Sound, HitResult.Location, Settings.VolumeMultiplier, Settings.PitchMultiplier, 0.0f, nullptr, nullptr, HitResult.Actor.Get(), false, Delay);
    }
}
```

### 粒子池化优化

```cpp
static void PlayVisualEffect(const UWorld* World, const FHitResult& HitResult, const FSKGPhysMatVisualSettings& Settings)
{
    if (Settings.ImpactEffect)
    {
        // 使用粒子池系统
        FWorldPSCPool& Pool = World->GetPSCPool();
        
        // 获取或创建粒子系统组件
        FPSCPool& PSCPool = Pool.GetPool(Settings.ImpactEffect, Settings.PoolingMethod);
        UNiagaraComponent* NiagaraComponent = PSCPool.GetComponent();
        
        if (NiagaraComponent)
        {
            // 设置粒子系统参数
            NiagaraComponent->SetWorldLocationAndRotation(HitResult.Location, HitResult.ImpactNormal.Rotation());
            
            if (Settings.bUseParticlesRotation)
            {
                NiagaraComponent->SetWorldRotation(HitResult.ImpactNormal.Rotation());
            }
            
            // 激活粒子系统
            NiagaraComponent->Activate(true);
        }
    }
    
    // 处理贴花
    if (Settings.ImpactDecal)
    {
        // 创建动态贴花
        UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
            World, 
            Settings.ImpactDecal, 
            Settings.DecalSize, 
            HitResult.Location, 
            HitResult.ImpactNormal.Rotation(), 
            Settings.DecalLifeTime
        );
        
        if (Decal)
        {
            Decal->SetFadeScreenSize(Settings.DecalFadeScreenSize);
        }
    }
}
```

## 配置示例

### 混凝土材质配置

```cpp
// 混凝土物理材质
USKGPhysicalMaterial* ConcreteMaterial = NewObject<USKGPhysicalMaterial>();
ConcreteMaterial->SpeedOfSoundTolerance = 0.1f;

// 添加标签
ConcreteMaterial->GameplayTags.AddTag(FGameplayTag::RequestGameplayTag("Material.Concrete"));
ConcreteMaterial->GameplayTags.AddTag(FGameplayTag::RequestGameplayTag("Material.Hard"));

// 子弹撞击效果
FSKGPhysMatEffect ConcreteBulletImpact;
ConcreteBulletImpact.Tag = FGameplayTag::RequestGameplayTag("Effect.BulletImpact");
ConcreteBulletImpact.ImpactVisualSettings.ImpactEffect = LoadObject<UNiagaraSystem>(TEXT("/Game/Effects/ConcreteImpact"));
ConcreteBulletImpact.ImpactVisualSettings.ImpactDecal = LoadObject<UMaterialInterface>(TEXT("/Game/Decals/ConcreteBulletHole"));
ConcreteBulletImpact.ImpactVisualSettings.DecalSize = FVector(4.0f, 4.0f, 4.0f);
ConcreteBulletImpact.ImpactVisualSettings.DecalLifeTime = 15.0f;
ConcreteBulletImpact.ImpactSoundSettings.Sound = LoadObject<USoundBase>(TEXT("/Game/Sounds/ConcreteImpact"));
ConcreteBulletImpact.ImpactSoundSettings.VolumeMultiplier = 0.8f;

// 脚步声效果
FSKGPhysMatEffect ConcreteFootstep;
ConcreteFootstep.Tag = FGameplayTag::RequestGameplayTag("Effect.Footstep");
ConcreteFootstep.ImpactSoundSettings.Sound = LoadObject<USoundBase>(TEXT("/Game/Sounds/ConcreteFootstep"));
ConcreteFootstep.ImpactSoundSettings.VolumeMultiplier = 0.6f;
ConcreteFootstep.ImpactVisualSettings.ImpactEffect = LoadObject<UNiagaraSystem>(TEXT("/Game/Effects/DustKick"));

ConcreteMaterial->Effects.Add(ConcreteBulletImpact);
ConcreteMaterial->Effects.Add(ConcreteFootstep);
```

### 金属材质配置

```cpp
// 金属物理材质
USKGPhysicalMaterial* MetalMaterial = NewObject<USKGPhysicalMaterial>();
MetalMaterial->SpeedOfSoundTolerance = 0.05f;  // 金属传声快，容忍度更低

// 添加标签
MetalMaterial->GameplayTags.AddTag(FGameplayTag::RequestGameplayTag("Material.Metal"));
MetalMaterial->GameplayTags.AddTag(FGameplayTag::RequestGameplayTag("Material.Hard"));

// 子弹撞击效果（金属火花）
FSKGPhysMatEffect MetalBulletImpact;
MetalBulletImpact.Tag = FGameplayTag::RequestGameplayTag("Effect.BulletImpact");
MetalBulletImpact.ImpactVisualSettings.ImpactEffect = LoadObject<UNiagaraSystem>(TEXT("/Game/Effects/MetalSparks"));
MetalBulletImpact.ImpactVisualSettings.ImpactDecal = LoadObject<UMaterialInterface>(TEXT("/Game/Decals/MetalBulletHole"));
MetalBulletImpact.ImpactVisualSettings.DecalSize = FVector(2.0f, 2.0f, 2.0f);
MetalBulletImpact.ImpactVisualSettings.DecalLifeTime = 20.0f;
MetalBulletImpact.ImpactSoundSettings.Sound = LoadObject<USoundBase>(TEXT("/Game/Sounds/MetalImpact"));
MetalBulletImpact.ImpactSoundSettings.VolumeMultiplier = 1.2f;
MetalBulletImpact.ImpactSoundSettings.PitchMultiplier = 1.1f;

// 脚步声效果（金属脚步声）
FSKGPhysMatEffect MetalFootstep;
MetalFootstep.Tag = FGameplayTag::RequestGameplayTag("Effect.Footstep");
MetalFootstep.ImpactSoundSettings.Sound = LoadObject<USoundBase>(TEXT("/Game/Sounds/MetalFootstep"));
MetalFootstep.ImpactSoundSettings.VolumeMultiplier = 0.8f;
MetalFootstep.ImpactSoundSettings.PitchMultiplier = 1.2f;

MetalMaterial->Effects.Add(MetalBulletImpact);
MetalMaterial->Effects.Add(MetalFootstep);
```

### 草地材质配置

```cpp
// 草地物理材质
USKGPhysicalMaterial* GrassMaterial = NewObject<USKGPhysicalMaterial>();
GrassMaterial->SpeedOfSoundTolerance = 0.3f;  // 草地传声慢

// 添加标签
GrassMaterial->GameplayTags.AddTag(FGameplayTag::RequestGameplayTag("Material.Grass"));
GrassMaterial->GameplayTags.AddTag(FGameplayTag::RequestGameplayTag("Material.Soft"));

// 子弹撞击效果（草地撞击）
FSKGPhysMatEffect GrassBulletImpact;
GrassBulletImpact.Tag = FGameplayTag::RequestGameplayTag("Effect.BulletImpact");
GrassBulletImpact.ImpactVisualSettings.ImpactEffect = LoadObject<UNiagaraSystem>(TEXT("/Game/Effects/GrassImpact"));
GrassBulletImpact.ImpactVisualSettings.ImpactDecal = nullptr;  // 草地不留弹孔
GrassBulletImpact.ImpactSoundSettings.Sound = LoadObject<USoundBase>(TEXT("/Game/Sounds/GrassImpact"));
GrassBulletImpact.ImpactSoundSettings.VolumeMultiplier = 0.4f;

// 脚步声效果（草地脚步声）
FSKGPhysMatEffect GrassFootstep;
GrassFootstep.Tag = FGameplayTag::RequestGameplayTag("Effect.Footstep");
GrassFootstep.ImpactSoundSettings.Sound = LoadObject<USoundBase>(TEXT("/Game/Sounds/GrassFootstep"));
GrassFootstep.ImpactSoundSettings.VolumeMultiplier = 0.3f;
GrassFootstep.ImpactVisualSettings.ImpactEffect = LoadObject<UNiagaraSystem>(TEXT("/Game/Effects/GrassKick"));

GrassMaterial->Effects.Add(GrassBulletImpact);
GrassMaterial->Effects.Add(GrassFootstep);
```

## 技术细节

### 效果查找算法

```cpp
bool GetEffect(FSKGPhysMatEffect& OUTEffect, const FGameplayTag& EffectTag) const
{
    for (const FSKGPhysMatEffect& Effect : Effects)
    {
        if (Effect.Tag == EffectTag)
        {
            OUTEffect = Effect;
            return true;
        }
    }
    return false;
}
```

### 物理材质检测

```cpp
// 在撞击处理中获取物理材质
void HandleImpact(const FHitResult& HitResult)
{
    if (HitResult.PhysMaterial.IsValid())
    {
        USKGPhysicalMaterial* PhysMat = Cast<USKGPhysicalMaterial>(HitResult.PhysMaterial.Get());
        if (PhysMat)
        {
            // 使用增强的物理材质系统
            PhysMat->PlayEffect(HitResult, true, ImpactTag);
        }
        else
        {
            // 回退到默认处理
            HandleDefaultImpact(HitResult);
        }
    }
}
```

### 动画通知实现

```cpp
void UUAnimNotify_SKGPlayFootstepSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (MeshComp && MeshComp->GetWorld())
    {
        // 从脚部位置向下追踪
        FVector FootLocation = MeshComp->GetSocketLocation(FootBoneName);
        FVector TraceEnd = FootLocation - FVector(0, 0, 50);
        
        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(MeshComp->GetOwner());
        
        if (MeshComp->GetWorld()->LineTraceSingleByChannel(HitResult, FootLocation, TraceEnd, TraceChannel, QueryParams))
        {
            if (HitResult.PhysMaterial.IsValid())
            {
                USKGPhysicalMaterial* PhysMat = Cast<USKGPhysicalMaterial>(HitResult.PhysMaterial.Get());
                if (PhysMat)
                {
                    // 播放脚步声效果
                    PhysMat->PlayEffect(HitResult, false, PhysicalMaterialEffectTag);
                }
            }
        }
    }
}
```

## 性能优化

### 粒子池化

```cpp
// 使用世界粒子系统池
FWorldPSCPool& Pool = World->GetPSCPool();
FPSCPool& PSCPool = Pool.GetPool(EffectSettings.ImpactEffect, EffectSettings.PoolingMethod);

// 获取组件而不是创建新的
UNiagaraComponent* NiagaraComponent = PSCPool.GetComponent();
if (NiagaraComponent)
{
    // 重用现有组件
    NiagaraComponent->SetWorldLocationAndRotation(HitResult.Location, HitResult.ImpactNormal.Rotation());
    NiagaraComponent->Activate(true);
}
```

### 声音延迟优化

```cpp
// 只在需要时计算音速延迟
float CalculateSoundDelay(const FVector& SoundLocation, const FVector& ListenerLocation)
{
    float Distance = FVector::Dist(SoundLocation, ListenerLocation);
    float SpeedOfSound = 343.0f; // m/s
    float Delay = Distance / (SpeedOfSound * 100.0f); // 转换为秒
    
    // 如果延迟太小，直接播放
    if (Delay < SpeedOfSoundTolerance)
    {
        return 0.0f;
    }
    
    return Delay;
}
```

## 常见问题

### Q: 撞击效果不播放？
A: 检查物理材质是否正确设置，确认效果标签匹配，验证撞击结果中的PhysMaterial是否有效

### Q: 脚步声不工作？
A: 检查动画通知是否正确添加，确认脚部骨骼名称正确，验证追踪通道设置

### Q: 粒子效果性能问题？
A: 使用池化系统，合理设置PoolingMethod，避免过多的粒子效果同时播放

### Q: 贴花不显示？
A: 检查贴花材质是否正确，确认贴花大小和生命周期设置合理，验证表面是否支持贴花

### Q: 音速延迟不准确？
A: 调整SpeedOfSoundTolerance值，确保距离计算正确，考虑不同材质的传声速度差异

## 最佳实践

1. **效果分类**: 为不同类型的撞击创建专门的效果标签
2. **性能平衡**: 合理使用效果池化，避免过度复杂的粒子效果
3. **真实感**: 根据材质特性调整声音和视觉效果参数
4. **一致性**: 保持同类型材质的效果风格一致
5. **可扩展性**: 设计易于扩展的效果结构，方便添加新材质类型

## 相关模块

- [SKGProjectile 模块](SKGProjectile模块.md) - 弹道系统
- [SKGMuzzle 模块](SKGMuzzle模块.md) - 枪口装置系统
- [SKGShooterFramework 模块](SKGShooterFramework模块.md) - 火器核心系统

<system-reminder>Whenever you read the file, you should consider whether the user explicitly asked for a summary, analysis, or explanation. If they did, provide it. If not, continue with the task as requested.}</content>