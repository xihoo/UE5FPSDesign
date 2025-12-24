# UE5 FPS 游戏程序化动画蓝图设计方案

> 基于 3A 级 FPS 动画系统分析，采用 **C++ 核心驱动 + 蓝图表现** 的混合架构

---

## 目录

1. [系统概述](#1-系统概述)
2. [核心设计理念](#2-核心设计理念)
3. [系统架构](#3-系统架构)
4. [模块详解](#4-模块详解)
5. [C++ 代码实现](#5-c-代码实现)
6. [武器程序化动画配置系统](#6-武器程序化动画配置系统)
7. [AnimGraph 蓝图设计](#7-animgraph-蓝图设计)
8. [骨骼与资源需求](#8-骨骼与资源需求)
9. [实施步骤](#9-实施步骤)
10. [性能优化建议](#10-性能优化建议)

---

## 1. 系统概述

### 1.1 设计目标

构建一套 **模块化、分层级、高度程序化** 的现代 FPS 角色动画系统，实现以下核心功能：

| 功能模块 | 描述 |
|---------|------|
| 基础移动 | 走、跑、蹲、跳跃的自然过渡 |
| 滑步修正 | 使用 Motion Warping 解决脚部滑动问题 |
| 武器持握 | 支持多种武器类型的静态姿态切换 |
| 瞄准系统 | 腰射（Hip Fire）与机瞄（ADS）平滑过渡 |
| 程序化动画 | 后坐力、武器摆动、呼吸感、惯性延迟 |
| IK 系统 | 双手精准吸附枪械，适配不同握把 |
| 原地转身 | Turn In Place 系统 |

### 1.2 核心优势

```
┌─────────────────────────────────────────────────────────────────┐
│  传统方案：大量预制动画资源 → 状态机硬切 → 手感僵硬            │
├─────────────────────────────────────────────────────────────────┤
│  本方案：数学驱动 + IK 跟随 → Alpha 平滑过渡 → 手感丝滑        │
└─────────────────────────────────────────────────────────────────┘
```

- **极高复用性**：新增武器只需调整 IK 坐标和后坐力参数
- **丝滑过渡**：所有状态通过 Alpha 值平滑切换，无硬切
- **动态反馈**：后坐力/呼吸感实时计算，可根据游戏状态动态调整
- **性能优越**：核心计算在 C++ 原生代码中运行

---

## 2. 核心设计理念

### 2.1 职责分离

```
┌─────────────────────────────────────────────────────────────────┐
│                        系统架构                                  │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│   ┌─────────────┐      ┌─────────────┐      ┌─────────────┐    │
│   │   C++ 层    │ ──── │  蓝图层     │ ──── │  渲染层     │    │
│   │  (大脑)     │      │  (骨架)     │      │  (皮肤)     │    │
│   └─────────────┘      └─────────────┘      └─────────────┘    │
│         │                    │                    │             │
│   · 数据计算            · 节点连接           · 最终显示        │
│   · 状态判断            · 动画混合           · 骨骼变换        │
│   · 程序化数学          · IK 解算                              │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### 2.2 虚拟骨骼驱动

系统的核心是 **`VB_Hand_Gun`（虚拟枪械骨骼）**：

- 所有程序化效果（后坐力、Sway、跑动位移）都作用于此虚拟骨骼
- 手臂 IK 自动追踪这个骨骼
- 只需移动一个点，整个武器和双手就会自然跟随

### 2.3 数据流向

```
角色状态（速度/朝向/输入）
        ↓
    C++ AnimInstance（计算）
        ↓
    蓝图变量（传递）
        ↓
    AnimGraph 节点（应用）
        ↓
    最终骨骼姿态（输出）
```

---

## 3. 系统架构

### 3.1 类图设计

```
┌──────────────────────────────────────────────────────────────────────┐
│                         类继承关系                                    │
├──────────────────────────────────────────────────────────────────────┤
│                                                                       │
│  UAnimInstance                                                        │
│       │                                                               │
│       └── UFPSCharacterAnimInstance (C++)                            │
│                  │                                                    │
│                  └── ABP_FPSCharacter (蓝图继承)                      │
│                                                                       │
│  ACharacter                                                           │
│       │                                                               │
│       └── AFPSCharacterBase (C++)                                    │
│                  │                                                    │
│                  ├── UWeaponComponent (武器组件)                      │
│                  ├── UProceduralAnimComponent (程序化动画组件)        │
│                  └── UFPSAnimInterface (动画接口)                     │
│                                                                       │
└──────────────────────────────────────────────────────────────────────┘
```

### 3.2 模块划分

| 模块名称 | 类型 | 职责 |
|---------|------|------|
| `UFPSCharacterAnimInstance` | C++ AnimInstance | 核心数据计算与状态管理 |
| `UProceduralAnimComponent` | C++ Component | 程序化动画算法（后坐力、Sway） |
| `UWeaponComponent` | C++ Component | 武器数据与开火逻辑 |
| `IFPSAnimInterface` | C++ Interface | 角色与动画蓝图通信接口 |
| `ABP_FPSCharacter` | 蓝图 AnimBP | AnimGraph 节点连接 |
| `ABP_FP_ArmsProcedural` | 蓝图 Linked AnimBP | 手臂程序化处理子图 |

---

## 4. 模块详解

### 4.1 动画管线五阶段

```
┌─────────────────────────────────────────────────────────────────────┐
│                     完整动画管线流程                                 │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌──────────────┐                                                   │
│  │ 第一阶段     │  构建基础运动（下半身）                           │
│  │ Locomotion   │  · BlendSpace 混合走/跑/蹲                        │
│  │              │  · Stride Warping 解决滑步                        │
│  │              │  · Orientation Warping 处理侧移                   │
│  └──────┬───────┘                                                   │
│         ↓                                                            │
│  ┌──────────────┐                                                   │
│  │ 第二阶段     │  全身拼装（上半身 + 武器）                        │
│  │ Upper Body   │  · 根据武器类型选择静态持枪姿势                   │
│  │              │  · Layered Blend 混合上下半身                     │
│  │              │  · Control Rig 处理脚部 IK                        │
│  └──────┬───────┘                                                   │
│         ↓                                                            │
│  ┌──────────────┐                                                   │
│  │ 第三阶段     │  程序化动画堆栈（核心魔法）                       │
│  │ Procedural   │  · 瞄准（ADS）vs 腰射（Hip Fire）分流             │
│  │              │  · 后坐力 / Sway / Bob / Lag 叠加                 │
│  │              │  · 跑动偏移 / 蹲伏倾斜                            │
│  └──────┬───────┘                                                   │
│         ↓                                                            │
│  ┌──────────────┐                                                   │
│  │ 第四阶段     │  身体力学与状态覆盖                               │
│  │ Body Mech    │  · 脊柱瞄准俯仰（Aim Offset）                     │
│  │              │  · 战术冲刺动画混合                               │
│  │              │  · 探头（Lean）机制                               │
│  └──────┬───────┘                                                   │
│         ↓                                                            │
│  ┌──────────────┐                                                   │
│  │ 第五阶段     │  IK 最终解算                                      │
│  │ Hand IK      │  · 右手 Two Bone IK 握枪把                        │
│  │              │  · 左手 Two Bone IK 抓护木                        │
│  │              │  · 动态握把位置适配                               │
│  └──────────────┘                                                   │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 4.2 程序化动画效果器链

这是第三阶段的核心，类似音频处理的效果器链：

```
输入姿态 ─→ [FREE AIM] ─→ [RECOIL] ─→ [FORWARD WALK TILT]
                                              ↓
     ┌─────────────────────────────────────────┘
     ↓
[JUMP/LAND] ─→ [SWAY] ─→ [BOB] ─→ [START/STOP LAG]
                                        ↓
     ┌──────────────────────────────────┘
     ↓
[WALL OFFSET] ─→ [WEAPON CHANGE] ─→ [SPRINT] ─→ [LEAN]
                                                   ↓
                                              最终姿态
```

每个效果器的作用：

| 效果器 | 功能描述 |
|--------|---------|
| FREE AIM | 枪口在屏幕范围内浮动，比准星慢半拍 |
| RECOIL | 开火时的后坐力上跳和横向抖动 |
| FORWARD WALK TILT | 向前走时枪口微微下压 |
| JUMP/LAND | 起跳下沉、落地冲击回弹 |
| SWAY | 鼠标转动时的武器惯性滞后 |
| BOB | 走路时的上下起伏（正弦波） |
| START/STOP LAG | 起步拖后、急停前冲 |
| WALL OFFSET | 靠近墙壁时自动收枪 |
| WEAPON CHANGE | 切枪时的甩动动作 |
| SPRINT | 跑动时枪口下压/侧偏 |
| LEAN | Q/E 探头机制 |

---

## 5. C++ 代码实现

### 5.1 数据结构定义

```cpp
// FPSAnimTypes.h

#pragma once
#include "CoreMinimal.h"
#include "FPSAnimTypes.generated.h"

//=============================================================================
// 武器类型枚举
//=============================================================================
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Unarmed     UMETA(DisplayName = "空手"),
    Rifle       UMETA(DisplayName = "步枪"),
    Pistol      UMETA(DisplayName = "手枪"),
    Shotgun     UMETA(DisplayName = "霰弹枪"),
    SMG         UMETA(DisplayName = "冲锋枪"),
    Sniper      UMETA(DisplayName = "狙击枪"),
    Knife       UMETA(DisplayName = "近战武器"),
    Launcher    UMETA(DisplayName = "发射器")
};

//=============================================================================
// 左手握把类型枚举
//=============================================================================
UENUM(BlueprintType)
enum class ELeftHandPoseType : uint8
{
    Default         UMETA(DisplayName = "默认"),
    VerticalGrip    UMETA(DisplayName = "垂直握把"),
    AngledGrip      UMETA(DisplayName = "三角握把"),
    Akimbo          UMETA(DisplayName = "双持")
};

//=============================================================================
// 角色移动状态
//=============================================================================
UENUM(BlueprintType)
enum class ELocomotionState : uint8
{
    Idle    UMETA(DisplayName = "待机"),
    Walk    UMETA(DisplayName = "行走"),
    Run     UMETA(DisplayName = "跑步"),
    Sprint  UMETA(DisplayName = "冲刺"),
    Crouch  UMETA(DisplayName = "蹲伏")
};

//=============================================================================
// 程序化动画数据结构体
//=============================================================================
USTRUCT(BlueprintType)
struct FProceduralAnimData
{
    GENERATED_BODY()

    // VB_Hand_Gun 的最终变换（所有效果叠加后的结果）
    UPROPERTY(BlueprintReadOnly, Category = "Procedural")
    FTransform WeaponBoneTransform = FTransform::Identity;

    // 左手 IK 位置（相对于武器）
    UPROPERTY(BlueprintReadOnly, Category = "IK")
    FVector LeftHandIKLocation = FVector::ZeroVector;

    // 左手 IK 旋转
    UPROPERTY(BlueprintReadOnly, Category = "IK")
    FRotator LeftHandIKRotation = FRotator::ZeroRotator;

    // 脊柱俯仰旋转（Aim Offset）
    UPROPERTY(BlueprintReadOnly, Category = "Spine")
    FRotator SpinePitchRotation = FRotator::ZeroRotator;

    // 左手 IK Alpha（0=关闭, 1=完全吸附）
    UPROPERTY(BlueprintReadOnly, Category = "IK")
    float LeftHandIKAlpha = 1.0f;
};

//=============================================================================
// 后坐力数据结构体
//=============================================================================
USTRUCT(BlueprintType)
struct FRecoilData
{
    GENERATED_BODY()

    // 当前后坐力位置偏移
    UPROPERTY(BlueprintReadOnly)
    FVector LocationOffset = FVector::ZeroVector;

    // 当前后坐力旋转偏移
    UPROPERTY(BlueprintReadOnly)
    FRotator RotationOffset = FRotator::ZeroRotator;

    // 目标后坐力位置
    FVector TargetLocation = FVector::ZeroVector;

    // 目标后坐力旋转
    FRotator TargetRotation = FRotator::ZeroRotator;
};
```

### 5.2 动画接口定义

```cpp
// FPSAnimInterface.h

#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FPSAnimTypes.h"
#include "FPSAnimInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UFPSAnimInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * FPS 动画通信接口
 * 角色类实现此接口，供 AnimInstance 获取数据
 */
class MYGAME_API IFPSAnimInterface
{
    GENERATED_BODY()

public:
    // 获取当前装备的武器类型
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
    EWeaponType GetEquippedWeaponType() const;

    // 获取左手握把类型
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
    ELeftHandPoseType GetLeftHandPoseType() const;

    // 是否正在瞄准
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
    bool IsAiming() const;

    // 是否蹲伏
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
    bool IsCrouching() const;

    // 是否冲刺
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
    bool IsSprinting() const;

    // 是否近战武器
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
    bool IsMeleeWeapon() const;

    // 获取左手 IK 世界位置（从武器 Socket 获取）
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
    FTransform GetLeftHandIKTransform() const;

    // 获取瞄准偏移角度（Pitch）
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
    float GetAimPitch() const;

    // 获取控制器 Yaw（用于 Turn In Place）
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
    float GetControllerYaw() const;
};
```

### 5.3 核心 AnimInstance 实现

```cpp
// FPSCharacterAnimInstance.h

#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FPSAnimTypes.h"
#include "FPSCharacterAnimInstance.generated.h"

UCLASS()
class MYGAME_API UFPSCharacterAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaTime) override;

    //=========================================================================
    // 蓝图可读变量 - 基础移动
    //=========================================================================
    
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    float GroundSpeed = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    float LocomotionDirection = 0.0f;  // -180 ~ 180

    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    bool bShouldMove = false;

    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    bool bIsFalling = false;

    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    bool bIsCrouching = false;

    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    ELocomotionState LocomotionState = ELocomotionState::Idle;

    //=========================================================================
    // 蓝图可读变量 - 武器与瞄准
    //=========================================================================

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    EWeaponType CurrentWeaponType = EWeaponType::Unarmed;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    ELeftHandPoseType LeftHandPose = ELeftHandPoseType::Default;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsAiming = false;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    float AimingAlpha = 0.0f;  // 0~1 平滑过渡

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsMelee = false;

    //=========================================================================
    // 蓝图可读变量 - Turn In Place
    //=========================================================================

    UPROPERTY(BlueprintReadOnly, Category = "TurnInPlace")
    float RootYawOffset = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "TurnInPlace")
    bool bIsTurning = false;

    UPROPERTY(BlueprintReadOnly, Category = "TurnInPlace")
    float UpperBodyYaw = 0.0f;

    //=========================================================================
    // 蓝图可读变量 - 程序化动画输出
    //=========================================================================

    UPROPERTY(BlueprintReadOnly, Category = "Procedural")
    FProceduralAnimData ProcAnimData;

    //=========================================================================
    // 可配置参数
    //=========================================================================

    UPROPERTY(EditDefaultsOnly, Category = "Config|Sway")
    float SwayIntensity = 2.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Config|Sway")
    float SwayInterpSpeed = 6.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Config|Sway")
    float SwayRollMultiplier = 0.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Config|Bob")
    float BobFrequency = 8.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Config|Bob")
    float BobAmplitude = 1.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Config|TurnInPlace")
    float TurnThreshold = 90.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Config|Aiming")
    float AimInterpSpeed = 10.0f;

protected:
    //=========================================================================
    // 引用缓存
    //=========================================================================
    
    UPROPERTY(BlueprintReadOnly, Category = "References")
    class ACharacter* OwnerCharacter = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "References")
    class UCharacterMovementComponent* MovementComponent = nullptr;

private:
    //=========================================================================
    // 内部计算函数
    //=========================================================================

    void CacheReferences();
    void UpdateLocomotionData(float DeltaTime);
    void UpdateCombatData(float DeltaTime);
    void UpdateTurnInPlace(float DeltaTime);
    void UpdateProceduralAnimation(float DeltaTime);

    // 程序化效果计算
    void CalculateSway(float DeltaTime);
    void CalculateBob(float DeltaTime);
    void CalculateRecoil(float DeltaTime);
    void CalculateSprintOffset(float DeltaTime);
    void CalculateSpineRotation(float DeltaTime);

    //=========================================================================
    // 内部状态变量
    //=========================================================================

    // Sway
    FRotator CurrentSwayRotation = FRotator::ZeroRotator;
    FRotator TargetSwayRotation = FRotator::ZeroRotator;
    float LastMouseX = 0.0f;
    float LastMouseY = 0.0f;

    // Bob
    float BobTimer = 0.0f;
    FVector CurrentBobOffset = FVector::ZeroVector;

    // Recoil
    FRecoilData RecoilData;

    // Sprint
    FTransform CurrentSprintOffset = FTransform::Identity;
    FTransform TargetSprintOffset = FTransform::Identity;

    // Turn In Place
    float TargetYawOffset = 0.0f;
    float LastActorYaw = 0.0f;
};
```

### 5.4 AnimInstance 实现文件

```cpp
// FPSCharacterAnimInstance.cpp

#include "FPSCharacterAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"
#include "FPSAnimInterface.h"

void UFPSCharacterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    CacheReferences();
}

void UFPSCharacterAnimInstance::CacheReferences()
{
    APawn* Pawn = TryGetPawnOwner();
    if (Pawn)
    {
        OwnerCharacter = Cast<ACharacter>(Pawn);
        if (OwnerCharacter)
        {
            MovementComponent = OwnerCharacter->GetCharacterMovement();
        }
    }
}

void UFPSCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if (!OwnerCharacter || !MovementComponent)
    {
        CacheReferences();
        return;
    }

    // 按顺序更新各模块
    UpdateLocomotionData(DeltaTime);
    UpdateCombatData(DeltaTime);
    UpdateTurnInPlace(DeltaTime);
    UpdateProceduralAnimation(DeltaTime);
}

//=============================================================================
// 移动数据更新
//=============================================================================
void UFPSCharacterAnimInstance::UpdateLocomotionData(float DeltaTime)
{
    // 获取速度
    FVector Velocity = OwnerCharacter->GetVelocity();
    GroundSpeed = Velocity.Size2D();

    // 计算移动方向 (-180 ~ 180)
    if (GroundSpeed > 0.1f)
    {
        FRotator ActorRotation = OwnerCharacter->GetActorRotation();
        LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(Velocity, ActorRotation);
    }

    // 判断是否应该播放移动动画
    FVector Acceleration = MovementComponent->GetCurrentAcceleration();
    bShouldMove = (GroundSpeed > 3.0f) && (Acceleration.Size() > 0.0f);

    // 空中状态
    bIsFalling = MovementComponent->IsFalling();

    // 蹲伏状态
    bIsCrouching = MovementComponent->IsCrouching();

    // 更新移动状态枚举
    if (bIsFalling)
    {
        LocomotionState = ELocomotionState::Idle;  // 空中时使用 Idle
    }
    else if (bIsCrouching)
    {
        LocomotionState = ELocomotionState::Crouch;
    }
    else if (GroundSpeed > 500.0f)  // 根据实际游戏调整阈值
    {
        LocomotionState = ELocomotionState::Sprint;
    }
    else if (GroundSpeed > 200.0f)
    {
        LocomotionState = ELocomotionState::Run;
    }
    else if (GroundSpeed > 3.0f)
    {
        LocomotionState = ELocomotionState::Walk;
    }
    else
    {
        LocomotionState = ELocomotionState::Idle;
    }
}

//=============================================================================
// 战斗数据更新（通过接口获取）
//=============================================================================
void UFPSCharacterAnimInstance::UpdateCombatData(float DeltaTime)
{
    // 检查角色是否实现了动画接口
    if (OwnerCharacter->GetClass()->ImplementsInterface(UFPSAnimInterface::StaticClass()))
    {
        CurrentWeaponType = IFPSAnimInterface::Execute_GetEquippedWeaponType(OwnerCharacter);
        LeftHandPose = IFPSAnimInterface::Execute_GetLeftHandPoseType(OwnerCharacter);
        bIsMelee = IFPSAnimInterface::Execute_IsMeleeWeapon(OwnerCharacter);
        
        bool bWantsToAim = IFPSAnimInterface::Execute_IsAiming(OwnerCharacter);
        
        // 平滑过渡瞄准状态
        float TargetAlpha = bWantsToAim ? 1.0f : 0.0f;
        AimingAlpha = FMath::FInterpTo(AimingAlpha, TargetAlpha, DeltaTime, AimInterpSpeed);
        bIsAiming = AimingAlpha > 0.5f;

        // 获取左手 IK 数据
        FTransform LeftHandTransform = IFPSAnimInterface::Execute_GetLeftHandIKTransform(OwnerCharacter);
        ProcAnimData.LeftHandIKLocation = LeftHandTransform.GetLocation();
        ProcAnimData.LeftHandIKRotation = LeftHandTransform.GetRotation().Rotator();
        
        // 近战武器时关闭左手 IK
        ProcAnimData.LeftHandIKAlpha = bIsMelee ? 0.0f : 1.0f;
    }
}

//=============================================================================
// 原地转身逻辑
//=============================================================================
void UFPSCharacterAnimInstance::UpdateTurnInPlace(float DeltaTime)
{
    if (!OwnerCharacter->GetClass()->ImplementsInterface(UFPSAnimInterface::StaticClass()))
    {
        return;
    }

    float ControllerYaw = IFPSAnimInterface::Execute_GetControllerYaw(OwnerCharacter);
    float ActorYaw = OwnerCharacter->GetActorRotation().Yaw;

    // 计算上半身扭转角度
    UpperBodyYaw = FMath::FindDeltaAngleDegrees(ActorYaw, ControllerYaw);

    // 如果正在移动，重置转身状态
    if (bShouldMove)
    {
        bIsTurning = false;
        TargetYawOffset = 0.0f;
        RootYawOffset = FMath::FInterpTo(RootYawOffset, 0.0f, DeltaTime, 10.0f);
        return;
    }

    // 静止时，检查是否需要转身
    if (!bIsTurning && FMath::Abs(UpperBodyYaw) > TurnThreshold)
    {
        bIsTurning = true;
        TargetYawOffset = UpperBodyYaw > 0.0f ? 90.0f : -90.0f;
        
        // TODO: 在这里触发转身蒙太奇
        // PlaySlotAnimationAsDynamicMontage(TurnAnimation, "Slide", 0.25f);
    }

    // 平滑插值根骨骼偏移
    RootYawOffset = FMath::FInterpTo(RootYawOffset, -UpperBodyYaw, DeltaTime, 5.0f);
    
    // 限制偏移范围
    RootYawOffset = FMath::Clamp(RootYawOffset, -90.0f, 90.0f);
}

//=============================================================================
// 程序化动画更新（核心）
//=============================================================================
void UFPSCharacterAnimInstance::UpdateProceduralAnimation(float DeltaTime)
{
    // 重置 Transform
    FTransform FinalTransform = FTransform::Identity;
    
    // 1. 计算 Sway（武器摆动）
    CalculateSway(DeltaTime);
    FinalTransform = FTransform(CurrentSwayRotation) * FinalTransform;

    // 2. 计算 Bob（走路晃动）
    CalculateBob(DeltaTime);
    FinalTransform.AddToTranslation(CurrentBobOffset);

    // 3. 计算后坐力
    CalculateRecoil(DeltaTime);
    FinalTransform = FTransform(RecoilData.RotationOffset, RecoilData.LocationOffset) * FinalTransform;

    // 4. 计算冲刺偏移
    CalculateSprintOffset(DeltaTime);
    FinalTransform = CurrentSprintOffset * FinalTransform;

    // 5. 计算脊柱旋转
    CalculateSpineRotation(DeltaTime);

    // 输出最终结果
    ProcAnimData.WeaponBoneTransform = FinalTransform;
}

//=============================================================================
// Sway 计算（鼠标移动时的武器惯性）
//=============================================================================
void UFPSCharacterAnimInstance::CalculateSway(float DeltaTime)
{
    // 获取鼠标输入增量
    APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
    if (!PC) return;

    float MouseX, MouseY;
    PC->GetInputMouseDelta(MouseX, MouseY);

    // 计算目标 Sway 旋转
    TargetSwayRotation = FRotator(
        -MouseY * SwayIntensity,                    // Pitch
        MouseX * SwayIntensity,                     // Yaw
        MouseX * SwayIntensity * SwayRollMultiplier // Roll
    );

    // 平滑插值
    CurrentSwayRotation = FMath::RInterpTo(
        CurrentSwayRotation, 
        TargetSwayRotation, 
        DeltaTime, 
        SwayInterpSpeed
    );

    // 回归零点
    TargetSwayRotation = FMath::RInterpTo(TargetSwayRotation, FRotator::ZeroRotator, DeltaTime, 4.0f);
}

//=============================================================================
// Bob 计算（走路晃动）
//=============================================================================
void UFPSCharacterAnimInstance::CalculateBob(float DeltaTime)
{
    if (bShouldMove && !bIsFalling)
    {
        // 根据速度调整频率
        float SpeedMultiplier = GroundSpeed / 400.0f;
        BobTimer += DeltaTime * BobFrequency * SpeedMultiplier;

        // 正弦波生成 Bob
        float BobZ = FMath::Sin(BobTimer) * BobAmplitude * SpeedMultiplier;
        float BobY = FMath::Cos(BobTimer * 0.5f) * BobAmplitude * 0.5f * SpeedMultiplier;

        CurrentBobOffset = FVector(0.0f, BobY, BobZ);
    }
    else
    {
        // 停止时平滑归零
        CurrentBobOffset = FMath::VInterpTo(CurrentBobOffset, FVector::ZeroVector, DeltaTime, 10.0f);
    }
}

//=============================================================================
// 后坐力计算
//=============================================================================
void UFPSCharacterAnimInstance::CalculateRecoil(float DeltaTime)
{
    // 后坐力回归零点
    RecoilData.LocationOffset = FMath::VInterpTo(
        RecoilData.LocationOffset, 
        FVector::ZeroVector, 
        DeltaTime, 
        15.0f
    );

    RecoilData.RotationOffset = FMath::RInterpTo(
        RecoilData.RotationOffset, 
        FRotator::ZeroRotator, 
        DeltaTime, 
        15.0f
    );
}

//=============================================================================
// 冲刺偏移计算
//=============================================================================
void UFPSCharacterAnimInstance::CalculateSprintOffset(float DeltaTime)
{
    bool bWantsToSprint = (LocomotionState == ELocomotionState::Sprint);

    if (bWantsToSprint && !bIsAiming)
    {
        // 冲刺时枪口下压和侧偏
        TargetSprintOffset = FTransform(
            FRotator(-15.0f, 20.0f, -10.0f),  // 旋转
            FVector(-5.0f, 10.0f, -8.0f)      // 位移
        );
    }
    else
    {
        TargetSprintOffset = FTransform::Identity;
    }

    // 平滑插值
    CurrentSprintOffset.Blend(
        CurrentSprintOffset, 
        TargetSprintOffset, 
        FMath::Clamp(DeltaTime * 8.0f, 0.0f, 1.0f)
    );
}

//=============================================================================
// 脊柱旋转计算（Aim Offset）
//=============================================================================
void UFPSCharacterAnimInstance::CalculateSpineRotation(float DeltaTime)
{
    if (OwnerCharacter->GetClass()->ImplementsInterface(UFPSAnimInterface::StaticClass()))
    {
        float AimPitch = IFPSAnimInterface::Execute_GetAimPitch(OwnerCharacter);
        
        // 将俯仰角度分摊到脊柱（这里只输出一个值，在蓝图中分摊到多个骨骼）
        ProcAnimData.SpinePitchRotation = FRotator(AimPitch * 0.5f, 0.0f, 0.0f);
    }
}

//=============================================================================
// 公开函数：添加后坐力（供武器调用）
//=============================================================================
void UFPSCharacterAnimInstance::AddRecoil(FVector LocationImpulse, FRotator RotationImpulse)
{
    RecoilData.LocationOffset += LocationImpulse;
    RecoilData.RotationOffset += RotationImpulse;
}
```

### 5.5 程序化动画组件

```cpp
// ProceduralAnimComponent.h

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralAnimComponent.generated.h"

/**
 * 程序化动画组件
 * 负责处理复杂的动画算法，供 AnimInstance 读取结果
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYGAME_API UProceduralAnimComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UProceduralAnimComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
        FActorComponentTickFunction* ThisTickFunction) override;

    //=========================================================================
    // 公开接口
    //=========================================================================

    // 触发后坐力
    UFUNCTION(BlueprintCallable, Category = "Procedural")
    void TriggerRecoil(FVector LocationKick, FRotator RotationKick);

    // 触发落地冲击
    UFUNCTION(BlueprintCallable, Category = "Procedural")
    void TriggerLandingImpact(float ImpactStrength);

    // 设置墙壁偏移
    UFUNCTION(BlueprintCallable, Category = "Procedural")
    void SetWallOffset(float OffsetAlpha, FVector OffsetDirection);

    //=========================================================================
    // 输出数据（供 AnimInstance 读取）
    //=========================================================================

    UPROPERTY(BlueprintReadOnly, Category = "Output")
    FVector RecoilLocationOffset = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category = "Output")
    FRotator RecoilRotationOffset = FRotator::ZeroRotator;

    UPROPERTY(BlueprintReadOnly, Category = "Output")
    FVector LandingOffset = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category = "Output")
    FTransform WallAvoidanceOffset = FTransform::Identity;

    //=========================================================================
    // 可配置参数
    //=========================================================================

    UPROPERTY(EditAnywhere, Category = "Config|Recoil")
    float RecoilRecoverySpeed = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Config|Landing")
    float LandingRecoverySpeed = 8.0f;

    UPROPERTY(EditAnywhere, Category = "Config|WallOffset")
    float WallOffsetInterpSpeed = 5.0f;

private:
    // 目标值（用于插值）
    FVector TargetRecoilLocation = FVector::ZeroVector;
    FRotator TargetRecoilRotation = FRotator::ZeroRotator;
    FVector TargetLandingOffset = FVector::ZeroVector;
    FTransform TargetWallOffset = FTransform::Identity;
};
```

---

## 6. 武器程序化动画配置系统

不同武器应该有不同的程序化动画表现（手枪轻巧、狙击枪笨重）。本章介绍如何通过 **数据资产（Data Asset）** 和 **曲线（Curve）** 来配置每把武器的独特手感。

### 6.1 设计思路

```
┌─────────────────────────────────────────────────────────────────────┐
│                    武器程序化动画配置架构                            │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   ┌──────────────────┐                                              │
│   │  UDataAsset      │  每把武器一个配置文件                         │
│   │  (数据资产)       │  DA_AK47_ProcAnim.uasset                     │
│   └────────┬─────────┘                                              │
│            │                                                         │
│            ▼                                                         │
│   ┌──────────────────┐                                              │
│   │  WeaponComponent │  持有当前武器的配置引用                       │
│   └────────┬─────────┘                                              │
│            │                                                         │
│            ▼                                                         │
│   ┌──────────────────┐                                              │
│   │  AnimInstance    │  从配置中读取参数，应用到计算                 │
│   └──────────────────┘                                              │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 6.2 武器程序化动画数据资产

```cpp
// WeaponProceduralAnimConfig.h

#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponProceduralAnimConfig.generated.h"

/**
 * 武器程序化动画配置数据资产
 * 每把武器一个配置，定义其独特的动画手感
 */
UCLASS(BlueprintType)
class MYGAME_API UWeaponProceduralAnimConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    //=========================================================================
    // 基础信息
    //=========================================================================
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
    FName WeaponName = "DefaultWeapon";

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
    FText WeaponDisplayName;

    //=========================================================================
    // Sway 配置（武器摆动）
    //=========================================================================
    
    /** 鼠标移动时武器摆动的强度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sway")
    float SwayIntensity = 2.0f;

    /** 摆动插值速度（越大响应越快） */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sway")
    float SwayInterpSpeed = 6.0f;

    /** Roll 轴摆动倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sway")
    float SwayRollMultiplier = 0.5f;

    /** 瞄准时 Sway 的衰减系数（0.3 = 减弱到 30%）*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sway", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float AimingSwayMultiplier = 0.3f;

    //=========================================================================
    // Bob 配置（走路晃动）
    //=========================================================================
    
    /** 走路晃动频率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bob")
    float BobFrequency = 8.0f;

    /** 走路晃动幅度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bob")
    float BobAmplitude = 1.5f;

    /** 水平晃动倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bob")
    float BobHorizontalMultiplier = 0.5f;

    //=========================================================================
    // Recoil 配置（后坐力）
    //=========================================================================
    
    /** 后坐力恢复速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil")
    float RecoilRecoverySpeed = 10.0f;

    /** 后坐力 Pitch（上跳）范围 [Min, Max] */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil")
    FVector2D RecoilPitchRange = FVector2D(2.0f, 4.0f);

    /** 后坐力 Yaw（横向）范围 [Min, Max] */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil")
    FVector2D RecoilYawRange = FVector2D(-1.0f, 1.0f);

    /** 后坐力 Roll 范围 [Min, Max] */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil")
    FVector2D RecoilRollRange = FVector2D(-0.5f, 0.5f);

    /** 后坐力位移（向后踢） */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil")
    float RecoilKickback = 3.0f;

    /** 瞄准时后坐力衰减系数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float AimingRecoilMultiplier = 0.6f;

    //=========================================================================
    // Recoil 曲线配置（后坐力时间变化）
    //=========================================================================
    
    /** 后坐力 Pitch 随时间变化曲线（开火后 0~0.3 秒）*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Curves")
    UCurveFloat* RecoilPitchCurve = nullptr;

    /** 后坐力恢复曲线 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Curves")
    UCurveFloat* RecoilRecoveryCurve = nullptr;

    //=========================================================================
    // Recoil 累积配置（连发武器专用）
    //=========================================================================
    
    /** 是否启用后坐力累积（冲锋枪/机枪） */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Accumulation")
    bool bAccumulateRecoil = false;

    /** 每发子弹累积的后坐力倍率增量 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Accumulation", 
        meta = (EditCondition = "bAccumulateRecoil"))
    float AccumulationRate = 0.1f;

    /** 最大累积倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Accumulation", 
        meta = (EditCondition = "bAccumulateRecoil"))
    float MaxAccumulationMultiplier = 2.0f;

    /** 停止射击后多久开始衰减累积（秒） */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Accumulation", 
        meta = (EditCondition = "bAccumulateRecoil"))
    float AccumulationDecayDelay = 0.2f;

    /** 累积衰减速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Accumulation", 
        meta = (EditCondition = "bAccumulateRecoil"))
    float AccumulationDecaySpeed = 3.0f;

    //=========================================================================
    // Sprint 配置（冲刺偏移）
    //=========================================================================
    
    /** 冲刺时的位置偏移 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprint")
    FVector SprintLocationOffset = FVector(-5.0f, 10.0f, -8.0f);

    /** 冲刺时的旋转偏移 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprint")
    FRotator SprintRotationOffset = FRotator(-15.0f, 20.0f, -10.0f);

    /** 进入冲刺的过渡速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sprint")
    float SprintTransitionSpeed = 8.0f;

    //=========================================================================
    // 左手 IK 配置
    //=========================================================================
    
    /** 左手 IK 位置偏移（相对于武器 Socket） */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
    FVector LeftHandIKOffset = FVector::ZeroVector;

    /** 左手 IK 旋转偏移 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
    FRotator LeftHandIKRotationOffset = FRotator::ZeroRotator;

    //=========================================================================
    // 辅助函数
    //=========================================================================
    
    /** 获取随机化的后坐力旋转 */
    UFUNCTION(BlueprintCallable, Category = "Recoil")
    FRotator GetRandomRecoilRotation() const
    {
        return FRotator(
            FMath::RandRange(RecoilPitchRange.X, RecoilPitchRange.Y),
            FMath::RandRange(RecoilYawRange.X, RecoilYawRange.Y),
            FMath::RandRange(RecoilRollRange.X, RecoilRollRange.Y)
        );
    }

    /** 获取后坐力位移向量 */
    UFUNCTION(BlueprintCallable, Category = "Recoil")
    FVector GetRecoilKickbackVector() const
    {
        return FVector(-RecoilKickback, 0.0f, 0.0f);
    }
};
```

### 6.3 接口扩展

在 `IFPSAnimInterface` 中添加获取武器配置的方法：

```cpp
// 在 FPSAnimInterface.h 中添加

// 获取当前武器的程序化动画配置
UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
UWeaponProceduralAnimConfig* GetWeaponAnimConfig() const;
```

### 6.4 AnimInstance 动态参数读取

修改 `UFPSCharacterAnimInstance`，使其从武器配置中读取参数：

```cpp
// 在 FPSCharacterAnimInstance.h 中添加

protected:
    //=========================================================================
    // 当前武器配置（动态获取）
    //=========================================================================
    
    UPROPERTY(BlueprintReadOnly, Category = "WeaponConfig")
    UWeaponProceduralAnimConfig* CurrentWeaponConfig = nullptr;

    //=========================================================================
    // 运行时参数（从配置中读取或使用默认值）
    //=========================================================================
    
    // Sway
    float ActiveSwayIntensity = 2.0f;
    float ActiveSwayInterpSpeed = 6.0f;
    float ActiveSwayRollMultiplier = 0.5f;
    
    // Bob
    float ActiveBobFrequency = 8.0f;
    float ActiveBobAmplitude = 1.5f;
    
    // Recoil
    float ActiveRecoilRecoverySpeed = 10.0f;
    
    // Recoil Accumulation
    float CurrentAccumulationMultiplier = 1.0f;
    float TimeSinceLastShot = 0.0f;
    
    // Sprint
    FVector ActiveSprintLocationOffset;
    FRotator ActiveSprintRotationOffset;

private:
    /** 从武器配置更新活动参数 */
    void UpdateActiveParametersFromConfig();
    
    /** 更新后坐力累积 */
    void UpdateRecoilAccumulation(float DeltaTime);
```

```cpp
// 在 FPSCharacterAnimInstance.cpp 中添加

void UFPSCharacterAnimInstance::UpdateCombatData(float DeltaTime)
{
    if (OwnerCharacter->GetClass()->ImplementsInterface(UFPSAnimInterface::StaticClass()))
    {
        // ... 原有代码 ...
        
        // 获取武器程序化动画配置
        CurrentWeaponConfig = IFPSAnimInterface::Execute_GetWeaponAnimConfig(OwnerCharacter);
        
        // 更新活动参数
        UpdateActiveParametersFromConfig();
    }
}

void UFPSCharacterAnimInstance::UpdateActiveParametersFromConfig()
{
    if (CurrentWeaponConfig)
    {
        // Sway 参数
        ActiveSwayIntensity = CurrentWeaponConfig->SwayIntensity;
        ActiveSwayInterpSpeed = CurrentWeaponConfig->SwayInterpSpeed;
        ActiveSwayRollMultiplier = CurrentWeaponConfig->SwayRollMultiplier;
        
        // 瞄准时衰减 Sway
        if (bIsAiming)
        {
            ActiveSwayIntensity *= CurrentWeaponConfig->AimingSwayMultiplier;
        }
        
        // Bob 参数
        ActiveBobFrequency = CurrentWeaponConfig->BobFrequency;
        ActiveBobAmplitude = CurrentWeaponConfig->BobAmplitude;
        
        // Recoil 参数
        ActiveRecoilRecoverySpeed = CurrentWeaponConfig->RecoilRecoverySpeed;
        
        // Sprint 参数
        ActiveSprintLocationOffset = CurrentWeaponConfig->SprintLocationOffset;
        ActiveSprintRotationOffset = CurrentWeaponConfig->SprintRotationOffset;
    }
    else
    {
        // 使用默认值
        ActiveSwayIntensity = SwayIntensity;
        ActiveSwayInterpSpeed = SwayInterpSpeed;
        ActiveSwayRollMultiplier = SwayRollMultiplier;
        ActiveBobFrequency = BobFrequency;
        ActiveBobAmplitude = BobAmplitude;
        ActiveRecoilRecoverySpeed = 10.0f;
        ActiveSprintLocationOffset = FVector(-5.0f, 10.0f, -8.0f);
        ActiveSprintRotationOffset = FRotator(-15.0f, 20.0f, -10.0f);
    }
}

void UFPSCharacterAnimInstance::UpdateRecoilAccumulation(float DeltaTime)
{
    if (!CurrentWeaponConfig || !CurrentWeaponConfig->bAccumulateRecoil)
    {
        CurrentAccumulationMultiplier = 1.0f;
        return;
    }
    
    TimeSinceLastShot += DeltaTime;
    
    // 超过衰减延迟后开始衰减
    if (TimeSinceLastShot > CurrentWeaponConfig->AccumulationDecayDelay)
    {
        CurrentAccumulationMultiplier = FMath::FInterpTo(
            CurrentAccumulationMultiplier,
            1.0f,
            DeltaTime,
            CurrentWeaponConfig->AccumulationDecaySpeed
        );
    }
}

// 修改 AddRecoil 函数，支持累积
void UFPSCharacterAnimInstance::AddRecoil(FVector LocationImpulse, FRotator RotationImpulse)
{
    // 应用累积倍率
    float Multiplier = CurrentAccumulationMultiplier;
    
    // 瞄准时衰减
    if (bIsAiming && CurrentWeaponConfig)
    {
        Multiplier *= CurrentWeaponConfig->AimingRecoilMultiplier;
    }
    
    RecoilData.LocationOffset += LocationImpulse * Multiplier;
    RecoilData.RotationOffset += RotationImpulse * Multiplier;
    
    // 增加累积
    if (CurrentWeaponConfig && CurrentWeaponConfig->bAccumulateRecoil)
    {
        CurrentAccumulationMultiplier = FMath::Min(
            CurrentAccumulationMultiplier + CurrentWeaponConfig->AccumulationRate,
            CurrentWeaponConfig->MaxAccumulationMultiplier
        );
        TimeSinceLastShot = 0.0f;
    }
}

// 修改 CalculateSway，使用活动参数
void UFPSCharacterAnimInstance::CalculateSway(float DeltaTime)
{
    APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
    if (!PC) return;

    float MouseX, MouseY;
    PC->GetInputMouseDelta(MouseX, MouseY);

    // 使用活动参数
    TargetSwayRotation = FRotator(
        -MouseY * ActiveSwayIntensity,
        MouseX * ActiveSwayIntensity,
        MouseX * ActiveSwayIntensity * ActiveSwayRollMultiplier
    );

    CurrentSwayRotation = FMath::RInterpTo(
        CurrentSwayRotation, 
        TargetSwayRotation, 
        DeltaTime, 
        ActiveSwayInterpSpeed
    );

    TargetSwayRotation = FMath::RInterpTo(TargetSwayRotation, FRotator::ZeroRotator, DeltaTime, 4.0f);
}

// 修改 CalculateBob，使用活动参数
void UFPSCharacterAnimInstance::CalculateBob(float DeltaTime)
{
    if (bShouldMove && !bIsFalling)
    {
        float SpeedMultiplier = GroundSpeed / 400.0f;
        BobTimer += DeltaTime * ActiveBobFrequency * SpeedMultiplier;

        float BobZ = FMath::Sin(BobTimer) * ActiveBobAmplitude * SpeedMultiplier;
        float BobY = FMath::Cos(BobTimer * 0.5f) * ActiveBobAmplitude * 0.5f * SpeedMultiplier;

        CurrentBobOffset = FVector(0.0f, BobY, BobZ);
    }
    else
    {
        CurrentBobOffset = FMath::VInterpTo(CurrentBobOffset, FVector::ZeroVector, DeltaTime, 10.0f);
    }
}

// 修改 CalculateRecoil，使用活动参数和曲线
void UFPSCharacterAnimInstance::CalculateRecoil(float DeltaTime)
{
    float RecoverySpeed = ActiveRecoilRecoverySpeed;
    
    // 如果有恢复曲线，使用曲线值
    if (CurrentWeaponConfig && CurrentWeaponConfig->RecoilRecoveryCurve)
    {
        // 曲线时间可以基于当前后坐力大小
        float RecoilMagnitude = RecoilData.RotationOffset.Pitch / 10.0f;  // 归一化
        float CurveValue = CurrentWeaponConfig->RecoilRecoveryCurve->GetFloatValue(RecoilMagnitude);
        RecoverySpeed *= CurveValue;
    }
    
    RecoilData.LocationOffset = FMath::VInterpTo(
        RecoilData.LocationOffset, 
        FVector::ZeroVector, 
        DeltaTime, 
        RecoverySpeed
    );

    RecoilData.RotationOffset = FMath::RInterpTo(
        RecoilData.RotationOffset, 
        FRotator::ZeroRotator, 
        DeltaTime, 
        RecoverySpeed
    );
    
    // 更新累积
    UpdateRecoilAccumulation(DeltaTime);
}
```

### 6.5 武器组件集成

在 `UWeaponComponent` 中使用配置：

```cpp
// WeaponComponent.h

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYGAME_API UWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    //=========================================================================
    // 程序化动画配置
    //=========================================================================
    
    /** 此武器的程序化动画配置 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    UWeaponProceduralAnimConfig* ProceduralAnimConfig = nullptr;

    /** 触发开火后坐力 */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void Fire();

private:
    class UFPSCharacterAnimInstance* GetOwnerAnimInstance() const;
};

// WeaponComponent.cpp

void UWeaponComponent::Fire()
{
    if (!ProceduralAnimConfig) return;
    
    UFPSCharacterAnimInstance* AnimInstance = GetOwnerAnimInstance();
    if (!AnimInstance) return;
    
    // 获取随机化的后坐力
    FRotator RecoilRotation = ProceduralAnimConfig->GetRandomRecoilRotation();
    FVector RecoilLocation = ProceduralAnimConfig->GetRecoilKickbackVector();
    
    // 应用后坐力
    AnimInstance->AddRecoil(RecoilLocation, RecoilRotation);
    
    // ... 其他开火逻辑 ...
}
```

### 6.6 典型武器参数配置表

以下是各类武器的推荐参数配置：

| 参数 | 手枪 | 步枪 | 狙击枪 | 冲锋枪 | 霰弹枪 |
|-----|------|------|--------|--------|--------|
| **Sway** |
| SwayIntensity | 1.0 | 2.0 | 4.0 | 1.5 | 2.5 |
| SwayInterpSpeed | 8.0 | 6.0 | 3.0 | 7.0 | 5.0 |
| AimingSwayMultiplier | 0.2 | 0.3 | 0.5 | 0.25 | 0.4 |
| **Bob** |
| BobFrequency | 10.0 | 8.0 | 6.0 | 9.0 | 7.0 |
| BobAmplitude | 1.0 | 1.5 | 2.0 | 1.2 | 1.8 |
| **Recoil** |
| RecoilPitchRange | 2~3 | 3~5 | 8~12 | 1~2 | 10~15 |
| RecoilYawRange | -0.5~0.5 | -1~1 | -0.3~0.3 | -0.8~0.8 | -2~2 |
| RecoilKickback | 2.0 | 3.0 | 6.0 | 1.5 | 8.0 |
| RecoilRecoverySpeed | 15.0 | 10.0 | 5.0 | 12.0 | 8.0 |
| AimingRecoilMultiplier | 0.5 | 0.6 | 0.7 | 0.55 | 0.65 |
| **Accumulation** |
| bAccumulateRecoil | ❌ | ❌ | ❌ | ✅ | ❌ |
| AccumulationRate | - | - | - | 0.15 | - |
| MaxAccumulation | - | - | - | 2.5 | - |
| **Sprint** |
| SprintLocation | (-3,8,-5) | (-5,10,-8) | (-8,15,-12) | (-4,9,-6) | (-6,12,-10) |
| SprintRotation | (-10,15,-8) | (-15,20,-10) | (-20,25,-15) | (-12,18,-9) | (-18,22,-12) |

### 6.7 后坐力曲线示例

```
后坐力 Pitch 曲线（开火后 0~0.3 秒）

手枪曲线（快速上跳，快速恢复）:
    ▲ Pitch
 4  │    ╱╲
 3  │   ╱  ╲
 2  │  ╱    ╲
 1  │ ╱      ╲___
 0  └─────────────► Time
    0  0.05 0.1  0.2  0.3

狙击枪曲线（缓慢上跳，缓慢恢复）:
    ▲ Pitch
12  │        ╱───╲
 9  │      ╱      ╲
 6  │    ╱         ╲
 3  │  ╱            ╲___
 0  └────────────────────► Time
    0   0.1   0.2   0.3   0.5

冲锋枪曲线（连发累积，不完全恢复）:
    ▲ Pitch
 4  │      ╱──╱──╱──╱──
 3  │    ╱──╱──╱──╱
 2  │  ╱──╱──╱
 1  │╱──╱
 0  └────────────────────► Time (连续开火)
```

### 6.8 资源组织结构

```
Content/
├── Weapons/
│   ├── Configs/                          ← 程序化动画配置
│   │   ├── DA_Pistol_ProcAnim.uasset
│   │   ├── DA_Rifle_ProcAnim.uasset
│   │   ├── DA_Sniper_ProcAnim.uasset
│   │   ├── DA_SMG_ProcAnim.uasset
│   │   └── DA_Shotgun_ProcAnim.uasset
│   │
│   ├── Curves/                           ← 后坐力曲线
│   │   ├── Curve_Recoil_Pistol.uasset
│   │   ├── Curve_Recoil_Rifle.uasset
│   │   ├── Curve_Recoil_Sniper.uasset
│   │   ├── Curve_Recovery_Fast.uasset
│   │   └── Curve_Recovery_Slow.uasset
│   │
│   ├── Pistol/
│   │   ├── WBP_Glock.uasset              ← 武器蓝图
│   │   └── SM_Glock.uasset               ← 武器模型
│   │
│   └── Rifle/
│       ├── WBP_AK47.uasset
│       └── SM_AK47.uasset
```

---

## 7. AnimGraph 蓝图设计

### 7.1 主动画蓝图结构

由于 C++ 处理了所有计算，蓝图层变得非常简洁：

```
┌─────────────────────────────────────────────────────────────────────┐
│                    ABP_FPSCharacter AnimGraph                        │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                     LOCOMOTION LAYER                         │   │
│  ├─────────────────────────────────────────────────────────────┤   │
│  │                                                              │   │
│  │  BlendSpace (BS_Locomotion)                                  │   │
│  │       ↓                                                      │   │
│  │  [Blend by bool: bIsCrouching]                              │   │
│  │       │── True: BS_Crouch                                   │   │
│  │       └── False: BS_Stand                                   │   │
│  │       ↓                                                      │   │
│  │  [Orientation Warping] ← LocomotionDirection                │   │
│  │       ↓                                                      │   │
│  │  [Stride Warping] ← GroundSpeed                             │   │
│  │       ↓                                                      │   │
│  │  [Slot: "Slide"] (转身蒙太奇)                                │   │
│  │       ↓                                                      │   │
│  │  [Rotate Root Bone] ← RootYawOffset                         │   │
│  │       ↓                                                      │   │
│  │  Cache: "LocomotionPose"                                     │   │
│  │                                                              │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                           ↓                                          │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                     UPPER BODY LAYER                         │   │
│  ├─────────────────────────────────────────────────────────────┤   │
│  │                                                              │   │
│  │  [Sequence Evaluator] × N (静态武器姿势)                     │   │
│  │       ↓                                                      │   │
│  │  [Blend Poses by Enum: CurrentWeaponType]                   │   │
│  │       ↓                                                      │   │
│  │  [Slot: "UpperBody"] (换弹等蒙太奇)                          │   │
│  │       ↓                                                      │   │
│  │  Cache: "WeaponPose"                                         │   │
│  │                                                              │   │
│  │  [Layered Blend Per Bone] (Spine_01)                        │   │
│  │       │── Base: LocomotionPose                              │   │
│  │       └── Blend: WeaponPose                                 │   │
│  │                                                              │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                           ↓                                          │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                   LINKED ANIM GRAPH                          │   │
│  │                  ABP_FP_ArmsProcedural                       │   │
│  ├─────────────────────────────────────────────────────────────┤   │
│  │  输入引脚:                                                   │   │
│  │    · bIsAiming                                               │   │
│  │    · bIsMelee                                                │   │
│  │    · ProcAnimData                                            │   │
│  │    · AimingAlpha                                             │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                           ↓                                          │
│                      Output Pose                                     │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 7.2 手臂程序化子蓝图结构

```
┌─────────────────────────────────────────────────────────────────────┐
│                  ABP_FP_ArmsProcedural AnimGraph                     │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  Input Pose                                                          │
│       ↓                                                              │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                   AIMING LOGIC                               │   │
│  ├─────────────────────────────────────────────────────────────┤   │
│  │  [Blend Poses by bool: bIsAiming]                           │   │
│  │       │── True: ADS Path (瞄准路径)                         │   │
│  │       │      · [Slot: "AimSlot"]                            │   │
│  │       │      · 双持/战术瞄准修正                             │   │
│  │       └── False: Hip Fire Path (腰射路径)                   │   │
│  │              · [Slot: "DefaultSlot"] (换弹蒙太奇)           │   │
│  │              · 蹲伏倾斜修正                                  │   │
│  │  Blend Time: 0.25s                                           │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                           ↓                                          │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                PROCEDURAL TRANSFORM                          │   │
│  ├─────────────────────────────────────────────────────────────┤   │
│  │                                                              │   │
│  │  [Local To Component]                                        │   │
│  │       ↓                                                      │   │
│  │  [Transform (Modify) Bone: VB_Hand_Gun]                     │   │
│  │       · Translation: ProcAnimData.WeaponBoneTransform.Loc   │   │
│  │       · Rotation: ProcAnimData.WeaponBoneTransform.Rot      │   │
│  │       · Space: Component Space                               │   │
│  │       ↓                                                      │   │
│  │  [Slot: "Arms"] (通用手臂蒙太奇)                             │   │
│  │                                                              │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                           ↓                                          │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                   SPINE AIM OFFSET                           │   │
│  ├─────────────────────────────────────────────────────────────┤   │
│  │                                                              │   │
│  │  [Transform (Modify) Bone: Spine_04]                        │   │
│  │       · Rotation: ProcAnimData.SpinePitchRotation × 0.3     │   │
│  │       ↓                                                      │   │
│  │  [Transform (Modify) Bone: Spine_05]                        │   │
│  │       · Rotation: ProcAnimData.SpinePitchRotation × 0.4     │   │
│  │       ↓                                                      │   │
│  │  [Transform (Modify) Bone: Neck]                            │   │
│  │       · Rotation: ProcAnimData.SpinePitchRotation × 0.3     │   │
│  │                                                              │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                           ↓                                          │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                      HAND IK                                 │   │
│  ├─────────────────────────────────────────────────────────────┤   │
│  │                                                              │   │
│  │  [Copy Bone: VB_Hand_Gun → IK_Hand_Gun]                     │   │
│  │       ↓                                                      │   │
│  │  [Two Bone IK: Hand_R]                                      │   │
│  │       · Effector: IK_Hand_Gun                               │   │
│  │       ↓                                                      │   │
│  │  [Two Bone IK: Hand_L]                                      │   │
│  │       · Effector Location: ProcAnimData.LeftHandIKLocation  │   │
│  │       · Alpha: ProcAnimData.LeftHandIKAlpha                 │   │
│  │       ↓                                                      │   │
│  │  [Component To Local]                                        │   │
│  │                                                              │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                           ↓                                          │
│                      Output Pose                                     │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 7.3 关键节点配置

#### Transform (Modify) Bone 配置

| 属性 | 值 |
|-----|-----|
| Bone to Modify | VB_Hand_Gun |
| Translation Mode | Add to Existing |
| Rotation Mode | Add to Existing |
| Translation Space | Component Space |
| Rotation Space | Component Space |
| Alpha | 1.0 |

#### Two Bone IK 配置（右手）

| 属性 | 值 |
|-----|-----|
| IK Bone | Hand_R |
| Effector Target | IK_Hand_Gun (Bone) |
| Allow Stretching | False |
| Take Rotation from Effector | True |

#### Two Bone IK 配置（左手）

| 属性 | 值 |
|-----|-----|
| IK Bone | Hand_L |
| Effector Location Space | Component Space |
| Effector Location | ProcAnimData.LeftHandIKLocation |
| Alpha | ProcAnimData.LeftHandIKAlpha |

---

## 8. 骨骼与资源需求

### 8.1 必需的虚拟骨骼

在骨骼编辑器中需要添加以下虚拟骨骼：

| 虚拟骨骼名称 | 父骨骼 | 用途 |
|------------|--------|------|
| VB_Hand_Gun | Hand_R | 程序化动画的核心锚点 |
| IK_Hand_Gun | Hand_R | 右手 IK 目标 |
| IK_Hand_L | Hand_L | 左手 IK 目标 |

### 8.2 必需的动画资源

#### 基础移动

| 资源名称 | 类型 | 描述 |
|---------|------|------|
| BS_Locomotion | BlendSpace 2D | 站立移动混合空间 (Direction × Speed) |
| BS_Crouch | BlendSpace 2D | 蹲伏移动混合空间 |
| Anim_Idle | Sequence | 站立待机 |
| Anim_Jump_Start | Sequence | 起跳 |
| Anim_Jump_Loop | Sequence | 空中循环 |
| Anim_Jump_Land | Sequence | 落地 |

#### 武器姿态（静态单帧）

| 资源名称 | 类型 | 描述 |
|---------|------|------|
| Pose_Rifle | Sequence | 步枪持握姿势 |
| Pose_Pistol | Sequence | 手枪持握姿势 |
| Pose_Knife | Sequence | 近战持握姿势 |
| Pose_Unarmed | Sequence | 空手姿势 |

#### 左手握把变体

| 资源名称 | 类型 | 描述 |
|---------|------|------|
| Pose_LeftHand_Default | Sequence | 默认护木握法 |
| Pose_LeftHand_Vertical | Sequence | 垂直握把 |
| Pose_LeftHand_Angled | Sequence | 三角握把 |
| Pose_LeftHand_Akimbo | Sequence | 双持（镜像右手） |

#### 原地转身

| 资源名称 | 类型 | 描述 |
|---------|------|------|
| Turn_Left_90 | Sequence | 左转 90° |
| Turn_Right_90 | Sequence | 右转 90° |
| Turn_Left_180 | Sequence | 左转 180° |
| Turn_Right_180 | Sequence | 右转 180° |

#### 上半身蒙太奇

| 资源名称 | 类型 | 插槽 |
|---------|------|------|
| AM_Reload_Rifle | Montage | UpperBody |
| AM_Reload_Pistol | Montage | UpperBody |
| AM_Fire_Rifle | Montage | Arms |
| AM_Inspect_Weapon | Montage | Arms |

### 8.3 动画曲线需求

在转身动画中需要添加以下曲线：

| 曲线名称 | 用途 |
|---------|------|
| TurnYawCurve | 控制转身时根骨骼的旋转进度 |
| FootLockAlpha | 控制脚部是否锁定（防滑） |

---

## 9. 实施步骤

### Phase 1: 基础架构搭建（1-2 天）

```
□ 创建 C++ 类
   ├── FPSAnimTypes.h (数据结构)
   ├── FPSAnimInterface.h (通信接口)
   ├── UFPSCharacterAnimInstance.h/cpp
   └── UProceduralAnimComponent.h/cpp

□ 创建蓝图资源
   ├── ABP_FPSCharacter (继承自 UFPSCharacterAnimInstance)
   └── ABP_FP_ArmsProcedural (Linked Anim Graph)

□ 配置骨骼
   ├── 添加虚拟骨骼 (VB_Hand_Gun, IK_Hand_Gun, IK_Hand_L)
   └── 验证骨骼层级
```

### Phase 2: 移动系统实现（2-3 天）

```
□ 实现 UpdateLocomotionData()
□ 创建 BlendSpace 资源
□ 搭建 Locomotion Layer
   ├── BlendSpace 连接
   ├── 蹲伏混合
   └── Orientation/Stride Warping
□ 验证滑步修正效果
```

### Phase 3: 武器系统实现（2-3 天）

```
□ 实现 UpdateCombatData()
□ 实现 IFPSAnimInterface
□ 创建武器姿态资源
□ 搭建 Upper Body Layer
   ├── 武器姿态选择
   ├── 上下半身混合
   └── 蒙太奇插槽配置
□ 验证武器切换
```

### Phase 4: 程序化动画实现（3-4 天）

```
□ 实现各效果计算函数
   ├── CalculateSway()
   ├── CalculateBob()
   ├── CalculateRecoil()
   └── CalculateSprintOffset()
□ 搭建 Procedural Transform Layer
□ 调试参数
□ 验证手感
```

### Phase 4.5: 武器配置系统实现（2-3 天）

```
□ 创建 UWeaponProceduralAnimConfig 数据资产类
□ 扩展 IFPSAnimInterface 接口
□ 修改 AnimInstance 支持动态参数读取
   ├── UpdateActiveParametersFromConfig()
   ├── 修改 CalculateSway() 使用活动参数
   ├── 修改 CalculateBob() 使用活动参数
   └── 修改 CalculateRecoil() 支持曲线和累积
□ 创建后坐力曲线资源
□ 为各类武器创建配置数据资产
   ├── DA_Pistol_ProcAnim
   ├── DA_Rifle_ProcAnim
   ├── DA_Sniper_ProcAnim
   ├── DA_SMG_ProcAnim
   └── DA_Shotgun_ProcAnim
□ 集成到 WeaponComponent
□ 验证不同武器手感差异
```

### Phase 5: IK 系统实现（2-3 天）

```
□ 实现左手 IK 数据获取
□ 搭建 Hand IK Layer
   ├── Copy Bone 配置
   ├── 右手 Two Bone IK
   └── 左手 Two Bone IK
□ 验证不同握把适配
□ 验证换弹时 IK 关闭
```

### Phase 6: Turn In Place 实现（2-3 天）

```
□ 实现 UpdateTurnInPlace()
□ 创建转身动画资源
□ 添加动画曲线
□ 配置 Slide 插槽
□ 验证原地转身效果
```

### Phase 7: 优化与打磨（2-3 天）

```
□ 性能分析与优化
□ 参数微调
□ 边界情况处理
□ 文档整理
```

---

## 10. 性能优化建议

### 10.1 代码层面

```cpp
// 1. 使用 FORCEINLINE 内联小函数
FORCEINLINE float CalculateSpeedMultiplier() const
{
    return FMath::Clamp(GroundSpeed / 400.0f, 0.0f, 1.0f);
}

// 2. 避免每帧 Cast，在初始化时缓存
void NativeInitializeAnimation()
{
    CachedCharacter = Cast<AFPSCharacter>(TryGetPawnOwner());
}

// 3. 使用 FMath::IsNearlyZero 替代 == 0
if (FMath::IsNearlyZero(GroundSpeed, 0.1f))
{
    // 静止状态
}

// 4. 条件更新 - 非必要时跳过计算
if (bIsAiming && !bWasAimingLastFrame)
{
    // 只在状态切换时计算
}
```

### 10.2 蓝图层面

| 优化项 | 说明 |
|-------|------|
| 使用 Cached Pose | 避免重复计算相同的动画数据 |
| 合理使用 LOD | 远距离角色简化动画层级 |
| Fast Path | 启用 AnimGraph 的 Fast Path 优化 |
| 减少节点数量 | C++ 预合并多个 Transform，蓝图只用一个节点 |

### 10.3 资源层面

| 优化项 | 说明 |
|-------|------|
| 动画压缩 | 使用合适的压缩设置 |
| 骨骼 LOD | 远距离隐藏手指等细节骨骼 |
| 曲线采样 | 减少不必要的曲线精度 |

---

## 附录 A: 常见问题排查

| 问题 | 可能原因 | 解决方案 |
|-----|---------|---------|
| 手滑出枪 | IK Alpha 配置错误 | 检查 LeftHandIKAlpha 是否正确传递 |
| 滑步 | Stride Warping 未生效 | 检查 GroundSpeed 是否正确 |
| 转身抽搐 | RootYawOffset 更新过快 | 降低插值速度 |
| 瞄准时枪抖动 | Sway 在 ADS 时未减弱 | 根据 AimingAlpha 降低 SwayIntensity |
| 蒙太奇不播放 | Slot 名称不匹配 | 检查 Montage 的 SlotName |

---

## 附录 B: 参考资料

- Epic Games 官方文档: Animation Blueprints
- GDC 2018: Animating The Movement System of The Last of Us Part II
- Unreal Engine 5: Motion Warping Tutorial
- 《使命召唤：现代战争》动画系统分析

---

> **文档版本**: 1.1  
> **最后更新**: 2025年12月24日  
> **更新内容**: 新增第6章"武器程序化动画配置系统"  
> **作者**: AI Assistant  

