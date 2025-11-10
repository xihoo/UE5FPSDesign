# GAS 集成 · Gameplay Tags 体系

## 标签结构

```
GameplayTags/
├── Ability/                        # 能力相关
│   ├── Ability.Skill              # 技能类能力
│   ├── Ability.Weapon             # 武器相关能力
│   │   ├── Ability.Weapon.Fire    # 射击
│   │   ├── Ability.Weapon.Reload  # 换弹
│   │   └── Ability.Weapon.Aim     # 瞄准
│   └── Ability.Interaction        # 交互能力
│
├── State/                         # 状态标签
│   ├── State.Dead                 # 死亡状态
│   ├── State.Stunned              # 眩晕状态
│   ├── State.Knockdown            # 击倒状态
│   ├── State.Reloading            # 换弹中
│   ├── State.Aiming               # 瞄准中
│   └── State.Sprinting            # 冲刺中
│
├── Element/                       # 元素类型
│   ├── Element.Fire               # 火
│   ├── Element.Ice                # 冰
│   ├── Element.Electric           # 雷
│   ├── Element.Poison             # 毒
│   └── Element.Physical           # 物理
│
├── ElementalStatus/               # 元素状态
│   ├── ElementalStatus.Burning    # 燃烧
│   ├── ElementalStatus.Frozen     # 冰冻
│   ├── ElementalStatus.Shocked    # 感电
│   └── ElementalStatus.Poisoned   # 中毒
│
├── Damage/                        # 伤害类型
│   ├── Damage.Basic               # 基础伤害
│   ├── Damage.Critical            # 暴击伤害
│   ├── Damage.Headshot            # 爆头伤害
│   └── Damage.Elemental           # 元素伤害
│
├── Buff/                          # Buff类型
│   ├── Buff.Positive              # 正面Buff
│   └── Buff.Negative              # 负面Buff
│
└── Event/                         # 事件标签
    ├── Event.Death                # 死亡事件
    ├── Event.Respawn              # 重生事件
    ├── Event.LevelUp              # 升级事件
    └── Event.ElementalReaction    # 元素反应事件
```
