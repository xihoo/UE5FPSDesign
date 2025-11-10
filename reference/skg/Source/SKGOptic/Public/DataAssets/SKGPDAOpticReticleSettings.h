// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/Scene.h"
#include "Engine/DataAsset.h"
#include "SKGPDAOpticReticleSettings.generated.h"

class UMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class USKGOpticSceneCaptureComponent;

USTRUCT(BlueprintType)
struct FSKGOpticReticle
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
	TObjectPtr<UMaterialInterface> ReticleMaterial;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicReticleMaterial;
	float StartingReticleSizeParameterValue {1.0f};
	float StartingEyeboxRangeParameterValue {-2000.0f};

	operator bool () const
	{
		return DynamicReticleMaterial != nullptr;
	}
};

USTRUCT(BlueprintType)
struct FSKGOpticReticleSettings
{
	GENERATED_BODY()
	// Material Index that your reticle is on
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
	int32 ReticleMaterialIndex {0};
	// If true you can switch to night vision brightness settings
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
	bool bSupportsNightVision {true};
	// Array of reticle materials to cycle through
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
	TArray<FSKGOpticReticle> ReticleMaterials;
	// Material to use when you are not aiming. Used only with magnified optics
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
	FSKGOpticReticle UnAimedReticleMaterial;
	// When you stop aiming, how long before the capture stops and reticle change happens. Used only with magnified optics
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
	float UnAimedCaptureDelay {2.0f};
	// Parameter name in the material that controls reticle brightness
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
	FName ReticleBrightnessParameterName {"ReticleBrightness"};
	// Brightness settings for default mode
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
	TArray<float> ReticleBrightnessSettings {1.0f};
	// Brightness settings for the night vision mode
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic", meta = (EditCondition = "bSupportsNightVision"))
	TArray<float> ReticleNightVisionBrightnessSettings;
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
	FWeightedBlendables WeightedBlendables;
	// This socket is for the eyebox center position (generally going to be your aim socket)
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
	FName EyeboxPositionSocket {FName("S_Aim")};
	
	bool bUsingReticleNightVisionBrightness {false};
private:
	uint8 CurrentReticleIndex {0};
	uint8 CurrentReticleBrightnessIndex {0};
	uint8 CurrentReticleNightVisionBrightnessIndex {0};

public:
	void ConstructDynamicMaterials(UMeshComponent* OpticMeshComponent, USKGOpticSceneCaptureComponent* OptionalOpticSceneCaptureComponent, const FName EyeboxSocketName);

	// Returns true if there was a change, false if no change
	bool CycleReticleMaterial()
	{
		const int32 OldIndex = CurrentReticleIndex;
		if (++CurrentReticleIndex >= ReticleMaterials.Num())
		{
			CurrentReticleIndex = 0;
		}
		return CurrentReticleIndex != OldIndex;
	}

	FSKGOpticReticle GetReticleMaterial()
	{
		if (ReticleMaterials.Num() > 0)
		{
			return ReticleMaterials[CurrentReticleIndex];
		}
		return FSKGOpticReticle();
	}
	
	// Returns true if brightness changed
	bool IncreaseReticleBrightnessSetting();
	// Returns true if brightness changed
	bool DecreaseReticleBrightnessSetting();

	float GetReticleBrightness() const
	{
		if (bUsingReticleNightVisionBrightness)
		{
			return ReticleNightVisionBrightnessSettings[CurrentReticleNightVisionBrightnessIndex];
		}
		return ReticleBrightnessSettings[CurrentReticleBrightnessIndex];
	}

	// Returns true if state changed
	bool CycleReticleNightVisionMode()
	{
		if (bSupportsNightVision && ReticleNightVisionBrightnessSettings.Num())
		{
			bUsingReticleNightVisionBrightness = !bUsingReticleNightVisionBrightness;
			return true;
		}
		return false;
	}
};


UCLASS()
class SKGOPTIC_API USKGPDAOpticReticleSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
	FSKGOpticReticleSettings ReticleSettings;
};
