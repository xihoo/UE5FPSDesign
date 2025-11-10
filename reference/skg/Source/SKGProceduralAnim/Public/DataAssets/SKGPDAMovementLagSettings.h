// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/SKGProceduralDataTypes.h"
#include "Engine/DataAsset.h"
#include "SKGPDAMovementLagSettings.generated.h"

/**
 * 
 */
UCLASS()
class SKGPROCEDURALANIM_API USKGPDAMovementLagSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGProcedural")
	FSKGMovementLagSettings MovementLagSettings;
};
