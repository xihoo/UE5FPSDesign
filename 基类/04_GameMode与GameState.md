# 基类设计 · GameMode 与 GameState

## AFPSGameMode

职责：定义游戏规则、管理游戏流程、控制回合/波次逻辑。

```cpp
UCLASS()
class PROJECTNAME_API AFPSGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AFPSGameMode();
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    virtual void StartPlay() override;
    virtual void RestartPlayer(AController* NewPlayer) override;

    UFUNCTION(BlueprintCallable, Category="FPS|GameMode") void StartDungeonGeneration();
    UFUNCTION(BlueprintCallable, Category="FPS|GameMode") void OnDungeonGenerationComplete();
    UFUNCTION(BlueprintCallable, Category="FPS|GameMode") void StartWave(int32 WaveNumber);
    UFUNCTION(BlueprintCallable, Category="FPS|GameMode") void EndWave();
    UFUNCTION(BlueprintCallable, Category="FPS|GameMode") void RespawnPlayer(AController* Controller);
    UFUNCTION(BlueprintCallable, Category="FPS|GameMode") void OnPlayerDeath(AFPSPlayerCharacter* DeadPlayer);
    UFUNCTION(BlueprintCallable, Category="FPS|GameMode") void SpawnEnemyWave(int32 WaveNumber);
    UFUNCTION(BlueprintCallable, Category="FPS|GameMode") void OnEnemyKilled(AFPSAICharacter* KilledEnemy);
protected:
    UPROPERTY(BlueprintReadOnly, Category="Game State") int32 CurrentWave;
    UPROPERTY(BlueprintReadOnly, Category="Game State") int32 AliveEnemyCount;
    UPROPERTY(EditDefaultsOnly, Category="Configuration") UFPSGameModeConfig* GameModeConfig;
    UPROPERTY(BlueprintReadOnly, Category="Dungeon") ADungeonGenerator* DungeonGenerator;
};
```

特性：地牢生成调度、波次与敌人生命周期管理、玩家重生策略。

## AFPSGameState

职责：同步全局游戏状态，供所有客户端查询。

```cpp
UENUM(BlueprintType)
enum class EFPSGamePhase : uint8
{ WaitingToStart, DungeonGeneration, InProgress, WaveComplete, Victory, Defeat };

UCLASS()
class PROJECTNAME_API AFPSGameState : public AGameStateBase
{
    GENERATED_BODY()
public:
    AFPSGameState();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintPure) EFPSGamePhase GetCurrentGamePhase() const { return CurrentGamePhase; }
    UFUNCTION(BlueprintCallable, meta=(Authority="Server")) void SetCurrentGamePhase(EFPSGamePhase NewPhase);
    UFUNCTION(BlueprintPure) int32 GetCurrentWave() const { return CurrentWave; }
    UFUNCTION(BlueprintPure) int32 GetRemainingEnemies() const { return RemainingEnemies; }
    UFUNCTION(BlueprintPure) int32 GetTeamKills() const { return TeamKills; }
    UFUNCTION(BlueprintCallable, meta=(Authority="Server")) void AddTeamKill();
protected:
    UPROPERTY(ReplicatedUsing=OnRep_CurrentGamePhase) EFPSGamePhase CurrentGamePhase;
    UFUNCTION() void OnRep_CurrentGamePhase();
    UPROPERTY(Replicated) int32 CurrentWave;
    UPROPERTY(Replicated) int32 RemainingEnemies;
    UPROPERTY(Replicated) int32 TeamKills;
    UPROPERTY(Replicated) float GameStartTime;
};
```

特性：Phase 变化通知、波次与团队统计复制、客户端只读访问接口。