# GAS 集成 · Abilities 设计范式

## 射击能力 UFPGA_Fire

文件路径：`Source/[ProjectName]/AbilitySystem/Abilities/FPGA_Fire.h`

```cpp
UCLASS()
class PROJECTNAME_API UFPGA_Fire : public UGameplayAbility
{
    GENERATED_BODY()
public:
    UFPGA_Fire();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
                            const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo,
                            bool bReplicateEndAbility,
                            bool bWasCancelled) override;

    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                    const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayTagContainer* SourceTags,
                                    const FGameplayTagContainer* TargetTags,
                                    FGameplayTagContainer* OptionalRelevantTags) const override;
protected:
    UFUNCTION(BlueprintCallable, Category = "FPS|Fire")
    void PerformFire();

    UFUNCTION(BlueprintCallable, Category = "FPS|Fire")
    void FireProjectile(const FVector& Start, const FVector& Direction);

    UPROPERTY(EditDefaultsOnly, Category = "Config")
    TSubclassOf<UGameplayEffect> DamageEffect;
    UPROPERTY(EditDefaultsOnly, Category = "Config")
    TSubclassOf<UGameplayEffect> AmmoCostEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Config")
    FVector2D RecoilAmount;
    UPROPERTY(EditDefaultsOnly, Category = "Config")
    float FireRate;
    UPROPERTY(EditDefaultsOnly, Category = "Config")
    bool bFullAuto;

    FTimerHandle FireTimerHandle;
};
```

## 换弹能力 UFPGA_Reload

```cpp
UCLASS()
class PROJECTNAME_API UFPGA_Reload : public UGameplayAbility
{
    GENERATED_BODY()
public:
    UFPGA_Reload();
    virtual void ActivateAbility(...) override;
protected:
    UFUNCTION(BlueprintCallable) void StartReload();
    UFUNCTION(BlueprintCallable) void CompleteReload();

    UPROPERTY(EditDefaultsOnly, Category = "Config") float ReloadDuration;
    UPROPERTY(EditDefaultsOnly, Category = "Config") UAnimMontage* ReloadMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Config") TSubclassOf<UGameplayEffect> AmmoRestoreEffect;
};
```

## 技能能力基类 UFPGA_SkillBase

```cpp
UCLASS()
class PROJECTNAME_API UFPGA_SkillBase : public UGameplayAbility
{
    GENERATED_BODY()
public:
    UFPGA_SkillBase();
protected:
    UPROPERTY(EditDefaultsOnly, Category = "Skill") float Cooldown;
    UPROPERTY(EditDefaultsOnly, Category = "Skill") float ResourceCost;
    UPROPERTY(BlueprintReadOnly, Category = "Skill") int32 SkillLevel;
    UPROPERTY(EditDefaultsOnly, Category = "Skill") TSubclassOf<UGameplayEffect> SkillEffect;
};
```
