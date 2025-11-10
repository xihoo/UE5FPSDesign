# GAS 集成 · AttributeSet 设计

## UFPSAttributeSet - 基础属性集

文件路径：`Source/[ProjectName]/AbilitySystem/FPSAttributeSet.h`

```cpp
UCLASS()
class PROJECTNAME_API UFPSAttributeSet : public UAttributeSet
{
    GENERATED_BODY()
    
public:
    UFPSAttributeSet();
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
    
    // ========== 生命值 ==========
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, Health)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, MaxHealth)
    
    // ========== 护盾值 ==========
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Shield", ReplicatedUsing = OnRep_Shield)
    FGameplayAttributeData Shield;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, Shield)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Shield", ReplicatedUsing = OnRep_MaxShield)
    FGameplayAttributeData MaxShield;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, MaxShield)
    
    // ========== 移动属性 ==========
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Movement", ReplicatedUsing = OnRep_MoveSpeed)
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, MoveSpeed)
    
    // ========== 战斗属性 ==========
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_AttackPower)
    FGameplayAttributeData AttackPower;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, AttackPower)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_Defense)
    FGameplayAttributeData Defense;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, Defense)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_CriticalChance)
    FGameplayAttributeData CriticalChance;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, CriticalChance)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_CriticalDamage)
    FGameplayAttributeData CriticalDamage;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, CriticalDamage)
    
    // ========== 元素抗性 ==========
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Resistance", ReplicatedUsing = OnRep_FireResistance)
    FGameplayAttributeData FireResistance;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, FireResistance)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Resistance", ReplicatedUsing = OnRep_IceResistance)
    FGameplayAttributeData IceResistance;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, IceResistance)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Resistance", ReplicatedUsing = OnRep_ElectricResistance)
    FGameplayAttributeData ElectricResistance;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, ElectricResistance)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Resistance", ReplicatedUsing = OnRep_PoisonResistance)
    FGameplayAttributeData PoisonResistance;
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, PoisonResistance)
    
    // ========== Meta 属性（不复制，仅用于计算） ==========
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Meta")
    FGameplayAttributeData Damage; // 临时伤害值，用于计算
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, Damage)
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Meta")
    FGameplayAttributeData Healing; // 临时治疗值
    ATTRIBUTE_ACCESSORS(UFPSAttributeSet, Healing)
    
protected:
    // RepNotify
    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldHealth);
    UFUNCTION()
    void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
    UFUNCTION()
    void OnRep_Shield(const FGameplayAttributeData& OldShield);
    UFUNCTION()
    void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield);
    UFUNCTION()
    void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);
    // ... 其他 RepNotify ...

    // 辅助
    void AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
                                     const FGameplayAttributeData& MaxAttribute,
                                     float NewMaxValue,
                                     const FGameplayAttribute& AffectedAttributeProperty);
};
```
