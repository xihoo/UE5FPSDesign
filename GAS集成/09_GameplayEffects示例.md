# GAS 集成 · Gameplay Effects 示例

## 伤害效果（Instant）

```cpp
// C++ 构建或蓝图 DataAsset 配置
FGameplayEffectSpec* DamageSpec = new FGameplayEffectSpec();
DamageSpec->Def = DamageEffectClass;
DamageSpec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Damage"), DamageValue);
DamageSpec->Duration = EGameplayEffectDurationType::Instant;
TargetASC->ApplyGameplayEffectSpecToSelf(*DamageSpec);
```

## 燃烧 DoT（Periodic）

```
Effect Name: GE_Burning
Duration: 5 秒
Period: 1 秒（每秒触发一次）
Modifiers:
  - Attribute: Health
  - Operation: Add
  - Magnitude: -10（每秒10点伤害）
Tags:
  - ElementalStatus.Burning
  - Buff.Negative
```

## 移速 Buff（Duration）

```
Effect Name: GE_SpeedBoost
Duration: 10 秒
Modifiers:
  - Attribute: MoveSpeed
  - Operation: Multiply
  - Magnitude: 1.5（150% 移速）
Tags:
  - Buff.Positive
```
