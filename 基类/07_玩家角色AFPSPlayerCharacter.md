# 基类设计 · 玩家角色 AFPSPlayerCharacter

职责：玩家控制的角色，扩展基类功能（第一人称视角、武器交互、技能快捷）。

```cpp
UCLASS()
class PROJECTNAME_API AFPSPlayerCharacter : public AFPSCharacterBase
{
    GENERATED_BODY()
public:
    AFPSPlayerCharacter();
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void PossessedBy(AController* NewController) override;

    // 相机
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera") USpringArmComponent* CameraBoom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera") UCameraComponent* FollowCamera;

    // Shooter Pawn 拓展
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") UFPSShooterPawnComponent* ShooterPawnComponent;

    // 输入接口（增强输入适配）
    UFUNCTION(BlueprintCallable) void Move(const FVector2D& MovementVector);
    UFUNCTION(BlueprintCallable) void Look(const FVector2D& LookAxisVector);

    // 战斗接口
    UFUNCTION(BlueprintCallable) void StartFire();
    UFUNCTION(BlueprintCallable) void StopFire();
    UFUNCTION(BlueprintCallable) void StartAiming();
    UFUNCTION(BlueprintCallable) void StopAiming();
    UFUNCTION(BlueprintCallable) void Reload();

    // 交互
    UFUNCTION(BlueprintCallable) void Interact();

    // 技能使用
    UFUNCTION(BlueprintCallable) void UseSkill(int32 SkillIndex);
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mesh") USkeletalMeshComponent* FirstPersonMesh;
    UPROPERTY(Replicated, BlueprintReadOnly, Category="Weapon") AFPSWeapon* CurrentWeapon;
};
```

特性：
- 第一人称/第三人称混合视角支持。
- 与武器组件/Fire Ability 对接。
- 技能快捷调用与输入事件桥接。