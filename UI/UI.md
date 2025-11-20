# UI系统设计文档

## 概述

本文档定义了游戏的用户界面系统，包括HUD、菜单、库存界面、技能界面等。UI系统基于UMG（Unreal Motion Graphics）实现，并与GAS深度集成。

## 设计原则

1. **模块化设计** - UI组件独立可复用
2. **数据绑定** - UI自动响应数据变化
3. **响应式布局** - 支持不同分辨率
4. **性能优化** - 合理的更新频率和对象池
5. **多语言支持** - 使用本地化系统

## 核心架构

### UI组件层次

```
UFPSPlayerHUD (主HUD)
├── 准星
├── 血条/护盾
├── 弹药显示
├── 技能快捷栏
├── 小地图
└── 交互提示

UFPSInventoryWidget (库存UI)
├── 物品网格
├── 装备槽
├── 物品详情
└── 快捷操作

UFPSSkillTreeWidget (技能树UI)
├── 技能节点
├── 连接线
├── 技能详情
└── 技能点显示

UFPSMainMenu (主菜单)
├── 开始游戏
├── 设置
├── 成就
└── 退出
```

---

## 核心Widget

### 1. 玩家HUD

```cpp
UCLASS()
class UFPSPlayerHUD : public UUserWidget
{
    GENERATED_BODY()
    
public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    
    // 更新生命值
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateHealth(float CurrentHealth, float MaxHealth);
    
    // 更新弹药
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateAmmo(int32 CurrentAmmo, int32 ReserveAmmo);
    
    // 显示伤害数字
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowDamageNumber(float Damage, const FVector& WorldLocation, bool bIsCritical);
    
    // 显示交互提示
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowInteractionPrompt(const FText& PromptText, float Progress);
    
protected:
    // Widget绑定
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthBar;
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* AmmoText;
    
    UPROPERTY(meta = (BindWidget))
    UImage* Crosshair;
    
    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* SkillBar;
    
    UPROPERTY(EditDefaultsOnly, Category = "Config")
    TSubclassOf<UUserWidget> DamageNumberClass;
};
```

### 2. 库存界面

```cpp
UCLASS()
class UFPSInventoryWidget : public UUserWidget
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void RefreshInventory();
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void OnItemClicked(UFPSItemInstance* Item);
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void OnItemDropped(UFPSItemInstance* Item, int32 Quantity);
    
protected:
    UPROPERTY(meta = (BindWidget))
    UUniformGridPanel* ItemGrid;
    
    UPROPERTY(meta = (BindWidget))
    UFPSEquipmentSlots* EquipmentSlots;
    
    UPROPERTY(meta = (BindWidget))
    UFPSItemDetails* ItemDetails;
    
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UFPSItemSlot> ItemSlotClass;
    
    void CreateItemSlots();
};
```

---

## UI更新策略

### 事件驱动更新

```cpp
// 在PlayerCharacter中
void AFPSPlayerCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    
    // 绑定属性变化事件
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
            UFPSAttributeSet::GetHealthAttribute()).AddUObject(this, &AFPSPlayerCharacter::OnHealthChanged);
    }
    
    // 绑定库存变化事件
    if (InventoryComponent)
    {
        InventoryComponent->OnItemAdded.AddDynamic(this, &AFPSPlayerCharacter::OnInventoryItemAdded);
    }
}

void AFPSPlayerCharacter::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    if (PlayerHUD)
    {
        PlayerHUD->UpdateHealth(Data.NewValue, GetMaxHealth());
    }
}
```

---

## 相关文档

- [基类设计文档](../基类/基类.md)
- [库存系统文档](../库存/库存.md)
- [技能系统文档](../技能/技能.md)
- [交互系统文档](../交互系统/交互系统.md)
