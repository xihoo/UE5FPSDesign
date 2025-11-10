# Enhanced Input 系统设计文档

## 概述

本文档定义了基于UE5 Enhanced Input Plugin的输入系统架构。Enhanced Input系统提供了更灵活的输入处理方式，支持输入映射上下文、修饰键、触发器等高级特性，适合复杂的FPS游戏输入需求。

## 设计原则

1. **上下文驱动** - 使用Input Mapping Context根据游戏状态动态切换输入映射
2. **数据驱动配置** - 输入配置通过DataAsset管理，便于调整和扩展
3. **修饰键支持** - 支持复杂的组合键操作（Ctrl+X, Shift+Space等）
4. **触发器系统** - 使用触发器定义输入激活条件（按下、释放、长按、双击等）
5. **多设备支持** - 统一处理键鼠、手柄等多种输入设备

## Enhanced Input 核心概念

### Input Action（输入动作）
定义一个输入行为的抽象表示，与具体按键解耦。

### Input Mapping Context（输入映射上下文）
定义一组Input Action到具体按键的映射关系，可以根据游戏状态动态添加/移除。

### Modifiers（修饰器）
在输入值传递到游戏逻辑前对其进行修改（如死区处理、平滑、缩放等）。

### Triggers（触发器）
定义输入动作何时被触发（按下、释放、长按、双击等）。

---

## 输入动作定义

### 1. 移动相关

#### IA_Move - 角色移动
```cpp
类型: Value (Vector2D)
描述: WASD键控制角色前后左右移动

映射:
- Keyboard: W/S/A/D
- Gamepad: Left Stick

修饰器:
- Swizzle Input Axis Values (Y, X, Z) - 交换XY轴

触发器:
- None (持续输入)
```

#### IA_Jump - 跳跃
```cpp
类型: Button
描述: 控制角色跳跃

映射:
- Keyboard: Space
- Gamepad: A Button (Xbox) / X Button (PlayStation)

触发器:
- Pressed - 按下时触发
```

#### IA_Crouch - 蹲下
```cpp
类型: Button
描述: 切换蹲伏状态

映射:
- Keyboard: C / Left Ctrl
- Gamepad: B Button (Xbox) / Circle Button (PlayStation)

触发器:
- Pressed - 按下时切换
```

#### IA_Sprint - 冲刺
```cpp
类型: Button
描述: 冲刺奔跑

映射:
- Keyboard: Left Shift
- Gamepad: Left Stick Click (L3)

触发器:
- Down - 按住时持续冲刺
```

---

### 2. 视角相关

#### IA_Look - 视角控制
```cpp
类型: Value (Vector2D)
描述: 鼠标/右摇杆控制视角

映射:
- Mouse: Mouse XY Movement
- Gamepad: Right Stick

修饰器:
- Negate (根据反转设置)
- Scalar (应用灵敏度)
- Smooth Delta (平滑输入，可选)

触发器:
- None (持续输入)
```

---

### 3. 战斗相关

#### IA_Fire - 开火
```cpp
类型: Button
描述: 射击武器

映射:
- Mouse: Left Button
- Gamepad: Right Trigger

触发器:
- Down - 按住持续射击（自动武器）
- Pressed - 单次射击（单发武器）

修饰器:
- 无（武器类型决定触发模式）
```

#### IA_Aim - 瞄准
```cpp
类型: Button
描述: 进入瞄准状态

映射:
- Mouse: Right Button
- Gamepad: Left Trigger

触发器:
- Down - 按住瞄准
```

#### IA_Reload - 换弹
```cpp
类型: Button
描述: 重新装填弹药

映射:
- Keyboard: R
- Gamepad: X Button (Xbox) / Square Button (PlayStation)

触发器:
- Pressed - 按下触发
```

#### IA_SwitchWeapon - 切换武器
```cpp
类型: Value (Axis1D)
描述: 切换主副武器

映射:
- Keyboard: 1, 2, 3, 4 (数字键)
- Mouse: Mouse Wheel Up/Down
- Gamepad: D-Pad Up/Down

触发器:
- Pressed - 按下切换
```

#### IA_ThrowGrenade - 投掷手雷
```cpp
类型: Button
描述: 投掷手雷或其他投掷物

映射:
- Keyboard: G
- Gamepad: Right Bumper

触发器:
- Pressed - 按下触发
- Hold (可选) - 长按蓄力投掷
```

#### IA_Melee - 近战攻击
```cpp
类型: Button
描述: 近战攻击

映射:
- Keyboard: V / Mouse Button 4
- Gamepad: Right Stick Click (R3)

触发器:
- Pressed - 按下触发
```

---

### 4. 交互相关

#### IA_Interact - 交互
```cpp
类型: Button
描述: 与环境物体交互

映射:
- Keyboard: E / F
- Gamepad: Y Button (Xbox) / Triangle Button (PlayStation)

触发器:
- Pressed - 按下触发
```

#### IA_Use - 使用物品
```cpp
类型: Button
描述: 使用库存物品或技能

映射:
- Keyboard: Q
- Gamepad: Left Bumper

触发器:
- Pressed - 按下触发
```

---

### 5. UI相关

#### IA_OpenInventory - 打开库存
```cpp
类型: Button
描述: 打开/关闭库存界面

映射:
- Keyboard: Tab / I
- Gamepad: Back/Select Button

触发器:
- Pressed - 按下切换
```

#### IA_Pause - 暂停菜单
```cpp
类型: Button
描述: 打开暂停菜单

映射:
- Keyboard: Escape
- Gamepad: Start/Options Button

触发器:
- Pressed - 按下切换
```

#### IA_ScoreBoard - 计分板
```cpp
类型: Button
描述: 显示/隐藏计分板

映射:
- Keyboard: Tab (Hold)
- Gamepad: Back/Select Button (Hold)

触发器:
- Down - 按住显示
```

---

### 6. 技能相关

#### IA_Skill1~4 - 技能快捷键
```cpp
类型: Button
描述: 触发技能栏技能

映射:
- Keyboard: 1, 2, 3, 4 (带修饰键区分武器切换)
- Gamepad: D-Pad Left/Right/Up/Down

触发器:
- Pressed - 按下触发
```

---

## Input Mapping Context 设计

### IMC_Default - 默认游戏内上下文

**优先级**：0（基础）

**用途**：包含所有基础游戏内输入

**包含的Input Actions**：
- IA_Move
- IA_Look
- IA_Jump
- IA_Crouch
- IA_Sprint
- IA_Fire
- IA_Aim
- IA_Reload
- IA_SwitchWeapon
- IA_ThrowGrenade
- IA_Melee
- IA_Interact
- IA_Use
- IA_Skill1~4
- IA_OpenInventory
- IA_Pause
- IA_ScoreBoard

---

### IMC_UI - UI交互上下文

**优先级**：1（覆盖游戏内输入）

**用途**：当打开菜单或UI界面时激活

**包含的Input Actions**：
- IA_Navigate（UI导航）
- IA_Confirm（确认）
- IA_Cancel（取消）
- IA_CloseUI（关闭界面）

**特性**：
- 激活时阻止游戏内输入
- 处理菜单导航
- ESC键关闭界面

---

### IMC_Vehicle - 载具控制上下文（预留）

**优先级**：1

**用途**：未来如果添加载具系统使用

**特性**：
- 重新映射移动输入到载具控制
- 添加载具特有的输入（切换视角、武器等）

---

## 代码实现

### 1. PlayerController输入设置

**文件路径**：`Source/[ProjectName]/Core/FPSPlayerController.h`

```cpp
UCLASS()
class PROJECTNAME_API AFPSPlayerController : public APlayerController
{
    GENERATED_BODY()
    
public:
    AFPSPlayerController();
    
protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    
    // ========== Input Mapping Contexts ==========
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Contexts")
    UInputMappingContext* DefaultMappingContext;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Contexts")
    UInputMappingContext* UIMappingContext;
    
    // ========== Input Actions - Movement ==========
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Movement")
    UInputAction* IA_Move;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Movement")
    UInputAction* IA_Look;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Movement")
    UInputAction* IA_Jump;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Movement")
    UInputAction* IA_Crouch;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Movement")
    UInputAction* IA_Sprint;
    
    // ========== Input Actions - Combat ==========
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Combat")
    UInputAction* IA_Fire;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Combat")
    UInputAction* IA_Aim;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Combat")
    UInputAction* IA_Reload;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Combat")
    UInputAction* IA_SwitchWeapon;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Combat")
    UInputAction* IA_ThrowGrenade;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Combat")
    UInputAction* IA_Melee;
    
    // ========== Input Actions - Interaction ==========
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Interaction")
    UInputAction* IA_Interact;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Interaction")
    UInputAction* IA_Use;
    
    // ========== Input Actions - UI ==========
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|UI")
    UInputAction* IA_OpenInventory;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|UI")
    UInputAction* IA_Pause;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|UI")
    UInputAction* IA_ScoreBoard;
    
    // ========== Input Actions - Skills ==========
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Skills")
    UInputAction* IA_Skill1;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Skills")
    UInputAction* IA_Skill2;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Skills")
    UInputAction* IA_Skill3;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input|Actions|Skills")
    UInputAction* IA_Skill4;
    
private:
    // ========== Input Callbacks ==========
    
    // Movement
    void Input_Move(const FInputActionValue& Value);
    void Input_Look(const FInputActionValue& Value);
    void Input_Jump();
    void Input_Crouch();
    void Input_Sprint_Started();
    void Input_Sprint_Completed();
    
    // Combat
    void Input_Fire_Started();
    void Input_Fire_Completed();
    void Input_Aim_Started();
    void Input_Aim_Completed();
    void Input_Reload();
    void Input_SwitchWeapon(const FInputActionValue& Value);
    void Input_ThrowGrenade();
    void Input_Melee();
    
    // Interaction
    void Input_Interact();
    void Input_Use();
    
    // UI
    void Input_OpenInventory();
    void Input_Pause();
    void Input_ScoreBoard_Started();
    void Input_ScoreBoard_Completed();
    
    // Skills
    void Input_Skill1();
    void Input_Skill2();
    void Input_Skill3();
    void Input_Skill4();
    
public:
    // Context管理
    UFUNCTION(BlueprintCallable, Category = "Input")
    void AddInputMappingContext(UInputMappingContext* Context, int32 Priority);
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    void RemoveInputMappingContext(UInputMappingContext* Context);
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    void SwitchToUIMode();
    
    UFUNCTION(BlueprintCallable, Category = "Input")
    void SwitchToGameplayMode();
};
```

---

### 2. PlayerController输入实现

**文件路径**：`Source/[ProjectName]/Core/FPSPlayerController.cpp`

```cpp
#include "FPSPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

AFPSPlayerController::AFPSPlayerController()
{
    // 构造函数
}

void AFPSPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    // 添加默认输入映射上下文
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (DefaultMappingContext)
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void AFPSPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    // 转换为Enhanced Input Component
    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
    if (!EnhancedInputComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("Enhanced Input Component not found!"));
        return;
    }
    
    // ========== 绑定移动输入 ==========
    
    EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AFPSPlayerController::Input_Move);
    EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AFPSPlayerController::Input_Look);
    EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Jump);
    EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Crouch);
    EnhancedInputComponent->BindAction(IA_Sprint, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Sprint_Started);
    EnhancedInputComponent->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &AFPSPlayerController::Input_Sprint_Completed);
    
    // ========== 绑定战斗输入 ==========
    
    EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Fire_Started);
    EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Completed, this, &AFPSPlayerController::Input_Fire_Completed);
    EnhancedInputComponent->BindAction(IA_Aim, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Aim_Started);
    EnhancedInputComponent->BindAction(IA_Aim, ETriggerEvent::Completed, this, &AFPSPlayerController::Input_Aim_Completed);
    EnhancedInputComponent->BindAction(IA_Reload, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Reload);
    EnhancedInputComponent->BindAction(IA_SwitchWeapon, ETriggerEvent::Started, this, &AFPSPlayerController::Input_SwitchWeapon);
    EnhancedInputComponent->BindAction(IA_ThrowGrenade, ETriggerEvent::Started, this, &AFPSPlayerController::Input_ThrowGrenade);
    EnhancedInputComponent->BindAction(IA_Melee, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Melee);
    
    // ========== 绑定交互输入 ==========
    
    EnhancedInputComponent->BindAction(IA_Interact, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Interact);
    EnhancedInputComponent->BindAction(IA_Use, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Use);
    
    // ========== 绑定UI输入 ==========
    
    EnhancedInputComponent->BindAction(IA_OpenInventory, ETriggerEvent::Started, this, &AFPSPlayerController::Input_OpenInventory);
    EnhancedInputComponent->BindAction(IA_Pause, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Pause);
    EnhancedInputComponent->BindAction(IA_ScoreBoard, ETriggerEvent::Started, this, &AFPSPlayerController::Input_ScoreBoard_Started);
    EnhancedInputComponent->BindAction(IA_ScoreBoard, ETriggerEvent::Completed, this, &AFPSPlayerController::Input_ScoreBoard_Completed);
    
    // ========== 绑定技能输入 ==========
    
    EnhancedInputComponent->BindAction(IA_Skill1, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Skill1);
    EnhancedInputComponent->BindAction(IA_Skill2, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Skill2);
    EnhancedInputComponent->BindAction(IA_Skill3, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Skill3);
    EnhancedInputComponent->BindAction(IA_Skill4, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Skill4);
}

// ========== 移动输入实现 ==========

void AFPSPlayerController::Input_Move(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();
    
    if (APawn* ControlledPawn = GetPawn())
    {
        // 转发给角色
        if (AFPSPlayerCharacter* PlayerCharacter = Cast<AFPSPlayerCharacter>(ControlledPawn))
        {
            PlayerCharacter->Move(MovementVector);
        }
    }
}

void AFPSPlayerController::Input_Look(const FInputActionValue& Value)
{
    const FVector2D LookAxisVector = Value.Get<FVector2D>();
    
    if (APawn* ControlledPawn = GetPawn())
    {
        if (AFPSPlayerCharacter* PlayerCharacter = Cast<AFPSPlayerCharacter>(ControlledPawn))
        {
            PlayerCharacter->Look(LookAxisVector);
        }
    }
}

void AFPSPlayerController::Input_Jump()
{
    if (APawn* ControlledPawn = GetPawn())
    {
        if (ACharacter* Character = Cast<ACharacter>(ControlledPawn))
        {
            Character->Jump();
        }
    }
}

// ... 其他输入实现 ...

// ========== Context管理 ==========

void AFPSPlayerController::AddInputMappingContext(UInputMappingContext* Context, int32 Priority)
{
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(Context, Priority);
    }
}

void AFPSPlayerController::RemoveInputMappingContext(UInputMappingContext* Context)
{
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->RemoveMappingContext(Context);
    }
}

void AFPSPlayerController::SwitchToUIMode()
{
    // 添加UI上下文，阻止游戏输入
    AddInputMappingContext(UIMappingContext, 1);
    
    // 显示鼠标光标
    SetShowMouseCursor(true);
    SetInputMode(FInputModeUIOnly());
}

void AFPSPlayerController::SwitchToGameplayMode()
{
    // 移除UI上下文
    RemoveInputMappingContext(UIMappingContext);
    
    // 隐藏鼠标光标
    SetShowMouseCursor(false);
    SetInputMode(FInputModeGameOnly());
}
```

---

## 输入配置最佳实践

### 1. 灵敏度设置

```cpp
// 在PlayerController中实现
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Settings")
float BaseLookSensitivity = 1.0f;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Settings")
float AimingSensitivityMultiplier = 0.5f;

void AFPSPlayerController::Input_Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    
    // 应用灵敏度
    float CurrentSensitivity = BaseLookSensitivity;
    if (bIsAiming)
    {
        CurrentSensitivity *= AimingSensitivityMultiplier;
    }
    
    LookAxisVector *= CurrentSensitivity;
    
    // 应用到角色
    if (AFPSPlayerCharacter* PlayerCharacter = Cast<AFPSPlayerCharacter>(GetPawn()))
    {
        PlayerCharacter->Look(LookAxisVector);
    }
}
```

---

### 2. 死区处理

在Input Mapping Context中为摇杆输入添加死区修饰器：

```
Modifiers:
- Dead Zone (0.25) - 小于25%的输入被忽略
- Exponential (2.0) - 指数曲线，提供更精细的控制
```

---

### 3. 平滑输入

```cpp
// 在Modifiers中添加
- Smooth Delta
  - Speed: 10.0
  - Type: Interp To
```

---

### 4. 输入缓冲

对于需要连招或快速输入的情况，可以实现输入缓冲：

```cpp
// 在PlayerController中
TQueue<FInputCommand> InputBuffer;
float InputBufferWindow = 0.2f; // 200ms缓冲窗口

void AFPSPlayerController::BufferInput(FInputCommand Command)
{
    Command.Timestamp = GetWorld()->GetTimeSeconds();
    InputBuffer.Enqueue(Command);
}

void AFPSPlayerController::ProcessInputBuffer()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    FInputCommand Command;
    
    while (InputBuffer.Dequeue(Command))
    {
        if (CurrentTime - Command.Timestamp <= InputBufferWindow)
        {
            // 执行输入
            ExecuteCommand(Command);
        }
    }
}
```

---

## 多设备适配

### 键鼠设置
- 提供可调节的鼠标灵敏度
- 支持DPI切换
- 可选的鼠标加速度
- Y轴反转选项

### 手柄设置
- 摇杆死区调整
- 摇杆响应曲线（线性/指数）
- 振动反馈强度
- 瞄准辅助（可选）

### 输入图标显示
根据当前使用的输入设备动态显示对应的按键图标：

```cpp
UFUNCTION(BlueprintPure, Category = "Input")
EInputDeviceType GetCurrentInputDevice() const;

UFUNCTION(BlueprintPure, Category = "Input")
UTexture2D* GetInputIcon(UInputAction* Action) const;
```

---

## 可访问性支持

### 1. 按键重映射
允许玩家自定义所有按键绑定。

### 2. 单手模式
提供预设的单手操作方案。

### 3. 自动运行
长按前进键后自动保持前进。

### 4. 切换 vs 长按
为瞄准、蹲下等动作提供切换和长按两种模式选择。

---

## 网络同步

### 输入预测
客户端本地处理输入，预测移动结果：

```cpp
// 在Character中实现
void AFPSPlayerCharacter::ServerMove_Implementation(FVector_NetQuantize10 Location, 
    FRotator Rotation, float Timestamp)
{
    // 服务器验证移动
}
```

### 输入RPC
关键输入动作需要通过RPC发送到服务器：

```cpp
UFUNCTION(Server, Reliable)
void ServerFire();

UFUNCTION(Server, Reliable)
void ServerReload();

UFUNCTION(Server, Reliable)
void ServerInteract(AActor* InteractTarget);
```

---

## 调试工具

### 输入可视化
```cpp
UFUNCTION(Exec)
void ShowInputDebug(bool bShow);
```

显示当前激活的Input Actions和输入值。

### 输入日志
```cpp
#define INPUT_LOG(Format, ...) \
    UE_LOG(LogInput, Log, TEXT(Format), ##__VA_ARGS__)
    
INPUT_LOG("Fire Input Received - Value: %f", FireValue);
```

---

## 相关文档

- [基类设计文档](../基类/基类.md)
- [射击框架文档](../射击框架/射击框架.md)
- [UI系统文档](../UI/UI.md)
