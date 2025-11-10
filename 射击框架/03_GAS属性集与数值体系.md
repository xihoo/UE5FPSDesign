# 射击框架 · GAS 属性集与数值体系

将武器相关数值统一由 AttributeSet 管理，并通过 GameplayEffect 动态修改与叠加。

## 属性清单（建议）

- 伤害与射程：BaseDamage, HeadshotMultiplier, Range
- 射速与重装：FireRateRPM, ReloadTime
- 弹药：MagazineSize, ReserveAmmo（CurrentAmmo 变更频繁，可按需加入 Attribute）
- 扩散：BaseSpread, MaxSpread, SpreadIncreasePerShot, SpreadDecreaseSpeed, AimingSpreadMultiplier
- 后坐力：RecoilVerticalMin, RecoilVerticalMax, RecoilHorizontalMin, RecoilHorizontalMax, RecoilRecoverySpeed
- 瞄准：AimSpeed, AimedFOV, AimSensitivityMultiplier

## 示例（节选）

```cpp
UCLASS()
class PROJECTNAME_API UFPSCombatAttributeSet : public UAttributeSet
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, Category="Damage", ReplicatedUsing=OnRep_BaseDamage)
    FGameplayAttributeData BaseDamage;
    UFUNCTION() void OnRep_BaseDamage(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(UFPSCombatAttributeSet, BaseDamage, OldValue); }

    UPROPERTY(BlueprintReadOnly, Category="Weapon", ReplicatedUsing=OnRep_FireRateRPM)
    FGameplayAttributeData FireRateRPM;
    UFUNCTION() void OnRep_FireRateRPM(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(UFPSCombatAttributeSet, FireRateRPM, OldValue); }

    UPROPERTY(BlueprintReadOnly, Category="Recoil", ReplicatedUsing=OnRep_RecoilVerticalMin)
    FGameplayAttributeData RecoilVerticalMin;
    UFUNCTION() void OnRep_RecoilVerticalMin(const FGameplayAttributeData& OldValue) { GAMEPLAYATTRIBUTE_REPNOTIFY(UFPSCombatAttributeSet, RecoilVerticalMin, OldValue); }

    // ... 其余属性同理
};
```

## 基础属性 GE 与附件 GE

- 装备武器时应用 `GE_Weapon_BaseStats`，将武器数据写入属性；卸下时移除
- 附件/被动通过 GE 修改属性（如 `GE_Attachment_MuzzleBrake_Recoil`）

—

下一节：能力与标签 → [04_Abilities与标签设计](./04_Abilities与标签设计.md)
