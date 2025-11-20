# 射击框架 · Abilities 与标签设计

## 能力清单（推荐）
- GA_Weapon_Fire
- GA_Weapon_Reload
- GA_Weapon_Aim
- GA_Weapon_Melee
- GA_Weapon_ThrowGrenade

## 输入与激活路径
InputAction -> GameplayTag(Input.Fire.Primary) -> ASC.TryActivateAbilitiesByTag({Ability.Weapon.Fire.*})

## 标签示例
输入标签：Input.Fire.Primary、Input.Aim、Input.Reload、Input.Weapon.Melee、Input.Skill.Slot.1
状态标签：State.Weapon.Ready、State.Weapon.Reloading、State.Weapon.Aiming、State.Weapon.Overheated
武器分类：Weapon.Rifle、Weapon.Automatic、Weapon.SMG

## GA_Weapon_Fire 契约
- 激活条件：HasTag(State.Weapon.Ready) && Ammo>0 && !HasTag(State.Weapon.Reloading)
- 预测：Started 即本地播放枪口火光/轻微后坐力，服务器确认后校正
- 循环：间隔 = 60 / FireRateRPM；自动武器使用 WaitInputRelease；点射/三连发使用内部循环计数
- 数值：从 AttributeSet 读取 BaseDamage、BaseSpread、Recoil*
- 取消：Input Completed 或弹尽/状态改变

### 示例片段
```cpp
void UGA_Weapon_Fire::DoSingleShot()
{
    const auto* Attr = GetCombatAttributes();
    const float Damage = Attr->GetBaseDamage();
    const float Spread = Attr->GetBaseSpread();
    const float VMin = Attr->GetRecoilVerticalMin();
    const float VMax = Attr->GetRecoilVerticalMax();
    const float HMin = Attr->GetRecoilHorizontalMin();
    const float HMax = Attr->GetRecoilHorizontalMax();

    if (auto* ShooterComp = GetShooterComp())
    {
        ShooterComp->ApplyRecoilFromAttributes(VMin, VMax, HMin, HMax);
    }

    ApplyHitAndDamage(Damage, Spread);
}
```

## 授能与移除
EquipWeapon：授予 Fire/Reload/Aim 等能力 + 应用 GE_Weapon_BaseStats + 添加 State.Weapon.HasWeapon
UnequipWeapon：移除能力 + 移除 GE + 移除标签

—

下一节：数据资产与授能流程 → [05_武器数据资产与授能流程](./05_武器数据资产与授能流程.md)
