# Gameplay Attributes 与 Tags 设计规划

本文档梳理了项目中现有的 Gameplay Attributes 以及建议新增的 FPS 游戏常用属性，并规定了对应的 GameplayTag 命名规范。

## 1. 命名规范与自动映射机制

本项目使用了 **反射 + 名称匹配** 的自动注册机制 (`UDFPSAttributeRegistry`)。

*   **规则**: GameplayTag 的 **Leaf Name** (最后一个点后面的部分) 必须与 `UDFPSAttributeSet` 中的 **C++ 变量名** 完全一致。
*   **格式**: `Attributes.[Category].[AttributeName]`
*   **示例**: Tag `Attributes.Core.Health` -> 自动映射到变量 `Health`。

---

## 2. 现有属性 (Existing)

这些属性已经在 `UDFPSAttributeSet.h` 中定义。

### 2.1 核心生存 (Core Survival)
| Attribute 变量名 | 对应 GameplayTag | 说明 |
| :--- | :--- | :--- |
| `Health` | `Attributes.Core.Health` | 当前生命值 |
| `MaxHealth` | `Attributes.Core.MaxHealth` | 最大生命值上限 |
| `Shield` | `Attributes.Core.Shield` | 当前护盾值 (优先扣除) |
| `MaxShield` | `Attributes.Core.MaxShield` | 最大护盾值上限 |

### 2.2 耐力系统 (Stamina)
| Attribute 变量名 | 对应 GameplayTag | 说明 |
| :--- | :--- | :--- |
| `Stamina` | `Attributes.Stamina.Stamina` | 当前耐力值 (用于冲刺/技能) |
| `MaxStamina` | `Attributes.Stamina.MaxStamina` | 最大耐力值上限 |
| `StaminaRegenRate` | `Attributes.Stamina.StaminaRegenRate` | 耐力每秒回复量 |

### 2.3 移动能力 (Movement)
| Attribute 变量名 | 对应 GameplayTag | 说明 |
| :--- | :--- | :--- |
| `MoveSpeed` | `Attributes.Movement.MoveSpeed` | 基础移动速度 (cm/s) |

---

## 3. 建议新增属性 (Proposed)

为了丰富 FPS/肉鸽玩法的深度，建议在 `UDFPSAttributeSet` 中扩展以下属性。

### 3.1 生存回复 (Recovery)
| Attribute 变量名 | 对应 GameplayTag | 说明 |
| :--- | :--- | :--- |
| `HealthRegenRate` | `Attributes.Recovery.HealthRegenRate` | 生命值每秒自然回复量 |
| `ShieldRegenRate` | `Attributes.Recovery.ShieldRegenRate` | 护盾每秒回复量 |
| `ShieldRechargeDelay`| `Attributes.Recovery.ShieldRechargeDelay` | 护盾受击后开始回复的延迟时间(秒) |

### 3.2 战斗攻击 (Combat Offense)
| Attribute 变量名 | 对应 GameplayTag | 说明 |
| :--- | :--- | :--- |
| `AttackPower` | `Attributes.Combat.AttackPower` | 全局攻击力倍率 (默认1.0) |
| `CriticalChance` | `Attributes.Combat.CriticalChance` | 暴击几率 (0.0 - 1.0) |
| `CriticalDamage` | `Attributes.Combat.CriticalDamage` | 暴击伤害倍率 (默认1.5) |
| `HeadshotMultiplier` | `Attributes.Combat.HeadshotMultiplier` | 爆头伤害倍率 (默认2.0) |

### 3.3 战斗防御 (Combat Defense)
| Attribute 变量名 | 对应 GameplayTag | 说明 |
| :--- | :--- | :--- |
| `Armor` | `Attributes.Combat.Armor` | 护甲值 (用于计算减伤百分比) |
| `DamageReduction` | `Attributes.Combat.DamageReduction` | 直接伤害减免百分比 (0.0 - 1.0) |

### 3.4 武器专精 (Weapon Proficiency)
| Attribute 变量名 | 对应 GameplayTag | 说明 |
| :--- | :--- | :--- |
| `ReloadSpeed` | `Attributes.Weapon.ReloadSpeed` | 换弹速度倍率 (默认1.0，越高越快) |
| `RecoilControl` | `Attributes.Weapon.RecoilControl` | 后坐力控制 (0.0 - 1.0，越高越稳) |
| `Accuracy` | `Attributes.Weapon.Accuracy` | 射击精准度/散布修正 (默认1.0) |
| `FireRateMultiplier` | `Attributes.Weapon.FireRateMultiplier` | 射速加成倍率 |

---

## 4. 实现步骤

如果要添加上述"建议新增"的属性，请遵循以下步骤：

1.  **修改 C++**: 在 `UDFPSAttributeSet.h` 中添加对应的 `FGameplayAttributeData` 变量，并使用 `ATTRIBUTE_ACCESSORS` 宏。
2.  **添加网络复制**: 在 `GetLifetimeReplicatedProps` 和 `OnRep_` 函数中处理复制逻辑。
3.  **添加 Tag**: 在编辑器的 Project Settings -> GameplayTags 中添加对应的 Tag (或者在 `DefaultGameplayTags.ini` 中配置)。
4.  **自动生效**: 由于使用了反射注册机制，编译并添加 Tag 后，系统会自动识别映射关系，无需修改 `DFPSAttributeRegistry.cpp`。
