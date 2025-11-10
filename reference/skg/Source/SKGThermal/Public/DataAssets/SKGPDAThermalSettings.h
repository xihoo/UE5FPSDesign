// Copyright 2025, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SKGPDAThermalSettings.generated.h"



UCLASS()
class SKGTHERMAL_API USKGPDAThermalSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// Components with this tag will be used with the heat system
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGThermal")
	FName HotComponentTag {NAME_None};
	// Time in seconds it takes to go from hot to cold
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGThermal")
	float TimeToCool {10.0f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGThermal")
	float StartingHeatValue {1.0f};
};
