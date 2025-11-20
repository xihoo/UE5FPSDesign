# 基类设计 · AFPSCharacterBase

职责：可控角色公共基类，集成 GAS 与核心组件。

```cpp
UCLASS()
class PROJECTNAME_API AFPSCharacterBase : public ACharacter
{
    GENERATED_BODY()
public:
    AFPSCharacterBase();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;

    // GAS
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    virtual void InitializeAbilitySystem();
    virtual void GrantDefaultAbilities();

    // 属性访问
    UFUNCTION(BlueprintPure) float GetHealth() const;
    UFUNCTION(BlueprintPure) float GetMaxHealth() const;
    UFUNCTION(BlueprintPure) float GetShield() const;
    UFUNCTION(BlueprintPure) float GetMaxShield() const;
    UFUNCTION(BlueprintPure) float GetMoveSpeed() const;

    // 伤害与死亡
    UFUNCTION(BlueprintCallable) virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo,
        const FGameplayTagContainer& DamageTags, AFPSCharacterBase* InstigatorCharacter, AActor* DamageCauser);
    UFUNCTION(BlueprintCallable) virtual void HandleDeath();
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS") UAbilitySystemComponent* AbilitySystemComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS") UFPSAttributeSet* AttributeSet;

    UPROPERTY(EditDefaultsOnly, Category="GAS|Abilities") TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;
    UPROPERTY(EditDefaultsOnly, Category="GAS|Effects") TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") UFPSInventoryComponent* InventoryComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") UFPSSkillComponent* SkillComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") UFPSFirearmComponent* FirearmComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components") UFPSInteractionComponent* InteractionComponent;

    UPROPERTY(Replicated, BlueprintReadOnly, Category="State") bool bIsDead;
    UPROPERTY(EditDefaultsOnly, Category="Configuration") UFPSCharacterData* CharacterData;
};
```

特性：
- 完整 GAS 集成与默认能力授予。
- 标准属性集与组件组合。
- 统一伤害/死亡处理与复制。