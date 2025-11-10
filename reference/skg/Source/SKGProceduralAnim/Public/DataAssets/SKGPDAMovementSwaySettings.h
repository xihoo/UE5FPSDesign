// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/SKGProceduralDataTypes.h"
#include "Engine/DataAsset.h"
#include "SKGPDAMovementSwaySettings.generated.h"

/**
 * 
 */
UCLASS()
class SKGPROCEDURALANIM_API USKGPDAMovementSwaySettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGProcedural")
	bool bUseMovementSway {true};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGProcedural")
	FSKGMovementSwayLocationSettings LocationSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGProcedural")
	FSKGMovementSwayLocationSettings RotationSettings;
};
