// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SKGPDAOpticMagnificationSettings.generated.h"

USTRUCT(BlueprintType)
struct FSKGOpticMagnificationSettings
{
	GENERATED_BODY()
	// Used in conjunction with SKGOpticSceneCaptureComponent
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
	TArray<float> Magnifications {1.0f};
	// The scale defined in the magnification settings for mouse sensitivity
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
	float SensitivityScale {1.0f};
	// If true, zooming in/out will be handled smoothly
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
	bool bSmoothZoom {true};
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic", meta = (EditCondition = "bSmoothZoom"))
	float SmoothZoomRate {10.0f};
	// Used in conjunction with SKGOpticSceneCaptureComponent. Allows the eyebox to shrink by a given amount as you zoon in
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
	bool bShrinkEyeboxWithMagnification {true};
	// Used in conjunction with SKGOpticSceneCaptureComponent. Amount to shrink the eyebox by as you zoom in
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic", meta = (EditCondition = "bShrinkEyeboxWithMagnification"))
	float ShrinkEyeboxMultiplier {0.5f};
	/**
	 * If true the optic will be first focal plane and the reticle will scale as you zoom in/out
	 */
	UPROPERTY(EditDefaultsOnly, Category = "SKGOpticSceneCaptureComponent")
	bool bIsFirstFocalPlane {true};

private:
	uint8 CurrentMagnificationIndex {0};

public:
	// Returns true if Current Magnification changed
	bool SetNextMagnification()
	{
		if (++CurrentMagnificationIndex >= Magnifications.Num())
		{
			CurrentMagnificationIndex = Magnifications.Num() - 1;
			return false;
		}
		return true;
	}

	// Returns true if Current Magnification changed
	bool SetPreviousMagnification()
	{
		if (!CurrentMagnificationIndex - 1 < 0)
		{
			--CurrentMagnificationIndex;
			return true;
		}
		return false;
	}
	
	float GetCurrentMagnification() const
	{
		return Magnifications[CurrentMagnificationIndex];
	}

	float GetCurrentMagnificationFOV() const;
};


UCLASS()
class SKGOPTIC_API USKGPDAOpticMagnificationSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
	FSKGOpticMagnificationSettings MagnificationSettings;
};
