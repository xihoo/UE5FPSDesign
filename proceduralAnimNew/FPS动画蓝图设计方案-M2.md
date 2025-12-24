# FPS游戏程序化动画蓝图设计方案

## 一、概述

本文档基于对某3A级FPS游戏动画蓝图的深度分析，设计一套模块化、分层级、高度程序化的FPS角色动画系统。该方案采用**C++核心驱动 + 蓝图表现**的混合架构，旨在实现以下目标：
- 高度的程序化动画（Procedural Animation）
- 流畅的状态过渡（无硬切）
- 良好的扩展性（支持多种武器类型）
- 性能优化（逻辑下沉到C++）

---

## 二、系统架构总览

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           核心设计理念                                    │
│  1. C++负责"大脑"：数据计算、状态判断、程序化数学                          │
│  2. 蓝图负责"骨架"：连接节点、混合动画                                    │
│  3. 虚拟骨骼为核心：使用 VB_Hand_Gun 作为程序化动画载体                    │
└─────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────┐
│                           数据流向图                                      │
│                                                                         │
│  [Character] ──► [AnimInstance(C++)] ──► [AnimGraph(Blueprint)] ──► [渲染]  │
│      │               │              │                                   │
│  速度/输入/状态      计算核心         节点连接                            │
│                     ├─Sway         ├─Transform Bone                      │
│                     ├─Recoil       ├─Blend Poses                         │
│                     ├─Lag          ├─Layered Blend                       │
│                     └─Bob          └─IK Solver                           │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 三、基础数据结构定义

### 3.1 枚举定义

```cpp
// 武器类型枚举
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Unarmed     UMETA(DisplayName = "徒手"),
    Rifle       UMETA(DisplayName = "步枪"),
    Pistol      UMETA(DisplayName = "手枪"),
    Melee       UMETA(DisplayName = "近战武器")
};

// 角色姿态枚举
UENUM(BlueprintType)
enum class EPostureState : uint8
{
    Standing    UMETA(DisplayName = "站立"),
    Crouching   UMETA(DisplayName = "蹲伏"),
    Prone       UMETA(DisplayName = "趴下")
};

// 移动状态枚举
UENUM(BlueprintType)
enum class EMovementState : uint8
{
    Idle        UMETA(DisplayName = "待机"),
    Walking     UMETA(DisplayName = "行走"),
    Running     UMETA(DisplayName = "奔跑"),
    Sprinting   UMETA(DisplayName = "冲刺")
};
```

### 3.2 程序化动画数据结构

```cpp
// 程序化动画计算结果结构体
USTRUCT(BlueprintType)
struct FProceduralAnimData
{
    GENERATED_BODY()

    // === 武器骨骼变换 ===
    UPROPERTY(BlueprintReadOnly)
    FTransform WeaponBoneTransform;      // VB_Hand_Gun 的最终变换

    UPROPERTY(BlueprintReadOnly)
    FVector WeaponOffset;                // 通用偏移（用于冲刺、下蹲等）

    UPROPERTY(BlueprintReadOnly)
    FRotator WeaponRotation;             // 通用旋转（用于Sway、瞄准等）

    // === 左手IK ===
    UPROPERTY(BlueprintReadOnly)
    FVector LeftHandIKLocation;          // 左手IK目标位置

    UPROPERTY(BlueprintReadOnly)
    FRotator LeftHandIKRotation;         // 左手IK目标旋转

    // === 脊柱瞄准偏移 ===
    UPROPERTY(BlueprintReadOnly)
    float SpinePitchOffset;              // 脊柱俯仰角

    UPROPERTY(BlueprintReadOnly)
    float HeadPitchOffset;               // 头部俯仰角

    // === 状态权重 ===
    UPROPERTY(BlueprintReadOnly)
    float AimingAlpha;                   // 瞄准过渡 (0-1)

    UPROPERTY(BlueprintReadOnly)
    float SprintAlpha;                   // 冲刺过渡 (0-1)

    UPROPERTY(BlueprintReadOnly)
    float CrouchAlpha;                   // 蹲伏过渡 (0-1)
};
```

---

## 四、自定义AnimInstance设计

### 4.1 头文件结构

```cpp
UCLASS()
class UFPSCharacterAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    // === 生命周期 ===
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaTime) override;

protected:
    // === 引用 ===
    UPROPERTY(BlueprintReadOnly, Category = "References")
    AMyFPSCharacter* Character;

    UPROPERTY(BlueprintReadOnly, Category = "References")
    UCharacterMovementComponent* MovementComponent;

    // === 基础移动数据 (对应原蓝图 Event Graph 图1) ===
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    float GroundSpeed;

    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    float LocomotionDirection;           // -180 到 180

    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    bool bShouldMove;

    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    bool bIsFalling;

    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    bool bIsCrouching;

    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    bool bIsSprinting;

    // === 战斗状态 ===
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsAiming;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    float AimingAlpha;                   // 0-1 平滑过渡

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    EWeaponType CurrentWeaponType;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsMelee;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsAkimbo;                      // 双持模式

    // === 武器IK配置 ===
    UPROPERTY(BlueprintReadOnly, Category = "IK")
    FVector LeftHandGripLocation;        // 握把Socket位置

    UPROPERTY(BlueprintReadOnly, Category = "IK")
    FName LeftHandGripBone;              // 握持骨骼名称

    // === 原地转身 (对应原蓝图 Event Graph 图2-3) ===
    UPROPERTY(BlueprintReadOnly, Category = "TurnInPlace")
    float RootYawOffset;                 // 根骨骼旋转偏移

    UPROPERTY(BlueprintReadOnly, Category = "TurnInPlace")
    float TurnAngle;                     // 当前转身角度

    UPROPERTY(BlueprintReadOnly, Category = "TurnInPlace")
    bool bIsTurning;                     // 是否正在转身

    // === 程序化动画结果 (驱动AnimGraph) ===
    UPROPERTY(BlueprintReadOnly, Category = "Procedural")
    FProceduralAnimData ProcAnimData;

private:
    // === 内部计算函数 ===
    void UpdateLocomotionData(float DeltaTime);
    void UpdateWeaponData();
    void CalculateTurnInPlace(float DeltaTime);
    void CalculateProceduralAnimations(float DeltaTime);

    // === 程序化算法核心 ===
    FTransform CalculateSwayTransform(float DeltaTime);
    FTransform CalculateRecoilTransform(float DeltaTime);
    FTransform CalculateSprintTransform(float DeltaTime);
    FTransform CalculateLeanTransform(float DeltaTime);
    FTransform CalculateWalkBobTransform(float DeltaTime);

    // === 状态管理 ===
    void HandleAimingTransition(float DeltaTime);
    void HandleSprintTransition(float DeltaTime);
    void HandleCrouchTransition(float DeltaTime);

    // === 内部状态变量 ===
    float CurrentSwayYaw;
    float CurrentSwayPitch;
    float CurrentSwayRoll;

    FVector RecoilVelocity;
    FVector RecoilOffset;

    float SprintAlphaInternal;           // 内部冲刺插值
    float AimingAlphaInternal;           // 内部瞄准插值

    FRotator LastCameraRotation;
    FVector2D CameraInputVelocity;
};
```

### 4.2 核心逻辑实现

```cpp
void UFPSCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if (!Character || !MovementComponent) return;

    // 1. 更新基础移动数据
    UpdateLocomotionData(DeltaTime);

    // 2. 更新武器数据
    UpdateWeaponData();

    // 3. 处理状态过渡
    HandleAimingTransition(DeltaTime);
    HandleSprintTransition(DeltaTime);
    HandleCrouchTransition(DeltaTime);

    // 4. 原地转身逻辑
    CalculateTurnInPlace(DeltaTime);

    // 5. 程序化动画计算 (核心)
    CalculateProceduralAnimations(DeltaTime);
}

void UFPSCharacterAnimInstance::UpdateLocomotionData(float DeltaTime)
{
    // 获取速度向量
    FVector Velocity = Character->GetVelocity();
    GroundSpeed = Velocity.Size2D();

    // 判断是否应该移动 (防抖处理)
    bShouldMove = (GroundSpeed > 3.0f) &&
                  (MovementComponent->GetCurrentAcceleration().Size() > 0.0f);

    // 空中状态
    bIsFalling = MovementComponent->IsFalling();

    // 蹲伏状态
    bIsCrouching = Character->bIsCrouched;

    // 冲刺状态 (需要从Character获取)
    bIsSprinting = Character->IsSprinting();

    // 计算移动方向 (-180 到 180)
    FRotator ActorRot = Character->GetActorRotation();
    FRotator VelRot = Velocity.ToOrientationRotator();
    LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(Velocity, ActorRot);
}
```

---

## 五、程序化动画算法详解

### 5.1 武器摆动 (Weapon Sway)

```cpp
FTransform UFPSCharacterAnimInstance::CalculateSwayTransform(float DeltaTime)
{
    // 获取鼠标输入速度
    float MouseX = CameraInputVelocity.X;
    float MouseY = CameraInputVelocity.Y;

    // 目标旋转偏移 (鼠标移动时枪口滞后)
    float TargetSwayYaw = MouseX * SwayYawMultiplier;      // 左右摆动
    float TargetSwayPitch = MouseY * SwayPitchMultiplier;  // 上下摆动
    float TargetSwayRoll = MouseX * SwayRollMultiplier;    // 倾斜效果

    // 平滑插值 (FInterp)
    CurrentSwayYaw = FMath::FInterpTo(CurrentSwayYaw, TargetSwayYaw,
                                       DeltaTime, SwayInterpSpeed);
    CurrentSwayPitch = FMath::FInterpTo(CurrentSwayPitch, TargetSwayPitch,
                                         DeltaTime, SwayInterpSpeed);
    CurrentSwayRoll = FMath::FInterpTo(CurrentSwayRoll, TargetSwayRoll,
                                        DeltaTime, SwayInterpSpeed);

    // 构建变换
    FTransform SwayTransform;
    SwayTransform.SetLocation(FVector(0.0f, CurrentSwayYaw * SwayLocationMultiplier,
                                       CurrentSwayPitch * SwayLocationMultiplier));
    SwayTransform.SetRotation(FRotator(CurrentSwayPitch, CurrentSwayYaw, CurrentSwayRoll).Quaternion());

    return SwayTransform;
}
```

### 5.2 程序化后坐力 (Procedural Recoil)

```cpp
FTransform UFPSCharacterAnimInstance::CalculateRecoilTransform(float DeltaTime)
{
    // 后坐力衰减 (弹簧回归)
    RecoilVelocity = FMath::VInterpTo(RecoilVelocity, FVector::ZeroVector,
                                       DeltaTime, RecoilRecoverySpeed);

    // 应用衰减后的后坐力偏移
    FVector RecoilLocation = RecoilVelocity * DeltaTime;
    FRotator RecoilRotation = FRotator(RecoilVelocity.Z * 0.5f, RecoilVelocity.Y, 0.0f);

    FTransform RecoilTransform;
    RecoilTransform.SetLocation(RecoilLocation);
    RecoilTransform.SetRotation(RecoilRotation.Quaternion());

    return RecoilTransform;
}

// 开火时调用 (由武器系统触发)
void UFPSCharacterAnimInstance::AddRecoil(float PitchAmount, float YawAmount)
{
    // 随机化增加自然感
    float RandomPitch = PitchAmount * FMath::RandRange(0.8f, 1.2f);
    float RandomYaw = YawAmount * FMath::RandRange(-0.5f, 0.5f);

    // 累加后坐力冲量
    RecoilVelocity += FVector(-RandomPitch * RecoilPitchMultiplier,
                               RandomYaw * RecoilYawMultiplier,
                               RandomPitch);
}

// 简单后坐力衰减 (指数衰减)
void UFPSCharacterAnimInstance::TickRecoil(float DeltaTime)
{
    float Damping = FMath::Exp(-RecoilRecoverySpeed * DeltaTime);
    RecoilVelocity *= Damping;
}
```

### 5.3 冲刺偏移 (Sprint Transform)

```cpp
FTransform UFPSCharacterAnimInstance::CalculateSprintTransform(float DeltaTime)
{
    if (!bIsSprinting || bIsAiming)
    {
        // 冲刺结束时平滑恢复
        SprintAlphaInternal = FMath::FInterpTo(SprintAlphaInternal, 0.0f,
                                                 DeltaTime, SprintBlendSpeed);
    }
    else
    {
        // 冲刺时平滑过渡
        SprintAlphaInternal = FMath::FInterpTo(SprintAlphaInternal, 1.0f,
                                                 DeltaTime, SprintBlendSpeed);
    }

    // 冲刺偏移参数 (从Character获取或配置)
    FVector SprintOffsetLocation = FVector(SprintForwardOffset,
                                            SprintSideOffset,
                                            SprintDownOffset) * SprintAlphaInternal;

    FRotator SprintOffsetRotation = FRotator(SprintPitchOffset, 0.0f, 0.0f) * SprintAlphaInternal;

    FTransform SprintTransform;
    SprintTransform.SetLocation(SprintOffsetLocation);
    SprintTransform.SetRotation(SprintOffsetRotation.Quaternion());

    return SprintTransform;
}
```

### 5.4 步进震动 (Walk Bob)

```cpp
FTransform UFPSCharacterAnimInstance::CalculateWalkBobTransform(float DeltaTime)
{
    if (!bShouldMove || GroundSpeed < 10.0f)
    {
        return FTransform::Identity;
    }

    // 基于正弦波的步进震动
    float TimeSeconds = GetWorld()->GetTimeSeconds();
    float BobSpeed = (GroundSpeed > 300.0f) ? RunBobSpeed : WalkBobSpeed;
    float BobAmount = (GroundSpeed > 300.0f) ? RunBobAmount : WalkBobAmount;

    float BobX = FMath::Sin(TimeSeconds * BobSpeed) * BobAmount;
    float BobY = FMath::Sin(TimeSeconds * BobSpeed * 2.0f) * BobAmount * 0.5f;
    float BobZ = FMath::Abs(FMath::Sin(TimeSeconds * BobSpeed)) * BobAmount;

    FTransform BobTransform;
    BobTransform.SetLocation(FVector(BobX, BobY, -BobZ)); // Z轴向下(枪口下垂)

    return BobTransform;
}
```

### 5.5 程序化动画合成

```cpp
void UFPSCharacterAnimInstance::CalculateProceduralAnimations(float DeltaTime)
{
    FTransform FinalTransform = FTransform::Identity;

    // 1. 基础瞄准对齐 (Aim Correction)
    if (bIsAiming)
    {
        // 瞄准时枪口归位到屏幕中心
        FTransform AimTransform = CalculateAimCorrectionTransform();
        FinalTransform = AimTransform * FinalTransform;
    }

    // 2. 叠加Sway (镜头延迟)
    FTransform SwayTransform = CalculateSwayTransform(DeltaTime);
    FinalTransform = SwayTransform * FinalTransform;

    // 3. 叠加Recoil (后坐力)
    FTransform RecoilTransform = CalculateRecoilTransform(DeltaTime);
    FinalTransform = RecoilTransform * FinalTransform;

    // 4. 叠加Sprint (冲刺偏移)
    FTransform SprintTransform = CalculateSprintTransform(DeltaTime);
    FinalTransform = SprintTransform * FinalTransform;

    // 5. 叠加Walk Bob (步进震动)
    FTransform BobTransform = CalculateWalkBobTransform(DeltaTime);
    FinalTransform = BobTransform * FinalTransform;

    // 6. 叠加Lean (侧身)
    FTransform LeanTransform = CalculateLeanTransform(DeltaTime);
    FinalTransform = LeanTransform * FinalTransform;

    // 7. 下蹲倾斜 (Crouch Tilt)
    if (bIsCrouching)
    {
        FTransform CrouchTilt = CalculateCrouchTiltTransform();
        FinalTransform = CrouchTilt * FinalTransform;
    }

    // 输出结果
    ProcAnimData.WeaponBoneTransform = FinalTransform;
    ProcAnimData.SprintAlpha = SprintAlphaInternal;
    ProcAnimData.AimingAlpha = AimingAlphaInternal;
}
```

---

## 六、原地转身系统 (Turn In Place)

```cpp
void UFPSCharacterAnimInstance::CalculateTurnInPlace(float DeltaTime)
{
    // 获取控制器旋转和角色旋转的差值
    FRotator ViewRotation = Character->GetControlRotation();
    FRotator ActorRotation = Character->GetActorRotation();
    FRotator DeltaRotation = (ViewRotation - ActorRotation).GetNormalized();

    float DeltaYaw = DeltaRotation.Yaw;

    // 移动时重置转身状态
    if (bShouldMove)
    {
        bIsTurning = false;
        RootYawOffset = 0.0f;
        TurnAngle = 0.0f;
        return;
    }

    // 阈值检测 (超过90度触发转身)
    const float TurnThreshold = 90.0f;

    if (!bIsTurning)
    {
        if (FMath::Abs(DeltaYaw) > TurnThreshold)
        {
            // 触发转身动画
            bIsTurning = true;
            TurnAngle = DeltaYaw > 0 ? 90.0f : -90.0f;

            // 播放转身蒙太奇 (通过接口调用)
            PlayTurnInPlaceMontage(TurnAngle, bIsCrouching);
        }
    }
    else
    {
        // 转身进行中，平滑插值RootYawOffset
        float TargetOffset = FMath::Clamp(DeltaYaw, -90.0f, 90.0f);
        RootYawOffset = FMath::FInterpTo(RootYawOffset, TargetOffset,
                                          DeltaTime, TurnInterpSpeed);

        // 检查转身是否完成 (DeltaYaw接近0)
        if (FMath::Abs(DeltaYaw) < 5.0f)
        {
            bIsTurning = false;
            RootYawOffset = 0.0f;
        }
    }
}

// 播放转身蒙太奇 (C++调用AnimMontage)
void UFPSCharacterAnimInstance::PlayTurnInPlaceMontage(float TurnAngle, bool bIsCrouching)
{
    UAnimMontage* MontageToPlay = nullptr;

    // 根据角度和状态选择蒙太奇
    if (FMath::IsNearlyZero(TurnAngle - 90.0f, 1.0f))
    {
        MontageToPlay = bIsCrouching ? CrouchTurnRightMontage : StandTurnRightMontage;
    }
    else if (FMath::IsNearlyZero(TurnAngle + 90.0f, 1.0f))
    {
        MontageToPlay = bIsCrouching ? CrouchTurnLeftMontage : StandTurnLeftMontage;
    }

    if (MontageToPlay)
    {
        // 设置蒙太奇曲线值用于同步
        float MontageLength = Montage_Play(MontageToPlay, 1.0f,
                                            EMontagePlayReturnType::MontageLength,
                                            0.0f, true);

        // 设置曲线值
        SetMorphTarget(TEXT("TurnAngle"), TurnAngle);
    }
}
```

---

## 七、AnimGraph蓝图设计

### 7.1 整体结构

```
┌─────────────────────────────────────────────────────────────────────────┐
│                          AnimGraph 结构层级                               │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  [Locomotion State Machine]                                             │
│       │                                                                │
│       ▼                                                                │
│  [Motion Warping]                                                       │
│       │                                                                │
│       ▼                                                                │
│  [Slot 'FullBody']  (全身蒙太奇：翻滚/死亡)                              │
│       │                                                                │
│       ▼                                                                │
│  [Control Rig (Foot IK)]                                                │
│       │                                                                │
│       ▼                                                                │
│  [Layered Blend Per Bone (Spine_01)]  ──────┐                           │
│       │                                      │                           │
│       ▼                                      ▼                           │
│  [Upper Body Pose Layer]              [Weapon Pose Layer]               │
│       │                                      │                           │
│       │                                      ▼                           │
│       │                              [Procedural Layer]                 │
│       │                                      │                           │
│       │                                      ▼                           │
│       │                              [ABP_FP_ArmsProcedural]            │
│       │                                      │                           │
│       └──────────────────────────────────────┘                           │
│                           │                                              │
│                           ▼                                              │
│                   [Final Pose Output]                                    │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 7.2 Locomotion状态机设计

```plaintext
┌─────────────────────────────────────────────┐
│            Locomotion State Machine          │
├─────────────────────────────────────────────┤
│                                             │
│  ┌─────────┐                                │
│  │  Idle   │◄────────────┐                  │
│  └────┬────┘             │                  │
│       │ Standing         │ Walking          │
│       ▼                  ▼                  │
│  ┌─────────┐        ┌─────────┐             │
│  │ Walking │───────►│ Running │             │
│  └────┬────┘        └────┬────┘             │
│       │                  │ Sprinting         │
│       ▼                  ▼                  │
│  ┌─────────┐        ┌─────────┐             │
│  │ Sprint  │───────►│  Land   │───────►Idle │
│  └─────────┘        └─────────┘             │
│       │                                    │
│       │ Any State                          │
│       ▼                                    │
│  ┌─────────┐                               │
│  │ Falling │                               │
│  └─────────┘                               │
│       │                                    │
│       ▼                                    │
│  ┌─────────┐                               │
│  │ Jump    │───────►Falling──────►Land     │
│  └─────────┘                               │
│                                             │
└─────────────────────────────────────────────┘
```

### 7.3 AnimGraph节点连接详细说明

#### Layer 1: Locomotion (下半身)
```
[BlendSpace Player]
├── Input: Direction (-180 to 180)
├── Input: GroundSpeed
└── Output: Locomotion Pose
    │
    ▼
[Orientation Warping]
├── LocomotionAngle: Direction
├── Alpha: 1.0
└── Output: Oriented Pose
    │
    ▼
[Stride Warping]
├── LocomotionSpeed: GroundSpeed
├── Alpha: Clamp(GroundSpeed / 100, 0, 1)
└── Output: Strided Pose
    │
    ▼
[Rotate Root Bone]
├── Yaw: RootYawOffset (来自C++)
├── Multiplier: bIsCrouching ? 0.6 : 1.0
└── Output: Final Locomotion (Cached)
```

#### Layer 2: Upper Body (上半身武器)
```
[Sequence Evaluator (Weapon Poses)]
├── Anim_Arms_Pistol_Idle (Time=0)
├── Anim_Arms_Rifle_Pose (Time=0)
├── Anim_Arms_Knife_Pose (Time=0)
└── Output: Weapon Pose Enum
    │
    ▼
[Blend Poses by Enum (EWeaponType)]
├── Rifle → Anim_Arms_Rifle_Pose
├── Pistol → Anim_Arms_Pistol_Idle
├── Melee → Anim_Arms_Knife_Pose
└── Output: Selected Weapon Pose
    │
    ▼
[Slot 'UpperBody']
├── 用于播放上半身蒙太奇 (换弹、检视)
└── Output: Masked Weapon Pose
```

#### Layer 3: Procedural Layer (程序化修正)
```
[Local To Component]
    │
    ▼
[Transform (Modify) Bone: VB_Hand_Gun]
├── Translation: ProcAnimData.WeaponBoneTransform.Location
├── Rotation: ProcAnimData.WeaponBoneTransform.Rotation
├── Scale: (1,1,1)
└── Alpha: 1.0
    │
    ├──► [Free Aim] (自由瞄准偏移)
    ├──► [Recoil] (后坐力 - 已在C++计算)
    ├──► [Sway] (武器摆动 - 已在C++计算)
    ├──► [Sprint Offset] (冲刺偏移 - 已在C++计算)
    ├──► [Walk Bob] (步进震动 - 已在C++计算)
    └──► [Wall Offset] (墙壁避让 - 射线检测计算)
    │
    ▼
[Component To Local]
```

#### Layer 4: Spine & Head (脊柱瞄准)
```
[Transform (Modify) Bone: spine_05]
├── Rotation.Pitch: ProcAnimData.SpinePitchOffset * 0.3
├── Alpha: 0.5
└── Space: Component Space

[Transform (Modify) Bone: spine_04]
├── Rotation.Pitch: ProcAnimData.SpinePitchOffset * 0.3
├── Alpha: 0.5

[Transform (Modify) Bone: neck_02]
├── Rotation.Pitch: ProcAnimData.HeadPitchOffset * 0.5
├── Alpha: 0.5

[Transform (Modify) Bone: head]
├── Rotation: (0, ProcAnimData.SpinePitchOffset, 0)
├── Alpha: 0.5
```

#### Layer 5: IK Layer (手部反向动力学)
```
[Copy Bone]
├── Source: VB_Hand_Gun
├── Target: IK_Hand_Gun
└── Output: IK Target Transform
    │
    ▼
[Two Bone IK: hand_r (右手)]
├── Effector: IK_Hand_Gun
├── Joint Target: (自动计算或指定Socket)
└── Result: Right Hand follows weapon
    │
    ▼
[Two Bone IK: hand_l (左手)]
├── Effector: LeftHandIKLocation (来自C++)
├── Alpha: bIsMelee ? 0 : 1  (近战时关闭左手IK)
└── Result: Left Hand follows grip position
```

---

## 八、关键实现细节

### 8.1 虚拟骨骼配置

在角色骨骼网格体中需要添加以下虚拟骨骼：

```
Virtual Bones:
├── VB_Hand_Gun          (武器根骨骼，用于程序化偏移)
├── IK_Hand_Gun          (IK目标骨骼)
├── IK_Hand_L            (左手IK目标)
└── IK_Hand_R            (右手IK目标)

Sockets (在VB_Hand_Gun上):
├── Socket: GripPoint    (右手握把位置)
└── Socket: MagPoint     (左手托弹匣位置)
```

### 8.2 武器配置数据结构

```cpp
// 武器动画配置 (可放在DataAsset中)
UCLASS()
class UWeaponAnimConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "IK")
    FVector LeftHandGripOffset;          // 左手握把相对位置

    UPROPERTY(EditAnywhere, Category = "IK")
    FRotator LeftHandGripRotation;       // 左手握把相对旋转

    UPROPERTY(EditAnywhere, Category = "Recoil")
    float RecoilPitchAmount;             // 后坐力 Pitch

    UPROPERTY(EditAnywhere, Category = "Recoil")
    float RecoilYawAmount;               // 后坐力 Yaw

    UPROPERTY(EditAnywhere, Category = "Recoil")
    float RecoilRecoverySpeed;           // 后坐力恢复速度

    UPROPERTY(EditAnywhere, Category = "Sway")
    float SwayMultiplier;                // 摆动强度

    UPROPERTY(EditAnywhere, Category = "Sprint")
    FVector SprintOffset;                // 冲刺时的枪口偏移
};
```

### 8.3 接口设计 (解耦)

```cpp
// 动画接口 - 供Character实现
UINTERFACE(MinimalAPI)
class UFPSAnimInterface : public UInterface
{
    GENERATED_BODY()
};

class IFPSAnimInterface
{
    GENERATED_IINTERFACE_BODY()

public:
    // 获取当前装备的武器类型
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation")
    EWeaponType GetEquippedWeaponType();

    // 获取武器动画配置
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Animation")
    UWeaponAnimConfig* GetWeaponAnimConfig();

    // 获取左手IK位置
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IK")
    FVector GetLeftHandGripLocation();

    // 是否处于近战状态
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
    bool IsInMeleeState();

    // 是否为双持模式
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
    bool IsAkimboMode();
};
```

---

## 九、蒙太奇插槽设计

```plaintext
AnimMontage Slots:
├── Slot: FullBody
│   └── 用途: 全身动画 (翻滚、死亡、拾取)
│
├── Slot: UpperBody
│   └── 用途: 上半身动画 (换弹、装填、检视)
│       ├── AnimMontage: Reload_Hip
│       ├── AnimMontage: Reload_ADS
│       ├── AnimMontage: Inspect
│       └── AnimMontage: MeleeAttack
│
├── Slot: Slide
│   └── 用途: 转身动画 (Turn In Place)
│       ├── AnimMontage: Turn_Right_90
│       ├── AnimMontage: Turn_Left_90
│       ├── AnimMontage: Crouch_Turn_Right
│       └── AnimMontage: Crouch_Turn_Left
│
└── Slot: ArmsWeapon
    └── 用途: 手臂特殊动画 (冲刺近战、职业技能)
```

---

## 十、扩展性设计

### 10.1 新增武器类型

只需实现 `UWeaponAnimConfig` DataAsset：

```
配置步骤:
1. 创建新的 UWeaponAnimConfig
2. 调整 LeftHandGripOffset (匹配模型)
3. 设置 Recoil 参数 (射击手感)
4. 配置 SprintOffset (冲刺姿态)
5. 在武器类中实现 IFPSAnimInterface
```

### 10.2 新增姿态类型

```cpp
// 扩展 EWeaponType
enum class EWeaponType : uint8
{
    Unarmed,
    Rifle,
    Pistol,
    Shotgun,      // 新增：霰弹枪
    SMG,          // 新增：冲锋枪
    LMG,          // 新增：轻机枪
    Sniper,       // 新增：狙击枪
    Melee,
    Grenade       // 新增：投掷物
};

// 在 AnimGraph 中添加对应的 Blend Pose
[Blend Poses by Enum]
├── Rifle → Rifle_Pose
├── Shotgun → Shotgun_Pose  // 新增
├── SMG → SMG_Pose          // 新增
└── ...
```

### 10.3 自定义程序化效果

```cpp
// 在 UFPSCharacterAnimInstance 中添加新的计算函数
void CalculateCustomEffect(float DeltaTime)
{
    // 示例：体力系统影响的开火晃动
    if (Character->IsExhausted())
    {
        RecoilMultiplier = 2.0f;  // 疲劳时后坐力更大
        SwayMultiplier = 1.5f;    // 疲劳时瞄准更晃
    }

    // 示例：受击反馈
    if (bIsHit)
    {
        HitRecoilOffset = FVector(0, FMath::RandRange(-5.0f, 5.0f), -10.0f);
    }
}
```

---

## 十一、性能优化建议

1. **减少Transform Bone节点**
   - 合并多个连续的 Transform Bone 操作为单个操作（在C++中完成计算）

2. **使用Cached Pose**
   ```cpp
   // 缓存常用的姿态计算结果
   UPROPERTY(BlueprintReadOnly, Category = "Cached")
   FPoseLink CachedLocomotionPose;

   UPROPERTY(BlueprintReadOnly, Category = "Cached")
   FPoseLink CachedUpperBodyPose;
   ```

3. **IK优化**
   - 仅在必要时启用IK (开镜时禁用Foot IK)
   - 使用两骨骼IK代替全IK

4. **减少蓝图虚拟机调用**
   - 将循环内的计算移到C++
   - 使用 `BlueprintCallable` 标记必要接口

---

## 十二、测试清单

- [ ] 站立/行走/奔跑/冲刺切换平滑
- [ ] 蹲伏状态切换正常
- [ ] 瞄准(ADS)与腰射(Hip Fire)过渡平滑
- [ ] 换弹动画不打断瞄准状态
- [ ] 跑步时开火无穿模
- [ ] 冲刺时武器偏移正确
- [ ] 跳跃和落地时武器跟随正确
- [ ] 侧身探头时脊柱弯曲自然
- [ ] 原地转身时脚步动画正确
- [ ] 不同武器的握把位置正确
- [ ] 滑步问题已解决 (Stride Warping)
- [ ] 墙壁避让功能正常

---

## 十三、参考资源

基于本文档实现的动画蓝图参考了以下3A级FPS游戏的最佳实践：
- 《使命召唤》系列 (程序化后坐力、IK系统)
- 《战地》系列 (Motion Warping、Locomotion混合)
- 《光环》系列 (脊柱瞄准、状态过渡)
- 《叛乱：沙漠风暴》 (自由瞄准、死区系统)

---

## 修订历史

| 版本 | 日期 | 描述 |
|------|------|------|
| 1.0 | 2024-12-24 | 初始设计方案 |
| 1.1 | 2024-12-24 | 新增曲线配置系统设计 |

---

## 十四、程序化动画曲线配置系统

### 14.1 概述

为了提供更灵活的程序化动画控制，我们设计了一套基于**曲线（Curves）**的配置系统。该系统允许：

1. **美术师可视化调整**：通过编辑器曲线直观调整参数
2. **动态时间轴控制**：支持基于时间的动态变化
3. **平滑过渡**：使用曲线插值实现自然的节奏变化
4. **武器差异化**：每种武器可以配置独立的曲线数据

### 14.2 曲线类型定义

```cpp
// 曲线类型枚举
UENUM(BlueprintType)
enum class EAnimCurveType : uint8
{
    None                UMETA(DisplayName = "无"),
    SwayIntensity       UMETA(DisplayName = "瞄准摆动强度"),
    SwaySpeed           UMETA(DisplayName = "瞄准摆动速度"),
    RecoilPitch         UMETA(DisplayName = "后坐力Pitch曲线"),
    RecoilYaw           UMETA(DisplayName = "后坐力Yaw曲线"),
    RecoilRecovery      UMETA(DisplayName = "后坐力恢复曲线"),
    WalkBobAmount       UMETA(DisplayName = "步进震动幅度"),
    WalkBobSpeed        UMETA(DisplayName = "步进震动速度"),
    SprintOffset        UMETA(DisplayName = "冲刺偏移曲线"),
    AimDownSpeed        UMETA(DisplayName = "开镜速度曲线"),
    LeanAmount          UMETA(DisplayName = "侧身幅度曲线"),
    WeaponSway_X        UMETA(DisplayName = "武器X轴摆动"),
    WeaponSway_Y        UMETA(DisplayName = "武器Y轴摆动"),
    WeaponSway_Z        UMETA(DisplayName = "武器Z轴摆动"),
    CustomProcedural    UMETA(DisplayName = "自定义程序化")
};

// 曲线容器配置
USTRUCT(BlueprintType)
struct FAnimCurveConfig
{
    GENERATED_BODY()

    // 曲线名称 (用于调试和查找)
    UPROPERTY(EditAnywhere, Category = "Basic")
    FName CurveName;

    // 曲线类型
    UPROPERTY(EditAnywhere, Category = "Basic")
    EAnimCurveType CurveType;

    // 曲线数据 (运行时使用)
    UPROPERTY(BlueprintReadOnly, Category = "Data")
    UCurveFloat* CurveFloat;

    // 曲线数据 (Transform类型，用于复杂的空间偏移)
    UPROPERTY(BlueprintReadOnly, Category = "Data")
    UCurveVector* CurveVector;

    // 曲线数据 (Rotator类型，用于复杂的旋转偏移)
    UPROPERTY(BlueprintReadOnly, Category = "Data")
    UCurveRotator* CurveRotator;

    // 曲线时间范围
    UPROPERTY(EditAnywhere, Category = "Settings")
    float TimeStart;

    UPROPERTY(EditAnywhere, Category = "Settings")
    float TimeEnd;

    // 曲线值范围 (用于归一化)
    UPROPERTY(EditAnywhere, Category = "Settings")
    FVector2D ValueRange;

    // 是否启用
    UPROPERTY(EditAnywhere, Category = "Settings")
    bool bEnabled;

    // 权重乘数 (可以在运行时调整)
    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    float WeightMultiplier;
};
```

### 14.3 武器曲线配置Asset

```cpp
// 武器动画曲线配置Asset
UCLASS(BlueprintType)
class UWeaponAnimCurveConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    // === 瞄准摆动曲线 (Aiming Sway) ===
    // 控制瞄准时武器随视角移动的滞后程度
    UPROPERTY(EditAnywhere, Category = "Sway")
    FAnimCurveConfig AimingSwayYawCurve;

    UPROPERTY(EditAnywhere, Category = "Sway")
    FAnimCurveConfig AimingSwayPitchCurve;

    // === Hip Fire 摆动曲线 ===
    // 控制腰射时的武器摆动
    UPROPERTY(EditAnywhere, Category = "Sway")
    FAnimCurveConfig HipFireSwayYawCurve;

    UPROPERTY(EditAnywhere, Category = "Sway")
    FAnimCurveConfig HipFireSwayPitchCurve;

    // === 后坐力曲线 (Recoil) ===
    // 控制开火时的后坐力模式
    UPROPERTY(EditAnywhere, Category = "Recoil")
    FAnimCurveConfig RecoilPitchCurve;   // 垂直后坐力

    UPROPERTY(EditAnywhere, Category = "Recoil")
    FAnimCurveConfig RecoilYawCurve;     // 水平后坐力

    UPROPERTY(EditAnywhere, Category = "Recoil")
    FAnimCurveConfig RecoilRollCurve;    // 滚转后坐力

    UPROPERTY(EditAnywhere, Category = "Recoil")
    FAnimCurveConfig RecoilKickCurve;    // 枪口后坐 (Z轴位移)

    UPROPERTY(EditAnywhere, Category = "Recoil")
    FAnimCurveConfig RecoilRecoveryCurve; // 后坐力恢复速度

    // === 步进震动曲线 (Walk Bob) ===
    UPROPERTY(EditAnywhere, Category = "WalkBob")
    FAnimCurveConfig WalkBobAmountCurve; // 行走震动幅度

    UPROPERTY(EditAnywhere, Category = "WalkBob")
    FAnimCurveConfig WalkBobSpeedCurve;  // 行走震动速度

    UPROPERTY(EditAnywhere, Category = "WalkBob")
    FAnimCurveConfig RunBobAmountCurve;  // 奔跑震动幅度

    UPROPERTY(EditAnywhere, Category = "WalkBob")
    FAnimCurveConfig RunBobSpeedCurve;   // 奔跑震动速度

    // === 冲刺偏移曲线 (Sprint) ===
    UPROPERTY(EditAnywhere, Category = "Sprint")
    FAnimCurveConfig SprintForwardCurve; // 冲刺时枪口前后偏移

    UPROPERTY(EditAnywhere, Category = "Sprint")
    FAnimCurveConfig SprintSideCurve;    // 冲刺时枪口左右偏移

    UPROPERTY(EditAnywhere, Category = "Sprint")
    FAnimCurveConfig SprintDownCurve;    // 冲刺时枪口下垂偏移

    UPROPERTY(EditAnywhere, Category = "Sprint")
    FAnimCurveConfig SprintPitchCurve;   // 冲刺时枪口俯仰

    // === 瞄准过渡曲线 (ADS) ===
    UPROPERTY(EditAnywhere, Category = "Aiming")
    FAnimCurveConfig AimDownPositionCurve; // 开镜时的位置偏移

    UPROPERTY(EditAnywhere, Category = "Aiming")
    FAnimCurveConfig AimDownRotationCurve; // 开镜时的旋转偏移

    UPROPERTY(EditAnywhere, Category = "Aiming")
    FAnimCurveConfig AimAlphaCurve;        // 瞄准过渡Alpha曲线

    // === 侧身探头曲线 (Lean) ===
    UPROPERTY(EditAnywhere, Category = "Lean")
    FAnimCurveConfig LeanLeftCurve;        // 左探头幅度

    UPROPERTY(EditAnywhere, Category = "Lean")
    FAnimCurveConfig LeanRightCurve;       // 右探头幅度

    // === 下蹲倾斜曲线 (Crouch Tilt) ===
    UPROPERTY(EditAnywhere, Category = "Crouch")
    FAnimCurveConfig CrouchTiltCurve;      // 蹲下时枪口内收

    // === 特殊效果曲线 (Procedural Effects) ===
    UPROPERTY(EditAnywhere, Category = "Procedural")
    TArray<FAnimCurveConfig> CustomCurves; // 自定义程序化曲线

    // === 武器特定参数 ===
    UPROPERTY(EditAnywhere, Category = "Weapon")
    float WeaponWeight;                    // 武器重量 (影响所有程序化效果)

    UPROPERTY(EditAnywhere, Category = "Weapon")
    float SwayMultiplierGlobal;            // 全局摆动乘数

    // === 获取曲线值的便捷函数 ===
    UFUNCTION(BlueprintCallable, Category = "Curve")
    float GetCurveValue(EAnimCurveType CurveType, float Time) const;

    UFUNCTION(BlueprintCallable, Category = "Curve")
    FVector GetCurveVectorValue(EAnimCurveType CurveType, float Time) const;

    UFUNCTION(BlueprintCallable, Category = "Curve")
    FRotator GetCurveRotatorValue(EAnimCurveType CurveType, float Time) const;
};
```

### 14.4 曲线管理器

```cpp
// 曲线管理器 - 负责加载和缓存曲线
UCLASS()
class UCurveManager : public UObject
{
    GENERATED_BODY()

public:
    static UCurveManager& Get();

    // 加载武器曲线配置
    void LoadWeaponCurves(EWeaponType WeaponType);

    // 获取当前武器的曲线配置
    UWeaponAnimCurveConfig* GetCurrentCurveConfig() const;

    // 更新当前武器类型
    void SetCurrentWeaponType(EWeaponType NewType);

    // 根据曲线类型获取曲线值 (用于AnimInstance调用)
    float GetCurveFloat(EAnimCurveType CurveType, float Time);

    // 缓存所有曲线 (减少查找开销)
    void CacheAllCurves();

private:
    UPROPERTY()
    TMap<EWeaponType, UWeaponAnimCurveConfig*> WeaponCurveConfigs;

    EWeaponType CurrentWeaponType;
};
```

### 14.5 AnimInstance中的曲线应用

```cpp
// 在AnimInstance中扩展曲线支持
UCLASS()
class UFPSCharacterAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    // === 新增：曲线相关属性 ===
    UPROPERTY(BlueprintReadOnly, Category = "Curves")
    UWeaponAnimCurveConfig* CurrentCurveConfig;

    UPROPERTY(BlueprintReadOnly, Category = "Curves")
    float CurveTime;                       // 当前曲线时间 (0-1 或自定义)

    // === 获取曲线值 ===
    UFUNCTION(BlueprintCallable, Category = "Curves")
    float GetSwayYawFromCurve(float Time) const;

    UFUNCTION(BlueprintCallable, Category = "Curves")
    float GetSwayPitchFromCurve(float Time) const;

    UFUNCTION(BlueprintCallable, Category = "Curves")
    float GetRecoilPitchFromCurve(float Time) const;

    UFUNCTION(BlueprintCallable, Category = "Curves")
    float GetRecoilYawFromCurve(float Time) const;

    UFUNCTION(BlueprintCallable, Category = "Curves")
    float GetWalkBobAmountFromCurve(float Time) const;

    UFUNCTION(BlueprintCallable, Category = "Curves")
    FVector GetSprintOffsetFromCurve(float Time) const;

protected:
    // 更新曲线时间
    void UpdateCurveTime(float DeltaTime);
};
```

### 14.6 使用曲线的程序化动画实现

```cpp
// 使用曲线的武器摆动计算
FTransform UFPSCharacterAnimInstance::CalculateSwayTransform(float DeltaTime)
{
    if (!CurrentCurveConfig) return FTransform::Identity;

    // 获取曲线时间 (使用脉冲时间或累加时间)
    float CurrentTime = GetCurveAccumulatedTime();

    // 从曲线获取摆动参数
    float SwayYawMultiplier = CurrentCurveConfig->AimingSwayYawCurve.bEnabled
        ? CurrentCurveConfig->GetCurveValue(EAnimCurveType::SwayIntensity, CurrentTime)
        : DefaultSwayMultiplier;

    float SwayPitchMultiplier = CurrentCurveConfig->AimingSwayPitchCurve.bEnabled
        ? CurrentCurveConfig->GetCurveValue(EAnimCurveType::SwayIntensity, CurrentTime)
        : DefaultSwayMultiplier;

    float SwayRollMultiplier = CurrentCurveConfig->HipFireSwayYawCurve.bEnabled
        ? CurrentCurveConfig->GetCurveValue(EAnimCurveType::WeaponSway_Z, CurrentTime)
        : DefaultSwayRollMultiplier;

    // 鼠标输入
    float MouseX = CameraInputVelocity.X;
    float MouseY = CameraInputVelocity.Y;

    // 从曲线获取速度参数
    float SwayInterpSpeed = CurrentCurveConfig->AimingSwayPitchCurve.bEnabled
        ? CurrentCurveConfig->GetCurveValue(EAnimCurveType::SwaySpeed, CurrentTime)
        : DefaultSwaySpeed;

    // 计算目标旋转
    float TargetSwayYaw = MouseX * SwayYawMultiplier;
    float TargetSwayPitch = MouseY * SwayPitchMultiplier;
    float TargetSwayRoll = MouseX * SwayRollMultiplier;

    // 平滑插值
    CurrentSwayYaw = FMath::FInterpTo(CurrentSwayYaw, TargetSwayYaw, DeltaTime, SwayInterpSpeed);
    CurrentSwayPitch = FMath::FInterpTo(CurrentSwayPitch, TargetSwayPitch, DeltaTime, SwayInterpSpeed);
    CurrentSwayRoll = FMath::FInterpTo(CurrentSwayRoll, TargetSwayRoll, DeltaTime, SwayInterpSpeed);

    // 构建变换
    FTransform SwayTransform;
    SwayTransform.SetLocation(FVector(0.0f, CurrentSwayYaw * SwayLocationMultiplier,
                                       CurrentSwayPitch * SwayLocationMultiplier));
    SwayTransform.SetRotation(FRotator(CurrentSwayPitch, CurrentSwayYaw, CurrentSwayRoll).Quaternion());

    return SwayTransform;
}

// 使用曲线的后坐力计算
FTransform UFPSCharacterAnimInstance::CalculateRecoilTransform(float DeltaTime)
{
    if (!CurrentCurveConfig) return FTransform::Identity;

    // 获取当前后坐力时间 (从开火开始计时)
    float RecoilTime = CurrentRecoilTime;

    // 从曲线获取后坐力参数
    float RecoilPitch = CurrentCurveConfig->RecoilPitchCurve.bEnabled
        ? CurrentCurveConfig->GetCurveValue(EAnimCurveType::RecoilPitch, RecoilTime)
        : DefaultRecoilPitch;

    float RecoilYaw = CurrentCurveConfig->RecoilYawCurve.bEnabled
        ? CurrentCurveConfig->GetCurveValue(EAnimCurveType::RecoilYaw, RecoilTime)
        : DefaultRecoilYaw;

    float RecoilRoll = CurrentCurveConfig->RecoilRollCurve.bEnabled
        ? CurrentCurveConfig->GetCurveValue(EAnimCurveType::WeaponSway_Z, RecoilTime)
        : 0.0f;

    float RecoilKick = CurrentCurveConfig->RecoilKickCurve.bEnabled
        ? CurrentCurveConfig->GetCurveValue(EAnimCurveType::RecoilPitch, RecoilTime)
        : DefaultRecoilKick;

    // 从曲线获取恢复速度
    float RecoverySpeed = CurrentCurveConfig->RecoilRecoveryCurve.bEnabled
        ? CurrentCurveConfig->GetCurveValue(EAnimCurveType::RecoilRecovery, RecoilTime)
        : DefaultRecoverySpeed;

    // 应用恢复衰减
    RecoilVelocity = FMath::VInterpTo(RecoilVelocity, FVector::ZeroVector, DeltaTime, RecoverySpeed);

    // 构建后坐力变换
    FTransform RecoilTransform;
    FVector RecoilLocation = FVector(0.0f, RecoilVelocity.Y * 0.5f, -RecoilKick * RecoilVelocity.Z * 0.1f);
    FRotator RecoilRotation = FRotator(RecoilPitch * RecoilVelocity.Z, RecoilYaw * RecoilVelocity.Y, RecoilRoll);

    RecoilTransform.SetLocation(RecoilLocation);
    RecoilTransform.SetRotation(RecoilRotation.Quaternion());

    return RecoilTransform;
}

// 使用曲线的步进震动计算
FTransform UFPSCharacterAnimInstance::CalculateWalkBobTransform(float DeltaTime)
{
    if (!bShouldMove || GroundSpeed < 10.0f) return FTransform::Identity;

    if (!CurrentCurveConfig) return FTransform::Identity;

    float TimeSeconds = GetWorld()->GetTimeSeconds();

    // 从曲线获取参数
    bool bIsRunning = GroundSpeed > 300.0f;

    float BobAmount = bIsRunning
        ? CurrentCurveConfig->GetCurveValue(EAnimCurveType::WalkBobAmount, TimeSeconds)
        : CurrentCurveConfig->GetCurveValue(EAnimCurveType::WalkBobAmount, TimeSeconds);

    float BobSpeed = bIsRunning
        ? CurrentCurveConfig->GetCurveValue(EAnimCurveType::WalkBobSpeed, TimeSeconds)
        : CurrentCurveConfig->GetCurveValue(EAnimCurveType::RunBobSpeed, TimeSeconds);

    // 应用曲线值的缩放
    BobAmount *= CurrentCurveConfig->WeaponWeight * 0.01f;

    // 正弦波计算
    float BobX = FMath::Sin(TimeSeconds * BobSpeed) * BobAmount;
    float BobY = FMath::Sin(TimeSeconds * BobSpeed * 2.0f) * BobAmount * 0.5f;
    float BobZ = FMath::Abs(FMath::Sin(TimeSeconds * BobSpeed)) * BobAmount;

    FTransform BobTransform;
    BobTransform.SetLocation(FVector(BobX, BobY, -BobZ));

    return BobTransform;
}
```

### 14.7 曲线编辑器配置说明

#### 14.7.1 瞄准摆动曲线配置

```
曲线名称: AimingSway_Yaw
类型: UCurveFloat
X轴: 鼠标移动量 (0 = 居中, -1 = 左, 1 = 右)
Y轴: 摆动幅度乘数 (0 = 无摆动, 1 = 标准摆动)

配置示例:
- (0.0, 0.0)      - 鼠标居中时无偏移
- (0.5, 0.3)      - 鼠标向右一半时，轻微右摆
- (1.0, 1.0)      - 鼠标到边缘时，正常摆动
- (-0.5, -0.3)    - 鼠标向左一半时，轻微左摆
- (-1.0, -1.0)    - 鼠标到边缘时，正常左摆
```

#### 14.7.2 后坐力曲线配置

```
曲线名称: Recoil_Pitch
类型: UCurveFloat
X轴: 时间 (0 = 开火瞬间, 1 = 后坐力结束)
Y轴: Pitch偏移量 (正值为向上，负值为向下)

配置示例 (突击步枪):
- (0.0, 1.0)      - 开火瞬间，枪口上跳100%
- (0.2, 0.8)      - 0.2秒后，上跳80%
- (0.5, 0.3)      - 0.5秒后，上跳30%
- (1.0, 0.0)      - 1秒后，完全恢复

配置示例 (狙击枪):
- (0.0, 2.5)      - 开火瞬间，大幅上跳250%
- (0.5, 1.5)      - 缓慢恢复
- (1.5, 0.0)      - 1.5秒才完全恢复 (更慢的后坐力)
```

#### 14.7.3 步进震动曲线配置

```
曲线名称: WalkBob_Amount
类型: UCurveFloat
X轴: 步进周期 (0 = 脚跟着地, 0.5 = 脚抬起, 1.0 = 下一个脚跟着地)
Y轴: Z轴偏移 (负值 = 枪口下沉)

配置示例:
- (0.0, -0.5)     - 脚跟着地，枪口下沉
- (0.25, 0.0)     - 中间位置，枪口回正
- (0.5, 0.3)      - 脚抬起，枪口轻微上扬
- (0.75, 0.0)     - 下降中，枪口回正
- (1.0, -0.5)     - 下一个脚跟着地，再次下沉
```

#### 14.7.4 冲刺偏移曲线配置

```
曲线名称: SprintForward_Offset
类型: UCurveFloat
X轴: 冲刺进度 (0 = 开始冲刺, 1 = 冲刺结束/持续)
Y轴: 位置偏移

配置示例:
- (0.0, 0.0)      - 冲刺开始，位置正常
- (0.2, -5.0)     - 快速过渡到枪口下垂
- (0.5, -8.0)     - 稳定在最深位置
- (1.0, -8.0)     - 持续冲刺保持
```

### 14.8 曲线在AnimGraph中的使用

#### 14.8.1 蓝图节点配置

```
┌─────────────────────────────────────────────────────────────────┐
│                    AnimGraph中的曲线应用                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  [Curve Float: RecoilPitch]                                     │
│       │                                                        │
│       ├──► [Multiply] (× CurrentRecoilAlpha)                   │
│       │        │                                                │
│       │        └──► [Transform Bone: VB_Hand_Gun]              │
│       │                 Rotation.Pitch                         │
│       │                                                        │
│  [Curve Float: RecoilYaw]                                       │
│       │                                                        │
│       ├──► [Multiply] (× CurrentRecoilAlpha)                   │
│       │        │                                                │
│       │        └──► [Transform Bone: VB_Hand_Gun]              │
│       │                 Rotation.Yaw                           │
│       │                                                        │
│  [Curve Vector: SprintOffset]                                   │
│       │                                                        │
│       ├──► [Multiply] (× SprintAlpha)                          │
│       │        │                                                │
│       │        └──► [Transform Bone: VB_Hand_Gun]              │
│       │                 Translation                            │
│       │                                                        │
│  [Curve Float: WalkBob]                                         │
│       │                                                        │
│       ├──► [Multiply] (× GroundSpeed / MaxSpeed)               │
│       │        │                                                │
│       │        └──► [Transform Bone: VB_Hand_Gun]              │
│       │                 Translation.Z                          │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

#### 14.8.2 曲线与蒙太奇结合

```
┌─────────────────────────────────────────────────────────────────┐
│                 曲线驱动的蒙太奇混合                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  [AnimMontage: Reload]                                          │
│       │                                                        │
│       ├──► [GetCurveValue: ReloadProgress] (从蒙太奇曲线)       │
│       │        │                                                │
│       │        └──► [Apply Additive: ReloadAdditive]           │
│       │                 Alpha = ReloadProgress                  │
│       │                                                        │
│  [Curve Float: WeaponSettle]                                    │
│       │                                                        │
│       └──► [Apply Additive: IdlePose]                          │
│                 Alpha = WeaponSettle                            │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 14.9 动态曲线更新

```cpp
// 动态更新曲线参数 (运行时修改)
void UFPSCharacterAnimInstance::UpdateCurveParameters()
{
    if (!CurrentCurveConfig) return;

    // 根据游戏状态动态调整曲线权重
    // 例如：体力下降时，增加武器晃动
    float FatigueFactor = Character->GetFatigueRatio(); // 0-1

    if (FatigueFactor > 0.0f)
    {
        // 增加摆动曲线的强度
        CurrentCurveConfig->AimingSwayYawCurve.WeightMultiplier = 1.0f + FatigueFactor * 0.5f;
        CurrentCurveConfig->AimingSwayPitchCurve.WeightMultiplier = 1.0f + FatigueFactor * 0.5f;

        // 减慢恢复曲线 (后坐力恢复更慢)
        CurrentCurveConfig->RecoilRecoveryCurve.WeightMultiplier = 1.0f - FatigueFactor * 0.3f;
    }

    // 根据瞄准状态切换曲线集
    if (bIsAiming)
    {
        // 瞄准时使用更平滑的曲线
        CurrentCurveConfig->AimingSwayYawCurve.WeightMultiplier *= 0.7f;
        CurrentCurveConfig->AimingSwayPitchCurve.WeightMultiplier *= 0.7f;
    }

    // 根据移动速度调整步进震动
    float SpeedRatio = FMath::Clamp(GroundSpeed / 600.0f, 0.0f, 1.0f);
    CurrentCurveConfig->WalkBobAmountCurve.WeightMultiplier = 1.0f + SpeedRatio * 0.5f;
    CurrentCurveConfig->WalkBobSpeedCurve.WeightMultiplier = 1.0f + SpeedRatio * 0.3f;
}
```

### 14.10 曲线配置最佳实践

#### 14.10.1 美术师配置指南

1. **命名规范**
   ```
   命名规则: [Effect]_[Component]_[Axis]
   示例:
   - Recoil_Pitch_Y    (后坐力_Pitch轴)
   - Sway_Yaw_X        (摆动_Yaw轴)
   - WalkBob_Z         (步进_Z轴)
   - Sprint_Down       (冲刺_下沉)
   ```

2. **曲线时间轴**
   ```
   标准时间范围:
   - 瞬时效果 (Recoil): 0-1秒
   - 循环效果 (Bob): 0-1个周期 (归一化)
   - 过渡效果 (ADS): 0-0.5秒
   - 持续效果 (Sprint): 0-1 (循环)
   ```

3. **值范围建议**
   ```
   Rotation Curves (度):
   - Pitch: -45 到 45
   - Yaw: -30 到 30
   - Roll: -15 到 15

   Location Curves (单位):
   - X (前后): -10 到 10
   - Y (左右): -5 到 5
   - Z (上下): -20 到 5
   ```

#### 14.10.2 性能优化

```cpp
// 1. 曲线缓存
UPROPERTY(Transient)
TMap<FName, UCurveFloat*> CachedCurves;

// 缓存所有曲线
void UWeaponAnimCurveConfig::CacheCurves()
{
    CachedCurves.Empty();

    if (AimingSwayYawCurve.CurveFloat) CachedCurves.Add(TEXT("SwayYaw"), AimingSwayYawCurve.CurveFloat);
    if (AimingSwayPitchCurve.CurveFloat) CachedCurves.Add(TEXT("SwayPitch"), AimingSwayPitchCurve.CurveFloat);
    if (RecoilPitchCurve.CurveFloat) CachedCurves.Add(TEXT("RecoilPitch"), RecoilPitchCurve.CurveFloat);
    // ... 继续缓存其他曲线
}

// 使用缓存
float UWeaponAnimCurveConfig::GetCachedCurveValue(FName CurveName, float Time)
{
    if (UCurveFloat** FoundCurve = CachedCurves.Find(CurveName))
    {
        return (*FoundCurve)->GetFloatValue(Time);
    }
    return 0.0f;
}

// 2. 批量更新优化
// 只在曲线数据变化时重新缓存
void UWeaponAnimCurveConfig::PostEditChangeProperty(FProperty* Property)
{
    Super::PostEditChangeProperty(Property);

    // 检测是否有曲线属性被修改
    if (Property && Property->IsA<UObjectProperty>())
    {
        CacheCurves();
    }
}
```

#### 14.10.3 调试工具

```cpp
// 曲线调试绘制
void UFPSCharacterAnimInstance::DrawCurveDebug()
{
#if ENABLE_DRAW_DEBUG
    if (!CurrentCurveConfig || !bDrawDebug) return;

    // 绘制当前曲线值
    FVector DebugLocation = Character->GetActorLocation() + FVector(0, 0, 100);

    // 绘制瞄准摆动
    float SwayYaw = GetSwayYawFromCurve(CurveTime);
    DrawDebugString(GetWorld(), DebugLocation + FVector(0, 0, 20),
                    FString::Printf(TEXT("SwayYaw: %.2f"), SwayYaw),
                    nullptr, FColor::Yellow, 0.0f);

    // 绘制后坐力
    float RecoilPitch = GetRecoilPitchFromCurve(CurrentRecoilTime);
    DrawDebugString(GetWorld(), DebugLocation + FVector(0, 0, 40),
                    FString::Printf(TEXT("RecoilPitch: %.2f"), RecoilPitch),
                    nullptr, FColor::Red, 0.0f);

    // 绘制曲线时间
    DrawDebugString(GetWorld(), DebugLocation + FVector(0, 0, 60),
                    FString::Printf(TEXT("CurveTime: %.2f"), CurveTime),
                    nullptr, FColor::Green, 0.0f);
#endif
}
```

### 14.11 常见曲线配置示例

#### 突击步枪 (AR)
```
Recoil_Pitch:    (0, 15) → (0.3, 8) → (0.8, 2) → (1.5, 0)
Recoil_Yaw:      (0, 0) → (0.1, 2) → (0.3, -1) → (0.6, 0)
Recoil_Recovery: (0, 5.0)  // 恢复速度
WalkBob_Amount:  (0, -3) → (0.5, 2) → (1, -3)
WalkBob_Speed:   6.28  // 2π (1Hz)
Sprint_Offset:   (0, 0) → (0.3, -8) → (1, -8)
```

#### 冲锋枪 (SMG)
```
Recoil_Pitch:    (0, 8) → (0.2, 5) → (0.5, 1) → (1.0, 0)
Recoil_Yaw:      (0, 0) → (0.05, 3) → (0.2, -2) → (0.4, 0)
Recoil_Recovery: (0, 8.0)  // 恢复更快
WalkBob_Amount:  (0, -2) → (0.5, 1) → (1, -2)
WalkBob_Speed:   9.42  // 1.5Hz 更快
```

#### 狙击枪 (Sniper)
```
Recoil_Pitch:    (0, 35) → (0.5, 20) → (1.0, 5) → (2.0, 0)
Recoil_Yaw:      (0, 0) → (0.1, 1) → (0.3, -1) → (0.5, 0)
Recoil_Recovery: (0, 2.0)  // 恢复很慢
WalkBob_Amount:  (0, -4) → (0.5, 2) → (1, -4)  // 更大幅度
WalkBob_Speed:   4.71  // 0.75Hz 更慢
```

#### 霰弹枪 (Shotgun)
```
Recoil_Pitch:    (0, 45) → (0.4, 25) → (1.0, 5) → (2.5, 0)
Recoil_Yaw:      (0, 0) → (0.05, 4) → (0.2, -3) → (0.5, 0)
Recoil_Recovery: (0, 1.5)  // 非常慢
Recoil_Kick:     (0, -15) → (0.5, -5) → (1.0, 0)  // 明显后坐
```

---

## 十五、曲线配置工具类

### 15.1 曲线生成工具

```cpp
// 工具类：程序化生成常用曲线
UCLASS()
class UCurveGenerationUtil : public UObject
{
    GENERATED_BODY()

public:
    // 生成线性渐变曲线
    UFUNCTION(BlueprintCallable, Category = "CurveGen")
    static UCurveFloat* CreateLinearCurve(float StartValue, float EndValue, int32 Points = 10);

    // 生成指数衰减曲线 (后坐力恢复)
    UFUNCTION(BlueprintCallable, Category = "CurveGen")
    static UCurveFloat* CreateExponentialDecayCurve(float StartValue, float EndValue, float HalfLife, float Duration);

    // 生成正弦波曲线 (步进震动)
    UFUNCTION(BlueprintCallable, Category = "CurveGen")
    static UCurveFloat* CreateSineWaveCurve(float Amplitude, float Frequency, float Phase = 0.0f);

    // 生成S形曲线 (平滑过渡)
    UFUNCTION(BlueprintCallable, Category = "CurveGen")
    static UCurveFloat* CreateSCurve(float StartValue, float EndValue, float Midpoint = 0.5f);

    // 生成脉冲曲线 (开火后坐力)
    UFUNCTION(BlueprintCallable, Category = "CurveGen")
    static UCurveFloat* CreateRecoilCurve(float PeakValue, float PeakTime, float Duration);

    // 生成反向曲线 (冲刺恢复)
    UFUNCTION(BlueprintCallable, Category = "CurveGen")
    static UCurveFloat* CreateInverseCurve(float MaxValue, float Duration);
};
```

### 15.2 曲线混合工具

```cpp
// 曲线混合工具
UCLASS()
class UCurveBlendingUtil : public UObject
{
    GENERATED_BODY()

public:
    // 混合两条曲线
    UFUNCTION(BlueprintCallable, Category = "CurveBlend")
    static UCurveFloat* BlendCurves(UCurveFloat* CurveA, UCurveFloat* CurveB, float Alpha, int32 Resolution = 100);

    // 缩放曲线
    UFUNCTION(BlueprintCallable, Category = "CurveBlend")
    static UCurveFloat* ScaleCurve(UCurveFloat* SourceCurve, float Scale, float Offset = 0.0f);

    // 时间缩放曲线
    UFUNCTION(BlueprintCallable, Category = "CurveBlend")
    static UCurveFloat* TimeScaleCurve(UCurveFloat* SourceCurve, float TimeScale, float TimeOffset = 0.0f);

    // 反转曲线
    UFUNCTION(BlueprintCallable, Category = "CurveBlend")
    static UCurveFloat* InvertCurve(UCurveFloat* SourceCurve);

    // 取绝对值
    UFUNCTION(BlueprintCallable, Category = "CurveBlend")
    static UCurveFloat* AbsCurve(UCurveFloat* SourceCurve);

    // 钳制曲线值
    UFUNCTION(BlueprintCallable, Category = "CurveBlend")
    static UCurveFloat* ClampCurve(UCurveFloat* SourceCurve, float Min, float Max);
};
```
