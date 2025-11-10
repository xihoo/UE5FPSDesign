// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "DataAssets/SKGPDALightLaserSettings.h"
#include "Components/ActorComponent.h"
#include "Interfaces/SKGInfraredInterface.h"
#include "SKGLightLaserComponent.generated.h"

class USKGPDALightLaserInitialize;

DECLARE_STATS_GROUP(TEXT("SKGShooterFrameworkLightLaserComponentStatsGroup"), STATGROUP_SKGLightLaserComponent,
                    STATCAT_Advanced);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLaserStateChanged, ESKGLaserMode, LaserMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLightModeChanged, ESKGLightMode, LightMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInfraredModeChanged, bool, bInInfraredMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLightStrobed, bool, bOn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLaserImpact, const FHitResult&, HitResult, bool, bHit);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKGLIGHTLASER_API USKGLightLaserComponent : public UActorComponent, public IGameplayTagAssetInterface, public ISKGInfraredInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USKGLightLaserComponent();
	UFUNCTION(BlueprintPure, Category = "SKGShooterFrameworkStatics|Getters")
	static USKGLightLaserComponent* GetLightLaserComponent(const AActor* Actor);
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }
	void InitializeLightLaserComponent();

	void InitializeComponentFromData();
	FORCEINLINE bool HasAuthority() const { return GetOwnerRole() == ROLE_Authority; }
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Should only be used when manually setting the value for construction
	void SetLightComponentName(const FName& Name) { LightComponentName = Name; }
	// Should only be used when manually setting the value for construction
	void SetLaserMeshComponentName(const FName& Name) { LaserMeshComponentName = Name; }
	// Should only be used when manually setting the value for construction
	void SetLaserDotComponentName(const FName& Name) { LaserDotComponentName = Name; }
	
	// Sets whether the device should be in infrared mode (for night vision use)
	UFUNCTION(BlueprintCallable, Category = "SKGLightLaser|LightLaser")
	void SetInfraredMode(bool InfraredModeOn);
	// Cycles through the defined lightlaser modes in LightLaserCycleModes (similar to tarkov setup)
	UFUNCTION(BlueprintCallable, Category = "SKGLightLaser|Laser")
	void CycleLightLaserMode();
	// Returns true if valid index and will set the mode, will do nothing if invalid
	UFUNCTION(BlueprintCallable, Category = "SKGLightLaser|Laser")
	bool SetLightLaserCycleModeIndex(const int32 Index);
	UFUNCTION(BlueprintPure, Category = "SKGLightLaser|Laser")
	int32 GetLightLaserCycleModeIndex() const { return LightLaserCycleModes.GetIndex(); }
	
	// Sets the laser to be off/on
	UFUNCTION(BlueprintCallable, Category = "SKGLightLaser|Laser")
	void SetLaserMode(ESKGLaserMode LaserMode);
	// Gets whether or not the laser is in the On or Off state
	UFUNCTION(BlueprintPure, Category = "SKGLightLaser|Laser")
	ESKGLaserMode GetLaserMode() const { return LaserState.LaserMode; }
	UFUNCTION(BlueprintPure, Category = "SKGLightLaser|Laser")
	bool IsLaserOn() const { return LaserState.LaserMode != ESKGLaserMode::Off; }
	
	// Sets the light to be Off/On/Strobe
	UFUNCTION(BlueprintCallable, Category = "SKGLightLaser|Light")
	void SetLightMode(ESKGLightMode LightMode);
	// Gets the current state of the light mode such as On/Off/Strobe
	UFUNCTION(BlueprintPure, Category = "SKGLightLaser|Light")
	ESKGLightMode GetLightMode() const { return LightState.LightMode; }
	// Gets whether the light is in On/Strobe (true) or Off (false)
	UFUNCTION(BlueprintPure, Category = "SKGLightLaser|Light")
	bool IsLightOn() const { return LightState.LightMode != ESKGLightMode::Off; }
	// Manually set the light intensity to a specific index in the in use array (normal/infrared)
	UFUNCTION(BlueprintCallable, Category = "SKGLightLaser|Light")
	void SetLightIntensityIndex(int32 Index);
	// Cycles through the light intensities set in the settings (uses infrared settings if in infrared mode)
	UFUNCTION(BlueprintCallable, Category = "SKGLightLaser|Light")
	void CycleLightIntensity() { SetLightIntensityIndex(LightSettings.GetNextLightIntensityIndex(bDeviceInfraredOn, LightState.LightIntensityIndex)); }

	UFUNCTION(BlueprintGetter)
	ULightComponent* GetLightSource() const { return LightSource; }
	UFUNCTION(BlueprintGetter)
	UStaticMeshComponent* GetLaserMesh() const { return LaserMesh; }
	UFUNCTION(BlueprintGetter)
	UStaticMeshComponent* GetLaserDot() const { return LaserDot; }
	UFUNCTION(BlueprintGetter)
	bool IsInInfraredMode() const { return bDeviceInfraredOn; }
	
	// START OF SKGInfraredInterface-----------------------------------------
	virtual bool IsInfraredModeOnForDevice() const override { return bDeviceInfraredOn; }
	virtual void OnInfraredEnabledForPlayer() override;
	virtual void OnInfraredDisabledForPlayer() override;
	// END OF SKGInfraredInterface-------------------------------------------

	// Events
	UPROPERTY(BlueprintAssignable, Category = "SKGLightLaser|Events")
	FOnLaserStateChanged OnLaserStateChanged;
	UPROPERTY(BlueprintAssignable, Category = "SKGLightLaser|Events")
	FOnLightModeChanged OnLightModeChanged;
	UPROPERTY(BlueprintAssignable, Category = "SKGLightLaser|Events")
	FOnInfraredModeChanged OnInfraredModeChanged;
	UPROPERTY(BlueprintAssignable, Category = "SKGLightLaser|Events")
	FOnLightStrobed OnLightStrobed;
	UPROPERTY(BlueprintAssignable, Category = "SKGLightLaser|Events")
	FOnLaserImpact OnLaserImpact;

	// if true, the component will auto initialize itself upon creation
	bool bAutoInitialize {true};
	// if true, this assumes you will set the LightComponentName, LaserMeshComponentName, and LaserDotComponentName in code before initialization
	bool bOverrideComponentNames {false};

	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser|Initialize")
	TObjectPtr<USKGPDALightLaserInitialize> InitializationSettingsDataAsset;
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser|Settings")
	TObjectPtr<USKGPDALightLaserSettings> LightLaserSettingsDataAsset;

	// Optional name of the Light Component (Such as SpotLight) to be used
	FName LightComponentName {"SpotLight"};
	// Optional name of the laser mesh component to be used
	FName LaserMeshComponentName {"LaserMesh"};
	// Optional name of the laser dot mesh component to be used
	FName LaserDotComponentName {"LaserDot"};
	// Whether this device supports infrared mode
	bool bHasInfraredMode {false};
	// Useful if your NetUpdateFrequency is set super low
	bool bAutoCallForceNetUpdate {true};
	FGameplayTagContainer GameplayTags;
protected:
	// Deprecated, Utilize Light Laser Settings instead
	FSKGLaserSettings LaserSettings;
	// Deprecated, Utilize Light Laser Settings instead
	FSKGLightSettings LightSettings;
	// Deprecated, Utilize Light Laser Settings instead
	FSKGLightLaserCycleModes LightLaserCycleModes;
	
	UPROPERTY(BlueprintGetter = GetLightSource, Category = "SKGLightLaser|Data")
	TObjectPtr<ULightComponent> LightSource;
	UPROPERTY(BlueprintGetter = GetLaserMesh, Category = "SKGLightLaser|Data")
	TObjectPtr<UStaticMeshComponent> LaserMesh;
	UPROPERTY(BlueprintGetter = GetLaserDot, Category = "SKGLightLaser|Data")
	TObjectPtr<UStaticMeshComponent> LaserDot;

	FVector LaserScale {FVector::OneVector};

	UPROPERTY()
	TObjectPtr<APlayerController> LocalPlayerController;

	UPROPERTY(ReplicatedUsing = OnRep_DeviceInfraredOn, BlueprintGetter = IsInInfraredMode, Category = "SKGLightLaser|Data")
	bool bDeviceInfraredOn {false};
	UFUNCTION()
	void OnRep_DeviceInfraredOn();
	
	UPROPERTY(ReplicatedUsing = OnRep_LaserState)
	FSKGLaserState LaserState;
	UFUNCTION()
	void OnRep_LaserState();
	
	UPROPERTY(ReplicatedUsing = OnRep_LightState)
	FSKGLightState LightState;
	UFUNCTION()
	void OnRep_LightState();
	double PreviousStrobeTimeStamp {0.0f};
	
	FORCEINLINE void TryForceNetUpdate() const;
	void SetComponentsAndState();
	void RegisterAsInfraredDevice();
	void UnregisterAsInfraredDevice();
	void PerformLaserScaling();
	void PerformLightStrobing();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetInfraredMode(bool bInfraredOn);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetLaserMode(ESKGLaserMode LaserMode);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetLightMode(ESKGLightMode LightMode);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetLightIntensityIndex(int32 Index);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetLightLaserMode(const FSKGLightLaserCycleMode& LightLaserMode);

	void SetLightLaserMode(const FSKGLightLaserCycleMode& LightLaserMode);
};