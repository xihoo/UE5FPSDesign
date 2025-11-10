// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/SKGProceduralDataTypes.h"
#include "Engine/DataAsset.h"
#include "SKGPDARotationLagSettings.generated.h"

/**
 * 
 */
UCLASS()
class SKGPROCEDURALANIM_API USKGPDARotationLagSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGProcedural")
	FSKGRotationLagSettings RotationLagSettings;
};
