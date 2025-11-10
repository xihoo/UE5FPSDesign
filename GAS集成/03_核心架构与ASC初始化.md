# GAS 集成 · 核心架构与 ASC 初始化

## Ability System Component 配置

```cpp
// 在 AFPSCharacterBase 中
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
UAbilitySystemComponent* AbilitySystemComponent;

// 服务器端初始化（PossessedBy）
void AFPSCharacterBase::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
        InitializeAbilitySystem();
    }
}

// 客户端初始化（OnRep_PlayerState）
void AFPSCharacterBase::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    
    if (AbilitySystemComponent && InputComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}
```
