// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "Components/ActorComponent.h"
#include "DataAssets/SKGPDAOpticReticleSettings.h"
#include "DataAssets/SKGPDAOpticMagnificationSettings.h"
#include "DataAssets/SKGPDAOpticZeroSettings.h"
#include "SKGOpticComponent.generated.h"

class USKGPDAOpticInitialize;
class UMeshComponent;
class UMaterialInstanceDynamic;
class USKGOpticSceneCaptureComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneCaptureStateChanged, bool, bStartedCapture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPointOfImpactChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPointOfImpactUpDownReset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPointOfImpactLeftRightReset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPointOfImpactUpChanged, int, clicks);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPointOfImpactDownChanged, int, clicks);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPointOfImpactLeftChanged, int, clicks);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPointOfImpactRightChanged, int, clicks);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnZoomChanged, bool, bZoomedIn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReticleCycled, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReticleBrightnessChanged, bool, bIncrease, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNightVisionModeChanged, bool, bNightVisionModeOn, bool, bWasSuccessful);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKGOPTIC_API USKGOpticComponent : public UActorComponent, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USKGOpticComponent();
	UFUNCTION(BlueprintPure, Category = "SKGShooterFrameworkStatics|Getters")
	static USKGOpticComponent* GetOpticComponent(const AActor* Actor);
	virtual void BeginPlay() override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }
	void InitializeOpticComponent();

	void InitializeComponentFromData();
	FORCEINLINE bool HasAuthority() const { return GetOwnerRole() == ROLE_Authority; }
	// Should only be used when manually setting the value for construction
	void SetOpticMeshName(const FName& Name) { OpticMeshName = Name; }
	// Should only be used when manually setting the value for construction
	void SetOpticSceneCaptureComponentName(const FName& Name) { OpticSceneCaptureComponentName = Name; }
	
	/**
	 * @return true, this optic is magnified
	 */
	UFUNCTION(BlueprintPure, Category = "SKGOptic")
	bool IsMagnifiedOptic() const { return OpticSceneCaptureComponent != nullptr; }
	UFUNCTION(BlueprintPure, Category = "SKGOptic")
	USKGOpticSceneCaptureComponent* GetOpticSceneCaptureComponent() const { return OpticSceneCaptureComponent; }
	// Cycles through the set reticles based on the given settings
	UFUNCTION(BlueprintCallable, Category = "SKGOptic|Reticle")
	void CycleReticle();
	UFUNCTION(BlueprintCallable, Category = "SKGOptic|Reticle")
	void CycleNextWeightedBlendable();
	// Increases your reticle brightness based on the given settings
	UFUNCTION(BlueprintCallable, Category = "SKGOptic|Reticle")
	void IncreaseReticleBrightness();
	// Decreases your reticle brightness based on the given settings
	UFUNCTION(BlueprintCallable, Category = "SKGOptic|Reticle")
	void DecreaseReticleBrightness();
	// Toggles the night vision setting on the optic (bright to not bright)
	UFUNCTION(BlueprintCallable, Category = "SKGOptic|Reticle")
	void ToggleReticleNightVisionSetting();

	// Zom your optic in
	UFUNCTION(BlueprintCallable, Category = "SKGOptic|Zoom")
	void ZoomIn();
	// Zoom your optic out
	UFUNCTION(BlueprintCallable, Category = "SKGOptic|Zoom")
	void ZoomOut();
	/**
	 * @return the current magnification of the optic
	 */
	UFUNCTION(BlueprintPure, Category = "SKGOptic|Zoom")
	float GetCurrentMagnification() const { return MagnificationSettings.GetCurrentMagnification(); }

	/**
	 * @return the scale defined in the magnification settings for mouse sensitivity. Default = 1.0
	 */
	UFUNCTION(BlueprintPure, Category = "SKGOptic|Zoom")
	float GetMagnificationSensitivityScale() const { return MagnificationSettings.SensitivityScale; }
	/* Sets the point of impact for elevation back to default
	 * @return the amount of clicks required for turrent to return back to 0
	 */
	UFUNCTION(BlueprintCallable, Category = "SKGOptic|Zero")
	int32 PointOfImpactUpDownDefault();
	/* Sets the point of impact for windage back to default
	* @return the amount of clicks required for turrent to return back to 0
	*/
	UFUNCTION(BlueprintCallable, Category = "SKGOptic|Zero")
	int32 PointOfImpactLeftRightDefault();
	/**
	 * Adjust your zero so your point of impact moves up
	 * @param Clicks How many clicks of the turret
	 */
	UFUNCTION(BlueprintCallable, Category = "SKGOptic|Zero")
	void PointOfImpactUp(const int32 Clicks = 1);
	/**
	 * Adjust your zero so your point of impact moves Down
	 * @param Clicks How many clicks of the turret
	 */
	UFUNCTION(BlueprintCallable, Category = "SKGOptic|Zero")
	void PointOfImpactDown(const int32 Clicks = 1);
	/**
	 * Adjust your zero so your point of impact moves Left
	 * @param Clicks How many clicks of the turret
	 */
	UFUNCTION(BlueprintCallable, Category = "SKGOptic|Zero")
	void PointOfImpactLeft(const int32 Clicks = 1);
	/**
	 * Adjust your zero so your point of impact moves Right
	 * @param Clicks How many clicks of the turret
	 */
	UFUNCTION(BlueprintCallable, Category = "SKGOptic|Zero")
	void PointOfImpactRight(const int32 Clicks = 1);

	UFUNCTION(BlueprintCallable, Category = "SKGOptic|Zero")
	void ApplyLookAtRotationZero(const FRotator& LookAtRotation);
	
	UFUNCTION(BlueprintGetter)
	UMeshComponent* GetOpticMesh() const { return OpticMesh; }
	UFUNCTION(BlueprintGetter)
	bool IsMagnifier() const { return bIsMagnifier; }

	UFUNCTION(BlueprintCallable, Category = "SKGOptic")
	void StartedAiming();
	UFUNCTION(BlueprintCallable, Category = "SKGOptic")
	void StoppedAiming();

	// This fires for the local client when the scene capture state changes (started/stopped)
	UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
	FOnSceneCaptureStateChanged OnSceneCaptureStateChanged;
	// This fires for the local client when the point of impact changes at all
	UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
	FOnPointOfImpactChanged OnPointOfImpactChanged;
	// This fires for the local client when the point of impact for up/down gets reset
	UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
	FOnPointOfImpactUpDownReset OnPointOfImpactUpDownReset;
	// This fires for the local client when the point of impact for left/right gets reset
	UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
	FOnPointOfImpactLeftRightReset OnPointOfImpactLeftRightReset;
	// This fires for the local client when the point of impact for up gets changed
	UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
	FOnPointOfImpactUpChanged OnPointOfImpactUpChanged;
	// This fires for the local client when the point of impact for down gets changed
	UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
	FOnPointOfImpactDownChanged OnPointOfImpactDownChanged;
	// This fires for the local client when the point of impact for left gets changed
	UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
	FOnPointOfImpactLeftChanged OnPointOfImpactLeftChanged;
	// This fires for the local client when the point of impact for right gets changed
	UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
	FOnPointOfImpactRightChanged OnPointOfImpactRightChanged;
	// This fires for the local client when zoom changes
	UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
	FOnZoomChanged OnZoomChanged;
	// This fires for the local client when reticle gets cycled/changed
	UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
	FOnReticleCycled OnReticleCycled;
	// This fires for the local client when the reticle brightness changes
	UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
	FOnReticleBrightnessChanged OnReticleBrightnessChanged;
	// This fires for the local client when night vision mode changes
	UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
	FOnNightVisionModeChanged OnNightVisionModeChanged;

	// if true, the component will auto initialize itself upon creation
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic|Initialize")
	bool bAutoInitialize {true};
	// if true, this assumes you will set the OpticMeshName and OpticSceneCaptureComponentName in code before initialization
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic|Initialize")
	bool bOverrideComponentNames {false};
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic|Initialize")
	TObjectPtr<USKGPDAOpticInitialize> InitializationSettingsDataAsset;
	
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic|Settings")
	TObjectPtr<USKGPDAOpticReticleSettings> ReticleSettingsDataAsset;
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic|Settings")
	TObjectPtr<USKGPDAOpticMagnificationSettings> MagnificationSettingsDataAsset;
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic|Settings")
	TObjectPtr<USKGPDAOpticZeroSettings> OpticZeroSettingsDataAsset;
	
	// The name of the mesh that is used for the optic itself (contains the reticle)
	FName OpticMeshName {"StaticMesh"};
	// Optional Scene Capture Component that gets controlled through this class
	FName OpticSceneCaptureComponentName {"SKGOpticSceneCapture"};
	// This is just to aid in use for creating magnifiers
	bool bIsMagnifier {false};
	FGameplayTagContainer GameplayTags;

protected:
	UPROPERTY()
	FSKGOpticReticleSettings ReticleSettings;
	FSKGOpticMagnificationSettings MagnificationSettings;
	FSKGOpticZeroSettings OpticZeroSettings;
	
	// Useful if your NetUpdateFrequency is set super low
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic|Settings")
	bool bAutoCallForceNetUpdate {true};
	
	UPROPERTY(BlueprintGetter = GetOpticMesh, Category = "SKGOptic|Mesh")
	TObjectPtr<UMeshComponent> OpticMesh;
	UPROPERTY()
	TObjectPtr<USKGOpticSceneCaptureComponent> OpticSceneCaptureComponent;

	int32 ZeroUpDownClicks {0};
	int32 ZeroLeftRightClicks {0};
	FTimerHandle UnAimedTimerHandle;
	uint8 BlendablesIndex {0};
	
	FORCEINLINE void TryForceNetUpdate() const;
	void SetComponents();
	void SetStartingZero();
	void UpdateOpticMaterialInstance();
	void UpdateReticleBrightness();

	void StartSceneCapture();
	void StopSceneCapture();
};
