# 基类设计 · AI 角色与 AI 控制器

## AFPSAICharacter

职责：AI 控制的敌人角色，包含行为状态、仇恨与掉落系统。

```cpp
UENUM(BlueprintType)
enum class EFPSAIBehavior : uint8 { Idle, Patrol, Alert, Chase, Attack, Retreat };

UCLASS()
class PROJECTNAME_API AFPSAICharacter : public AFPSCharacterBase
{
    GENERATED_BODY()
public:
    AFPSAICharacter();

    UFUNCTION(BlueprintCallable) void SetAIBehavior(EFPSAIBehavior NewBehavior);
    UFUNCTION(BlueprintPure) EFPSAIBehavior GetAIBehavior() const { return CurrentAIBehavior; }

    UFUNCTION(BlueprintCallable) void SetTargetActor(AActor* NewTarget);
    UFUNCTION(BlueprintPure) AActor* GetTargetActor() const { return TargetActor; }

    UFUNCTION(BlueprintCallable) void AddThreat(AActor* ThreatActor, float ThreatAmount);
    UFUNCTION(BlueprintPure) AActor* GetHighestThreatTarget() const;

    UFUNCTION(BlueprintCallable, Category="FPS|Loot") void SpawnLoot();
protected:
    UPROPERTY(Replicated, BlueprintReadOnly, Category="AI") EFPSAIBehavior CurrentAIBehavior;
    UPROPERTY(Replicated, BlueprintReadOnly, Category="AI") AActor* TargetActor;
    UPROPERTY(BlueprintReadOnly, Category="AI") TMap<AActor*, float> ThreatTable;
    UPROPERTY(EditDefaultsOnly, Category="Configuration") UFPSAICharacterData* AICharacterData;
    UPROPERTY(EditDefaultsOnly, Category="Loot") UFPSLootTable* LootTable;
};
```

要点：
- 仇恨表 ThreatTable 维护动态威胁值。
- 行为枚举驱动 BT/黑板条件切换。
- LootTable 数据驱动掉落。

## AFPSAIController

职责：决策层，封装行为树、黑板与感知。

```cpp
UCLASS()
class PROJECTNAME_API AFPSAIController : public AAIController
{
    GENERATED_BODY()
public:
    AFPSAIController();
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditDefaultsOnly, Category="AI") UBehaviorTree* BehaviorTree;
    UPROPERTY(BlueprintReadOnly, Category="AI") UBlackboardComponent* BlackboardComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI") UAIPerceptionComponent* AIPerceptionComponent;

    UFUNCTION() void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    UFUNCTION(BlueprintCallable) void StartAttacking();
    UFUNCTION(BlueprintCallable) void StopAttacking();
    UFUNCTION(BlueprintCallable) bool CanSeeTarget() const;
protected:
    void InitializeBlackboard();
    void UpdateAIState();
    UPROPERTY(EditDefaultsOnly, Category="AI|Perception") float SightRadius;
    UPROPERTY(EditDefaultsOnly, Category="AI|Perception") float LoseSightRadius;
    UPROPERTY(EditDefaultsOnly, Category="AI|Perception") float PeripheralVisionAngleDegrees;
};
```

要点：
- Perception 回调驱动目标刷新与威胁更新。
- 黑板键与行为树任务需统一命名规范（后续扩展）。
- Tick 中可选轻量状态监测（避免昂贵查询）。