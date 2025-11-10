# GAS 集成 · 伤害系统与 Execution

## UFPSDamageExecution（GE 自定义执行器）

文件路径：`Source/[ProjectName]/AbilitySystem/Executions/FPSDamageExecution.h/.cpp`

```cpp
UCLASS()
class PROJECTNAME_API UFPSDamageExecution : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()
public:
    UFPSDamageExecution();
    virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                        FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
```

```cpp
UFPSDamageExecution::UFPSDamageExecution()
{
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        UFPSAttributeSet::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true));
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        UFPSAttributeSet::GetAttackPowerAttribute(), EGameplayEffectAttributeCaptureSource::Source, false));
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        UFPSAttributeSet::GetDefenseAttribute(), EGameplayEffectAttributeCaptureSource::Target, false));
}

void UFPSDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                 FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

    FGameplayTagContainer SourceTags; Spec.GetAllAssetTags(SourceTags);
    UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
    UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();

    float BaseDamage = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        FGameplayEffectAttributeCaptureDefinition(UFPSAttributeSet::GetDamageAttribute(),
            EGameplayEffectAttributeCaptureSource::Source, true),
        FAggregatorEvaluateParameters(), BaseDamage);

    float AttackPower = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        FGameplayEffectAttributeCaptureDefinition(UFPSAttributeSet::GetAttackPowerAttribute(),
            EGameplayEffectAttributeCaptureSource::Source, false),
        FAggregatorEvaluateParameters(), AttackPower);
    BaseDamage *= (1.0f + AttackPower / 100.0f);

    bool bIsHeadshot = Spec.GetContext().GetHitResult() &&
                       IsHeadshot(Spec.GetContext().GetHitResult()->BoneName);
    if (bIsHeadshot)
    {
        BaseDamage *= 2.0f;
        OutExecutionOutput.AddOutputTag(FGameplayTag::RequestGameplayTag(FName("Damage.Headshot")));
    }

    float CritChance = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        FGameplayEffectAttributeCaptureDefinition(UFPSAttributeSet::GetCriticalChanceAttribute(),
            EGameplayEffectAttributeCaptureSource::Source, false),
        FAggregatorEvaluateParameters(), CritChance);
    if (FMath::FRandRange(0.0f, 100.0f) < CritChance)
    {
        float CritDamage = 0.0f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
            FGameplayEffectAttributeCaptureDefinition(UFPSAttributeSet::GetCriticalDamageAttribute(),
                EGameplayEffectAttributeCaptureSource::Source, false),
            FAggregatorEvaluateParameters(), CritDamage);
        BaseDamage *= (1.5f + CritDamage / 100.0f);
        OutExecutionOutput.AddOutputTag(FGameplayTag::RequestGameplayTag(FName("Damage.Critical")));
    }

    float Defense = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        FGameplayEffectAttributeCaptureDefinition(UFPSAttributeSet::GetDefenseAttribute(),
            EGameplayEffectAttributeCaptureSource::Target, false),
        FAggregatorEvaluateParameters(), Defense);
    const float DamageReduction = Defense / (Defense + 100.0f);
    BaseDamage *= (1.0f - DamageReduction);

    float ElementalDamage = BaseDamage;
    if (SourceTags.HasTagExact(FGameplayTag::RequestGameplayTag(FName("Element.Fire"))))
    {
        float FireResistance = 0.0f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
            FGameplayEffectAttributeCaptureDefinition(UFPSAttributeSet::GetFireResistanceAttribute(),
                EGameplayEffectAttributeCaptureSource::Target, false),
            FAggregatorEvaluateParameters(), FireResistance);
        ElementalDamage *= (1.0f - FireResistance / 100.0f);
        ApplyElementalStatus(TargetASC, "ElementalStatus.Burning");
    }

    if (ElementalDamage > 0.0f)
    {
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
            UFPSAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -ElementalDamage));
    }
}

bool UFPSDamageExecution::IsHeadshot(const FName& BoneName) const
{
    return BoneName.ToString().Contains(TEXT("head"), ESearchCase::IgnoreCase);
}

void UFPSDamageExecution::ApplyElementalStatus(UAbilitySystemComponent* TargetASC, const FName& StatusTag) const
{
    if (!TargetASC) return;
    FGameplayTag Tag = FGameplayTag::RequestGameplayTag(StatusTag);
    if (Tag.IsValid())
    {
        TargetASC->AddLooseGameplayTag(Tag);
        // TODO: 也可改为应用一个持续 GE
    }
}
```
