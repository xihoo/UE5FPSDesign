# 基类设计 · PlayerController 与 PlayerState

## AFPSPlayerController

职责：处理输入、UI 管理、相机控制、交互桥接。

```cpp
UCLASS()
class PROJECTNAME_API AFPSPlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    AFPSPlayerController();
    virtual void SetupInputComponent() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

    UPROPERTY(EditDefaultsOnly, Category="Input") UInputMappingContext* DefaultInputMapping;
    UPROPERTY(EditDefaultsOnly, Category="Input") UInputAction* MoveAction;
    UPROPERTY(EditDefaultsOnly, Category="Input") UInputAction* LookAction;
    UPROPERTY(EditDefaultsOnly, Category="Input") UInputAction* JumpAction;
    UPROPERTY(EditDefaultsOnly, Category="Input") UInputAction* FireAction;
    UPROPERTY(EditDefaultsOnly, Category="Input") UInputAction* AimAction;
    UPROPERTY(EditDefaultsOnly, Category="Input") UInputAction* ReloadAction;
    UPROPERTY(EditDefaultsOnly, Category="Input") UInputAction* InteractAction;

    UFUNCTION(BlueprintCallable, Category="FPS|UI") void ShowPauseMenu();
    UFUNCTION(BlueprintCallable, Category="FPS|UI") void HidePauseMenu();
    UFUNCTION(BlueprintCallable, Category="FPS|UI") void ShowInventory();
    UFUNCTION(BlueprintCallable, Category="FPS|UI") void HideInventory();
    UFUNCTION(BlueprintCallable, Category="FPS|Interaction") void AttemptInteract();
protected:
    void HandleMove(const FInputActionValue& Value);
    void HandleLook(const FInputActionValue& Value);
    void HandleJump();
    void HandleFire(const FInputActionValue& Value);
    void HandleAim(const FInputActionValue& Value);
    void HandleReload();
    void HandleInteract();

    UPROPERTY(BlueprintReadOnly, Category="UI") UFPSPlayerHUD* PlayerHUD;
    UPROPERTY(EditDefaultsOnly, Category="UI") TSubclassOf<UFPSPlayerHUD> PlayerHUDClass;
    UPROPERTY(EditDefaultsOnly, Category="Settings") float BaseLookSensitivity;
    UPROPERTY(EditDefaultsOnly, Category="Settings") float AimingSensitivityMultiplier;
};
```

特性：Enhanced Input 集成、UI 快速接口、灵敏度可配置。

## AFPSPlayerState

职责：玩家统计、货币与进度同步。

```cpp
UCLASS()
class PROJECTNAME_API AFPSPlayerState : public APlayerState
{
    GENERATED_BODY()
public:
    AFPSPlayerState();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintPure) int32 GetKills() const { return Kills; }
    UFUNCTION(BlueprintCallable, meta=(Authority="Server")) void AddKill();
    UFUNCTION(BlueprintPure) int32 GetDeaths() const { return Deaths; }
    UFUNCTION(BlueprintCallable, meta=(Authority="Server")) void AddDeath();
    UFUNCTION(BlueprintPure) float GetDamageDealt() const { return DamageDealt; }
    UFUNCTION(BlueprintCallable, meta=(Authority="Server")) void AddDamageDealt(float Damage);

    UFUNCTION(BlueprintPure, Category="FPS|Currency") int32 GetCurrency() const { return Currency; }
    UFUNCTION(BlueprintCallable, meta=(Authority="Server")) void AddCurrency(int32 Amount);
    UFUNCTION(BlueprintCallable, meta=(Authority="Server")) bool SpendCurrency(int32 Amount);
protected:
    UPROPERTY(Replicated) int32 Kills;
    UPROPERTY(Replicated) int32 Deaths;
    UPROPERTY(Replicated) float DamageDealt;
    UPROPERTY(ReplicatedUsing=OnRep_Currency) int32 Currency;
    UFUNCTION() void OnRep_Currency();
};
```

特性：统计/经济复制、服务端修改、客户端显示。