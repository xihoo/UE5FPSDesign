# 动画系统设计文档

## 概述

本文档定义了角色动画系统的架构，包括第一人称和第三人称动画、程序化动画、武器动画以及与SKG框架的集成。动画系统负责提供流畅的角色动作表现。

## 设计原则

1. **程序化与预制结合** - 关键动作使用动画资产，细节使用程序化生成
2. **第一/三人称兼顾** - 同时支持第一人称和第三人称视角
3. **状态机驱动** - 使用Animation Blueprint的状态机
4. **IK系统** - 脚部IK、手部IK、瞄准偏移
5. **与SKG集成** - 利用SKG的程序化动画组件

## 核心架构

### 动画蓝图结构

```
ABP_PlayerCharacter (玩家动画蓝图)
├── Locomotion (移动)
│   ├── Idle/Walk/Run
│   ├── Sprint
│   └── Crouch
├── Weapon (武器)
│   ├── Aim Offset
│   ├── Reload
│   └── Fire
├── Jump/Fall (跳跃/下落)
├── Procedural (程序化)
│   ├── Recoil
│   ├── Sway
│   └── Breathing
└── IK
    ├── Foot IK
    ├── Hand IK
    └── Look At
```

---

## 核心组件

### 1. 移动动画

#### 基础移动状态

```
States:
- Idle: 待机动画
- Walk: 行走循环
- Run: 奔跑循环
- Sprint: 冲刺循环
- Crouch: 蹲伏移动

Transitions:
- Speed < 1: → Idle
- Speed 1-150: → Walk
- Speed 150-400: → Run
- Sprint Input + Speed > 400: → Sprint
- Crouch Input: → Crouch State
```

#### Blend Spaces

```
BS_Locomotion (移动融合空间)
- X轴: 速度 (0-600)
- Y轴: 方向 (-180 to 180)
- 动画: Idle, Walk_Fwd, Walk_Bwd, Walk_Left, Walk_Right, Run_Fwd, etc.

BS_CrouchLocomotion (蹲伏移动)
- X轴: 速度 (0-300)
- Y轴: 方向 (-180 to 180)
```

---

### 2. 武器动画

#### 装备动画

```cpp
// 在动画蓝图中
UFUNCTION(BlueprintCallable, Category = "Animation|Weapon")
void PlayEquipAnimation(UAnimMontage* EquipMontage)
{
    if (EquipMontage && AnimInstance)
    {
        AnimInstance->Montage_Play(EquipMontage);
    }
}
```

#### 射击动画

```
Montage: AM_Fire_Rifle
- Fire: 射击动作
- Additive Layer: 后坐力叠加

Montage: AM_Reload_Rifle
- Remove Magazine
- Insert Magazine  
- Chamber Round
- Sections: Start, Loop, End
```

#### 瞄准偏移 (Aim Offset)

```
AO_Rifle (瞄准偏移)
- X轴: Yaw (-90 to 90)
- Y轴: Pitch (-90 to 90)
- 动画: Aim poses for different angles

在蓝图中应用:
- Input: Control Rotation - Actor Rotation
- Output: Blend with base pose
```

---

### 3. 程序化动画

#### 武器摆动 (Weapon Sway)

```cpp
UCLASS()
class UFPSProceduralAnimComponent : public UActorComponent
{
    GENERATED_BODY()
    
public:
    // 计算武器摆动
    UFUNCTION(BlueprintPure, Category = "ProceduralAnim")
    FTransform CalculateWeaponSway(float DeltaTime);
    
protected:
    UPROPERTY(EditDefaultsOnly, Category = "Sway")
    float SwayAmount = 0.5f;
    
    UPROPERTY(EditDefaultsOnly, Category = "Sway")
    float SwaySpeed = 5.0f;
    
    FVector2D CurrentSway;
    FVector2D TargetSway;
};

FTransform UFPSProceduralAnimComponent::CalculateWeaponSway(float DeltaTime)
{
    // 插值到目标摆动
    CurrentSway = FMath::Vector2DInterpTo(CurrentSway, TargetSway, DeltaTime, SwaySpeed);
    
    // 转换为Transform
    FTransform SwayTransform;
    SwayTransform.SetLocation(FVector(0, CurrentSway.X * SwayAmount, CurrentSway.Y * SwayAmount));
    SwayTransform.SetRotation(FQuat::MakeFromEuler(FVector(CurrentSway.Y, 0, CurrentSway.X)));
    
    return SwayTransform;
}
```

#### 呼吸动画

```cpp
// 在动画蓝图中
float CalculateBreathingOffset()
{
    float Time = GetWorld()->GetTimeSeconds();
    float BreathCycle = FMath::Sin(Time * BreathingSpeed);
    return BreathCycle * BreathingAmount;
}
```

#### 后坐力动画

```cpp
// 应用后坐力到武器位置
FVector ApplyRecoilOffset(FVector BaseLocation, FVector RecoilAmount)
{
    // 使用Spring Interpolation实现后坐力恢复
    FVector TargetLocation = BaseLocation - RecoilAmount;
    CurrentRecoilLocation = FMath::VInterpTo(
        CurrentRecoilLocation,
        TargetLocation,
        DeltaTime,
        RecoilRecoverySpeed
    );
    return CurrentRecoilLocation;
}
```

---

### 4. IK系统

#### 脚部IK (Foot IK)

```cpp
// 在动画蓝图中实现
void UpdateFootIK()
{
    // 左脚IK
    FVector LeftFootLocation;
    FRotator LeftFootRotation;
    TraceFootLocation(LeftFootBone, LeftFootLocation, LeftFootRotation);
    
    // 右脚IK
    FVector RightFootLocation;
    FRotator RightFootRotation;
    TraceFootLocation(RightFootBone, RightFootLocation, RightFootRotation);
    
    // 调整骨盆高度
    float HipOffset = CalculateHipOffset(LeftFootLocation.Z, RightFootLocation.Z);
    
    // 应用IK
    SetIKFootLocation(LeftFootSocket, LeftFootLocation);
    SetIKFootRotation(LeftFootSocket, LeftFootRotation);
    SetIKFootLocation(RightFootSocket, RightFootLocation);
    SetIKFootRotation(RightFootSocket, RightFootRotation);
    SetHipOffset(HipOffset);
}

void TraceFootLocation(FName FootBone, FVector& OutLocation, FRotator& OutRotation)
{
    FVector FootLocation = GetBoneLocation(FootBone);
    FVector TraceStart = FootLocation + FVector(0, 0, IKTraceDistance);
    FVector TraceEnd = FootLocation - FVector(0, 0, IKTraceDistance);
    
    FHitResult HitResult;
    if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility))
    {
        OutLocation = HitResult.Location;
        OutRotation = HitResult.Normal.Rotation();
    }
}
```

#### 手部IK (Hand IK)

```cpp
// 用于左手握持武器的IK
void UpdateHandIK()
{
    if (CurrentWeapon && bIsHoldingWeapon)
    {
        // 获取武器的左手IK目标位置
        FTransform LeftHandTarget = CurrentWeapon->GetLeftHandIKTransform();
        
        // 应用IK
        SetIKTarget(LeftHandSocket, LeftHandTarget);
    }
}
```

#### 视线跟踪 (Look At)

```cpp
// 头部朝向目标
void UpdateLookAt()
{
    if (LookAtTarget)
    {
        FVector TargetLocation = LookAtTarget->GetActorLocation();
        FVector HeadLocation = GetBoneLocation(HeadBone);
        FRotator LookAtRotation = (TargetLocation - HeadLocation).Rotation();
        
        // 限制旋转角度
        LookAtRotation = ClampLookAtRotation(LookAtRotation);
        
        // 应用到头部骨骼
        ApplyBoneRotation(HeadBone, LookAtRotation, 0.5f);
    }
}
```

---

### 5. 上下半身分层

```
Animation Layers:
├── Base Layer (全身)
├── Upper Body (上半身覆盖)
│   ├── Weapon animations
│   ├── Reload
│   └── Aim Offset
└── Additive Layer (叠加层)
    ├── Recoil
    ├── Hit reactions
    └── Breathing
```

**实现方式**：

```cpp
// 在动画蓝图中
- 使用 Layered Blend Per Bone 节点
- Branch Filter: spine_01 (从脊柱开始分离上半身)
- Blend Depth: 1
- Mesh Space Rotation Blend: true
```

---

### 6. 动画通知 (Anim Notifies)

#### 射击相关

```cpp
UCLASS()
class UAN_FireWeapon : public UAnimNotify
{
    GENERATED_BODY()
    
public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override
    {
        if (AFPSPlayerCharacter* Character = Cast<AFPSPlayerCharacter>(MeshComp->GetOwner()))
        {
            // 触发射击逻辑
            Character->OnFireAnimNotify();
        }
    }
};
```

#### 换弹相关

```cpp
// Anim Notify: AN_EjectMagazine
- 在动画中标记弹匣移除时机
- 触发弹匣掉落物理效果

// Anim Notify: AN_InsertMagazine
- 标记新弹匣插入时机
- 播放插入音效

// Anim Notify: AN_ReloadComplete
- 标记换弹完成
- 恢复弹药数量
```

---

### 7. 第一人称专用动画

```
FP_Arms (第一人称手臂)
├── 独立网格和骨骼
├── 更细致的手部动画
├── 相机相对动画
└── 专用的瞄准偏移

注意事项:
- 使用专门的第一人称骨骼网格
- 动画播放速率可能需要调整
- 考虑FOV对动画的影响
```

---

### 8. 动画状态同步

#### 网络复制

```cpp
UCLASS()
class UFPSAnimInstance : public UAnimInstance
{
    GENERATED_BODY()
    
public:
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
    
protected:
    // 复制的动画状态
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    float Speed;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    float Direction;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    bool bIsInAir;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    bool bIsCrouching;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    bool bIsAiming;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    FRotator AimRotation;
};

void UFPSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    
    if (AFPSPlayerCharacter* Character = Cast<AFPSPlayerCharacter>(TryGetPawnOwner()))
    {
        // 更新速度
        FVector Velocity = Character->GetVelocity();
        Speed = Velocity.Size2D();
        
        // 更新方向
        Direction = CalculateDirection(Velocity, Character->GetActorRotation());
        
        // 更新状态
        bIsInAir = Character->GetCharacterMovement()->IsFalling();
        bIsCrouching = Character->bIsCrouched;
        bIsAiming = Character->IsAiming();
        AimRotation = Character->GetControlRotation();
    }
}
```

---

## 动画资产组织

### 文件夹结构

```
Content/
├── Characters/
│   ├── Player/
│   │   ├── Animations/
│   │   │   ├── Locomotion/
│   │   │   ├── Combat/
│   │   │   ├── Interactions/
│   │   │   └── Montages/
│   │   ├── BlendSpaces/
│   │   └── AnimBlueprints/
│   │       ├── ABP_Player_TP.uasset
│   │       └── ABP_Player_FP.uasset
│   └── Enemies/
└── Weapons/
    └── Animations/
        ├── Rifle/
        ├── Pistol/
        └── Shotgun/
```

---

## 性能优化

### LOD设置

```cpp
// 根据距离调整动画更新频率
void SetAnimationLOD(float Distance)
{
    if (Distance < 500.0f)
    {
        SetComponentTickInterval(0.0f); // 每帧更新
    }
    else if (Distance < 1500.0f)
    {
        SetComponentTickInterval(0.05f); // 20fps
    }
    else
    {
        SetComponentTickInterval(0.1f); // 10fps
    }
}
```

### 动画压缩

```
- 使用 Automatic Compression 进行自动压缩
- 对远距离角色使用更激进的压缩
- 移除不必要的动画曲线
```

---

## 相关文档

- [基类设计文档](../基类/基类.md)
- [射击框架文档](../射击框架/射击框架.md)
- [输入系统文档](../Input/Input.md)
