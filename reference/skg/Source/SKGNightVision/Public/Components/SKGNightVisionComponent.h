// Copyright 2025, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/SKGPDANightVisionSettings.h"
#include "Components/ActorComponent.h"
#include "SKGNightVisionComponent.generated.h"

static FName NoiseFactorParameterName {FName("NoiseFactor")};
static FName GrainMultiplierParameterName {FName("GrainMultiplier")};

static FName NightVisionColorParameterName {FName("NightVisionColor")};
static FName GainParameterName {FName("Gain")};
static FName ContrastParameterName {FName("Contrast")};
static FName NormalizedGainParameterName {FName("NormalizedGain")};

static FName DistortionRadiusParameterName {FName("DistortionRadius")};
static FName DistortionHardnessParameterName {FName("DistortionHardness")};
static FName UseRadialDistortionParameterName {FName("bUseRadialDistortion")};


/**
 * This component is intended to be added to anything you want night vision control on such as
 * your pawn and/or optic.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKGNIGHTVISION_API USKGNightVisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USKGNightVisionComponent();
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
	FNightVisionMaterialPair& CreateAndSetupDynamicNightVisionMaterialInstance(USKGPDANightVisionTubeSettings* Tube, USKGPDANightVisionHousingSettings* Housing);

	// GainAlpha = 0-1, 0 = off, 1 = full gain
	UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
	void SetGain(const float GainAlpha);
	// StepAmount is the amount to increase the gain by. Returns true if gain is max
	UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
	void IncreaseGain();
	// StepAmount is the amount to increase the gain by. Returns true if gain is 0
	UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
	void DecreaseGain();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision")
	float MinimumGainValue {0.01f};
	UPROPERTY()
	TObjectPtr<USKGPDANightVisionTubeSettings> TubeSettings;
	UPROPERTY()
	TObjectPtr<USKGPDANightVisionHousingSettings> HousingSettings;
	UPROPERTY()
	FNightVisionMaterialPair NightVisionMaterialPair;

	float CurrentGainLerpValue {0.0f};
	float TargetGainLerpValue {0.0f};
	FTimerHandle TSmoothGainAdjustHandle;
	float PreviousSmoothGainAdjustTimestamp {0.0f};

	float GetGainValue() const;
	void SetGainValues(const float Gain) const;
};