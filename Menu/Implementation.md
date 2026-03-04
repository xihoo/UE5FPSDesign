# Menu 系统 C++ 实现示例

本文档提供关键类的完整C++实现代码。

---

## 1. UFPSGameSettings 实现

### FPSGameSettings.h

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FPSGameSettings.generated.h"

UENUM(BlueprintType)
enum class EGraphicsQuality : uint8
{
    VeryLow     UMETA(DisplayName = "非常低"),
    Low         UMETA(DisplayName = "低"),
    Medium      UMETA(DisplayName = "中"),
    High        UMETA(DisplayName = "高"),
    VeryHigh    UMETA(DisplayName = "非常高"),
    Epic        UMETA(DisplayName = "史诗"),
    Custom      UMETA(DisplayName = "自定义")
};

UENUM(BlueprintType)
enum class EGameDifficulty : uint8
{
    Easy        UMETA(DisplayName = "简单"),
    Normal      UMETA(DisplayName = "普通"),
    Hard        UMETA(DisplayName = "困难"),
    Nightmare   UMETA(DisplayName = "噩梦")
};

UCLASS()
class FPSGAME_API UFPSGameSettings : public USaveGame
{
    GENERATED_BODY()
    
public:
    UFPSGameSettings();
    
    static const FString SaveSlotName;
    static const uint32 SaveUserIndex;
    
    // ========== 图形设置 ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EWindowMode::Type WindowMode;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    FIntPoint Resolution;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    int32 FrameRateLimit;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    bool bVSync;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EGraphicsQuality OverallQuality;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EGraphicsQuality AntiAliasingQuality;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EGraphicsQuality PostProcessQuality;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EGraphicsQuality ShadowQuality;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EGraphicsQuality TextureQuality;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EGraphicsQuality EffectsQuality;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EGraphicsQuality ViewDistanceQuality;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    EGraphicsQuality FoliageQuality;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    float FieldOfView;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    bool bShowFPS;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
    bool bMotionBlur;
    
    // ========== 音频设置 ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float MasterVolume;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float MusicVolume;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float SFXVolume;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float VoiceVolume;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float AmbientVolume;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    bool bMuteMaster;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    bool bEnableVoiceChat;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    bool bSubtitles;
    
    // ========== 控制设置 ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    float MouseSensitivity;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    float AimSensitivityMultiplier;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    bool bInvertMouseY;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    float GamepadSensitivity;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    float GamepadDeadZone;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    bool bAimAssist;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    bool bToggleSprint;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    bool bToggleAim;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    bool bToggleCrouch;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls")
    TMap<FName, FKey> CustomKeyBindings;
    
    // ========== 游戏设置 ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    EGameDifficulty Difficulty;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    bool bShowDamageNumbers;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    bool bShowEnemyHealthBar;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    bool bAutoLoot;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    FString Language;
    
    // ========== 方法 ==========
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ApplyAllSettings();
    
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ApplyGraphicsSettings();
    
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ApplyAudioSettings();
    
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ApplyControlSettings();
    
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ResetToDefaults();
    
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SaveSettings();
    
    static UFPSGameSettings* LoadOrCreateSettings();
    
private:
    void ApplyDefaultSettings();
    bool ValidateGraphicsSettings();
};
```

### FPSGameSettings.cpp

```cpp
#include "UI/Menu/FPSGameSettings.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"

const FString UFPSGameSettings::SaveSlotName = TEXT("FPSSettings");
const uint32 UFPSGameSettings::SaveUserIndex = 0;

UFPSGameSettings::UFPSGameSettings()
{
    ApplyDefaultSettings();
}

void UFPSGameSettings::ApplyDefaultSettings()
{
    // Graphics
    WindowMode = EWindowMode::Fullscreen;
    Resolution = FIntPoint(1920, 1080);
    FrameRateLimit = 120;
    bVSync = true;
    OverallQuality = EGraphicsQuality::High;
    AntiAliasingQuality = EGraphicsQuality::High;
    PostProcessQuality = EGraphicsQuality::High;
    ShadowQuality = EGraphicsQuality::High;
    TextureQuality = EGraphicsQuality::High;
    EffectsQuality = EGraphicsQuality::High;
    ViewDistanceQuality = EGraphicsQuality::High;
    FoliageQuality = EGraphicsQuality::High;
    FieldOfView = 90.0f;
    bShowFPS = false;
    bMotionBlur = true;
    
    // Audio
    MasterVolume = 1.0f;
    MusicVolume = 0.8f;
    SFXVolume = 1.0f;
    VoiceVolume = 1.0f;
    AmbientVolume = 0.7f;
    bMuteMaster = false;
    bEnableVoiceChat = true;
    bSubtitles = true;
    
    // Controls
    MouseSensitivity = 1.0f;
    AimSensitivityMultiplier = 0.5f;
    bInvertMouseY = false;
    GamepadSensitivity = 1.0f;
    GamepadDeadZone = 0.2f;
    bAimAssist = true;
    bToggleSprint = false;
    bToggleAim = false;
    bToggleCrouch = true;
    
    // Gameplay
    Difficulty = EGameDifficulty::Normal;
    bShowDamageNumbers = true;
    bShowEnemyHealthBar = true;
    bAutoLoot = false;
    Language = TEXT("zh-Hans");
}

void UFPSGameSettings::ApplyAllSettings()
{
    ApplyGraphicsSettings();
    ApplyAudioSettings();
    ApplyControlSettings();
}

void UFPSGameSettings::ApplyGraphicsSettings()
{
    UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
    if (!UserSettings) return;
    
    // Apply window mode and resolution
    UserSettings->SetFullscreenMode(WindowMode);
    UserSettings->SetScreenResolution(Resolution);
    UserSettings->SetFrameRateLimit(FrameRateLimit);
    UserSettings->SetVSyncEnabled(bVSync);
    
    // Apply quality settings
    if (OverallQuality != EGraphicsQuality::Custom)
    {
        int32 QualityLevel = StaticCast<int32>(OverallQuality);
        UserSettings->SetOverallScalabilityLevel(QualityLevel);
    }
    else
    {
        UserSettings->SetAntiAliasingQuality(StaticCast<int32>(AntiAliasingQuality));
        UserSettings->SetPostProcessingQuality(StaticCast<int32>(PostProcessQuality));
        UserSettings->SetShadowQuality(StaticCast<int32>(ShadowQuality));
        UserSettings->SetTextureQuality(StaticCast<int32>(TextureQuality));
        UserSettings->SetVisualEffectQuality(StaticCast<int32>(EffectsQuality));
        UserSettings->SetViewDistanceQuality(StaticCast<int32>(ViewDistanceQuality));
        UserSettings->SetFoliageQuality(StaticCast<int32>(FoliageQuality));
    }
    
    UserSettings->ApplySettings(false);
    UserSettings->SaveSettings();
}

void UFPSGameSettings::ApplyAudioSettings()
{
    // Set sound mix classes
    // 假设你在项目中有对应的SoundClass
    // USoundClass* MasterSoundClass = LoadObject<USoundClass>(nullptr, TEXT("/Game/Audio/SoundClasses/SC_Master.SC_Master"));
    // if (MasterSoundClass)
    // {
    //     MasterSoundClass->Properties.Volume = bMuteMaster ? 0.0f : MasterVolume;
    // }
    
    // 使用SoundMix来应用音量
    // UGameplayStatics::SetSoundMixClassOverride(this, SoundMix, MasterSoundClass, MasterVolume);
    // UGameplayStatics::PushSoundMixModifier(this, SoundMix);
}

void UFPSGameSettings::ApplyControlSettings()
{
    // 控制设置通常在PlayerController或Character中应用
    // 这里可以广播一个委托，让相关组件更新
}

void UFPSGameSettings::SaveSettings()
{
    UGameplayStatics::SaveGameToSlot(this, SaveSlotName, SaveUserIndex);
}

UFPSGameSettings* UFPSGameSettings::LoadOrCreateSettings()
{
    if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex))
    {
        USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex);
        if (UFPSGameSettings* Settings = Cast<UFPSGameSettings>(SaveGame))
        {
            return Settings;
        }
    }
    
    // Create new settings with defaults
    UFPSGameSettings* NewSettings = Cast<UFPSGameSettings>(
        UGameplayStatics::CreateSaveGameObject(UFPSGameSettings::StaticClass())
    );
    
    if (NewSettings)
    {
        NewSettings->SaveSettings();
    }
    
    return NewSettings;
}

void UFPSGameSettings::ResetToDefaults()
{
    ApplyDefaultSettings();
    ApplyAllSettings();
    SaveSettings();
}
```

---

## 2. UFPSMenuManager 实现

### FPSMenuManager.h

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CommonActivatableWidgetContainer.h"
#include "FPSMenuManager.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnMenuPopupConfirmed);
DECLARE_DYNAMIC_DELEGATE(FOnMenuPopupCancelled);

class UFPSMenuBase;
class UFPSGameSettings;
class UInputMappingContext;

UCLASS()
class FPSGAME_API UFPSMenuManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    
    // Main Menu
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void OpenMainMenu();
    
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void CloseMainMenuAndStartGame();
    
    // Pause Menu
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void TogglePauseMenu();
    
    UFUNCTION(BlueprintPure, Category = "Menu")
    bool IsPauseMenuOpen() const;
    
    // Stack Management
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void PushMenu(TSubclassOf<UFPSMenuBase> MenuClass);
    
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void PopMenu();
    
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void PopToRoot();
    
    // Popups
    UFUNCTION(BlueprintCallable, Category = "Menu", meta = (AutoCreateRefTerm = "OnConfirmed,OnCancelled"))
    void ShowConfirmPopup(
        const FText& Title,
        const FText& Message,
        const FText& ConfirmText,
        const FText& CancelText,
        FOnMenuPopupConfirmed OnConfirmed,
        FOnMenuPopupCancelled OnCancelled
    );
    
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void ShowMessagePopup(const FText& Title, const FText& Message);
    
    // Settings
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void LoadAllSettings();
    
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void SaveAllSettings();
    
    UFUNCTION(BlueprintPure, Category = "Menu")
    UFPSGameSettings* GetGameSettings() const { return GameSettings; }
    
protected:
    UPROPERTY()
    UCommonActivatableWidgetContainerBase* MenuLayer;
    
    UPROPERTY()
    UCommonActivatableWidgetContainerBase* PopupLayer;
    
    UPROPERTY()
    UFPSGameSettings* GameSettings;
    
    UPROPERTY(EditDefaultsOnly, Category = "Menu")
    TSubclassOf<UFPSMenuBase> MainMenuClass;
    
    UPROPERTY(EditDefaultsOnly, Category = "Menu")
    TSubclassOf<UFPSMenuBase> PauseMenuClass;
    
    UPROPERTY(EditDefaultsOnly, Category = "Menu")
    TSubclassOf<UFPSMenuBase> SettingsMenuClass;
    
    UPROPERTY(EditDefaultsOnly, Category = "Menu")
    TSubclassOf<UFPSMenuBase> ConfirmPopupClass;
    
    UPROPERTY(EditDefaultsOnly, Category = "Menu")
    TSubclassOf<UFPSMenuBase> MessagePopupClass;
    
    // Input Mapping Contexts
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* GameplayMappingContext;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* UIMappingContext;
    
    void SetupMenuLayers();
    void SetGamePaused(bool bPaused);
    void SetInputModeForMenu(bool bMenuOpen);
    void SwitchToUIMode();
    void SwitchToGameplayMode();
};
```

### FPSMenuManager.cpp (关键部分)

```cpp
#include "UI/Menu/FPSMenuManager.h"
#include "UI/Menu/FPSMenuBase.h"
#include "UI/Menu/FPSGameSettings.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"

void UFPSMenuManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Load settings
    GameSettings = UFPSGameSettings::LoadOrCreateSettings();
    if (GameSettings)
    {
        GameSettings->ApplyAllSettings();
    }
    
    // Setup menu layers when world is ready
    FWorldDelegates::OnPostWorldInitialization.AddWeakLambda(this, [this](UWorld* World, const UWorld::InitializationValues IVS)
    {
        SetupMenuLayers();
    });
}

void UFPSMenuManager::SetupMenuLayers()
{
    UWorld* World = GetWorld();
    if (!World) return;
    
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC) return;
    
    // Create menu layer widget
    // Note: In practice, you'd create a specific widget that contains the containers
    // This is simplified
}

void UFPSMenuManager::PushMenu(TSubclassOf<UFPSMenuBase> MenuClass)
{
    if (!MenuLayer || !MenuClass) return;
    
    MenuLayer->AddWidget(MenuClass);
}

void UFPSMenuManager::PopMenu()
{
    if (!MenuLayer) return;
    
    MenuLayer->RemoveTopWidget();
}

void UFPSMenuManager::TogglePauseMenu()
{
    if (IsPauseMenuOpen())
    {
        PopMenu();
        SetGamePaused(false);
        SwitchToGameplayMode();
    }
    else
    {
        if (PauseMenuClass)
        {
            SetGamePaused(true);
            SwitchToUIMode();
            PushMenu(PauseMenuClass);
        }
    }
}

void UFPSMenuManager::SwitchToUIMode()
{
    UWorld* World = GetWorld();
    if (!World) return;
    
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC) return;
    
    // Add UI mapping context
    if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (UIMappingContext)
            {
                InputSubsystem->AddMappingContext(UIMappingContext, 10);
            }
        }
    }
    
    // Set input mode
    FInputModeUIOnly InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    PC->SetInputMode(InputMode);
    PC->SetShowMouseCursor(true);
}

void UFPSMenuManager::SwitchToGameplayMode()
{
    UWorld* World = GetWorld();
    if (!World) return;
    
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC) return;
    
    // Remove UI mapping context
    if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (UIMappingContext)
            {
                InputSubsystem->RemoveMappingContext(UIMappingContext);
            }
        }
    }
    
    // Set input mode
    FInputModeGameOnly InputMode;
    PC->SetInputMode(InputMode);
    PC->SetShowMouseCursor(false);
}

bool UFPSMenuManager::IsPauseMenuOpen() const
{
    if (!MenuLayer) return false;
    return MenuLayer->GetNumWidgets() > 0;
}

void UFPSMenuManager::SetGamePaused(bool bPaused)
{
    UWorld* World = GetWorld();
    if (World)
    {
        UGameplayStatics::SetGamePaused(World, bPaused);
    }
}
```

---

## 3. UFPSMenuBase 实现

### FPSMenuBase.h

```cpp
#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "FPSMenuBase.generated.h"

class UFPSMenuManager;
class UFPSGameSettings;
class UFPSCommonButton;
class UCommonTextBlock;

UCLASS(Abstract)
class FPSGAME_API UFPSMenuBase : public UCommonActivatableWidget
{
    GENERATED_BODY()
    
public:
    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual UWidget* NativeGetDesiredFocusTarget() const override;
    
protected:
    UFUNCTION(BlueprintNativeEvent, Category = "Menu")
    UWidget* GetDefaultFocusWidget() const;
    virtual UWidget* GetDefaultFocusWidget_Implementation() const { return nullptr; }
    
    UFUNCTION(BlueprintNativeEvent, Category = "Menu")
    void PlayOpenAnimation();
    virtual void PlayOpenAnimation_Implementation() {}
    
    UFUNCTION(BlueprintNativeEvent, Category = "Menu")
    void PlayCloseAnimation();
    virtual void PlayCloseAnimation_Implementation() {}
    
    UFUNCTION(BlueprintCallable, Category = "Menu")
    virtual void OnBackPressed();
    
    UPROPERTY(EditDefaultsOnly, Category = "Menu|Input")
    FDataTableRowHandle BackInputAction;
    
    UPROPERTY(meta = (BindWidget))
    UCommonTextBlock* MenuTitle;
    
    UPROPERTY(meta = (BindWidgetOptional))
    UImage* BackgroundImage;
    
    UPROPERTY(meta = (BindWidget))
    UPanelWidget* ContentContainer;
    
    UPROPERTY(meta = (BindWidgetOptional))
    UFPSCommonButton* BackButton;
    
    UPROPERTY(EditDefaultsOnly, Category = "Menu")
    bool bIsPauseMenu = false;
    
    UFUNCTION(BlueprintPure, Category = "Menu")
    UFPSMenuManager* GetMenuManager() const;
    
    UFUNCTION(BlueprintPure, Category = "Menu")
    UFPSGameSettings* GetGameSettings() const;
};
```

### FPSMenuBase.cpp

```cpp
#include "UI/Menu/FPSMenuBase.h"
#include "UI/Menu/FPSMenuManager.h"
#include "UI/Menu/FPSGameSettings.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "CommonInputSubsystem.h"

void UFPSMenuBase::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    
    // Register back action if configured
    if (BackInputAction.RowName != NAME_None)
    {
        RegisterInputActionBinding(BackInputAction, ECommonInputEvent::Triggered, FExecuteInputActionDelegate::CreateUObject(this, &UFPSMenuBase::OnBackPressed));
    }
    
    // Bind back button
    if (BackButton)
    {
        BackButton->OnClicked().AddUObject(this, &UFPSMenuBase::OnBackPressed);
    }
}

void UFPSMenuBase::NativeConstruct()
{
    Super::NativeConstruct();
    PlayOpenAnimation();
}

void UFPSMenuBase::NativeDestruct()
{
    PlayCloseAnimation();
    Super::NativeDestruct();
}

UWidget* UFPSMenuBase::NativeGetDesiredFocusTarget() const
{
    return GetDefaultFocusWidget();
}

void UFPSMenuBase::OnBackPressed()
{
    if (UFPSMenuManager* Manager = GetMenuManager())
    {
        Manager->PopMenu();
    }
}

UFPSMenuManager* UFPSMenuBase::GetMenuManager() const
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        return GameInstance->GetSubsystem<UFPSMenuManager>();
    }
    return nullptr;
}

UFPSGameSettings* UFPSMenuBase::GetGameSettings() const
{
    if (UFPSMenuManager* Manager = GetMenuManager())
    {
        return Manager->GetGameSettings();
    }
    return nullptr;
}
```

---

## 4. 与 Enhanced Input 集成

### 键位绑定应用代码

```cpp
void UFPSGameSettings::ApplyControlSettings()
{
    // Get the local player
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
    if (!World) return;
    
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC) return;
    
    ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
    if (!LocalPlayer) return;
    
    UEnhancedInputLocalPlayerSubsystem* InputSubsystem = 
        LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    if (!InputSubsystem) return;
    
    // Apply custom key bindings
    for (const auto& Pair : CustomKeyBindings)
    {
        FName ActionName = Pair.Key;
        FKey NewKey = Pair.Value;
        
        // Load the InputAction
        FSoftObjectPath ActionPath(FString::Printf(TEXT("/Game/Input/Actions/IA_%s.IA_%s"), 
            *ActionName.ToString(), *ActionName.ToString()));
        UInputAction* InputAction = Cast<UInputAction>(ActionPath.TryLoad());
        
        if (InputAction)
        {
            // Remove existing mappings for this action
            InputSubsystem->RemoveAllPlayerMappedKeysForAction(InputAction);
            
            // Add new mapping
            FModifyContextOptions Options;
            Options.bIgnoreAllPressedKeysUntilRelease = false;
            
            InputSubsystem->AddPlayerMappedKeyInSlot(
                InputAction,
                NewKey,
                EInputMappingRebuildType::None,
                Options
            );
        }
    }
    
    // Rebuild control mappings
    InputSubsystem->RequestRebuildControlMappings(
        EInputMappingRebuildType::RebuildWithFlush,
        true
    );
}
```

---

## 5. 辅助函数

### 获取可用分辨率列表

```cpp
void UFPSGraphicsSettingsWidget::InitializeResolutionOptions()
{
    if (!ResolutionCombo) return;
    
    ResolutionCombo->ClearOptions();
    
    // Get supported resolutions
    TArray<FIntPoint> Resolutions;
    UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
    
    for (const FIntPoint& Resolution : Resolutions)
    {
        FString ResolutionString = FString::Printf(TEXT("%dx%d"), Resolution.X, Resolution.Y);
        ResolutionCombo->AddOption(ResolutionString);
    }
}
```

### 检查键位冲突

```cpp
bool UFPSControlSettingsWidget::CheckKeyConflict(FKey Key, FName ExcludedAction)
{
    if (UFPSGameSettings* Settings = GetGameSettings())
    {
        for (const auto& Pair : Settings->CustomKeyBindings)
        {
            if (Pair.Key != ExcludedAction && Pair.Value == Key)
            {
                return true;
            }
        }
    }
    return false;
}
```

---

这些实现代码提供了完整的Menu系统基础，可以在此基础上继续扩展更多功能。
