// Copyright 2025, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SKGPDANightVisionSettings.generated.h"

class UMaterialInterface;
class UMaterialInstanceDynamic;

USTRUCT(BlueprintType)
struct FNightVisionMaterialPair
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision")
	TObjectPtr<UMaterialInstanceDynamic> NightVisionMaterialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision")
	TObjectPtr<UMaterialInstanceDynamic> NightVisionScintillationMaterialInstance;
};


UCLASS()
class SKGNIGHTVISION_API USKGPDANightVisionTubeSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Material")
	TObjectPtr<UMaterialInterface> NightVisionMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Material")
	TObjectPtr<UMaterialInterface> NightVisionScintillationMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|NightVision")
	FLinearColor Color {FLinearColor(0.4f, 0.88f, 1.0f, 1.0f)};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|NightVision")
	float Gain {100.0f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|NightVision")
	float Contrast {1.0f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|NightVision")
	float MaxGain {1000.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Grain")
	float NoiseFactor {0.992};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Grain")
	float GrainMultiplier {0.5};
};

UCLASS()
class SKGNIGHTVISION_API USKGPDANightVisionHousingSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Housing")
	bool bManualGain {true};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Housing", meta = (EditCondition = "bManualGain"))
	bool bSmoothAdjustGain {false};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Housing", meta = (EditCondition = "bManualGain && bSmoothAdjustGain"))
	float SmoothAdjustSpeed {1.0f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Housing", meta = (EditCondition = "bManualGain"))
	float GainAdjustmentStepAmount {0.1f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Housing", meta = (EditCondition = "bManualGain"))
	float MinimumGain {0.1f};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Lens")
	float DistortionRadius {0.88f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Lens")
	float DistortionHardness {4.0f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Lens")
	bool bUseRadialDistortion {true};
};