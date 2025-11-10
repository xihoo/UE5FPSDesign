// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/SKGProceduralDataTypes.h"
#include "Engine/DataAsset.h"
#include "SKGPDAAimingSettings.generated.h"

/**
 * 
 */
UCLASS()
class SKGPROCEDURALANIM_API USKGPDAAimingSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGProcedural")
	FSKGProceduralAimingSettings AimingSettings;
};
