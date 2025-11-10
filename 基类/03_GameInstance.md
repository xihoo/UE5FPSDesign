# 基类设计 · GameInstance

## UFPSGameInstance

职责：管理游戏全局状态、持久化数据、跨关卡数据传递。

头文件路径：`Source/[ProjectName]/Core/FPSGameInstance.h`

```cpp
UCLASS()
class PROJECTNAME_API UFPSGameInstance : public UGameInstance
{
    GENERATED_BODY()
public:
    UFPSGameInstance();
    virtual void Init() override;
    virtual void Shutdown() override;

    UFUNCTION(BlueprintCallable, Category = "FPS|GameInstance") void SavePlayerProgress();
    UFUNCTION(BlueprintCallable, Category = "FPS|GameInstance") void LoadPlayerProgress();

    UFUNCTION(BlueprintCallable, Category = "FPS|GameInstance") void ApplyGameSettings(const FFPSGameSettings& Settings);
    UFUNCTION(BlueprintPure, Category = "FPS|GameInstance") FFPSGameSettings GetGameSettings() const { return CurrentSettings; }
protected:
    UPROPERTY(BlueprintReadOnly, Category = "Settings") FFPSGameSettings CurrentSettings;
    UPROPERTY(BlueprintReadOnly, Category = "Progress") UFPSPlayerProgressData* PlayerProgressData;
    UPROPERTY(EditDefaultsOnly, Category = "Data") UFPSGlobalGameData* GlobalGameData;
};
```

关键特性：
- 管理全局配置与存档系统。
- 维护玩家跨关卡的进度数据。
- 提供全局数据访问入口。