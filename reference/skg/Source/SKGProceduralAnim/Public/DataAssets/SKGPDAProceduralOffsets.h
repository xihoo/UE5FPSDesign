// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/SKGProceduralDataTypes.h"
#include "Engine/DataAsset.h"
#include "SKGPDAProceduralOffsets.generated.h"

/**
 * 
 */
UCLASS()
class SKGPROCEDURALANIM_API USKGPDAProceduralOffsets : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// The held offset position of the held actor. Separate for first and third person
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGProcedural")
	FTransform FirstPersonBasePoseOffset {FTransform()};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGProcedural")
	FTransform ThirdPersonBasePoseOffset {FTransform()};
	// Offset applied when aiming for third person view with left hand holding firearm
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGProcedural")
	FTransform ThirdPersonRightHandedAimingOffset {FTransform()};
	// Offset applied when aiming for third person view with right hand holding firearm
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGProcedural")
	FTransform ThirdPersonLeftHandedAimingOffset {FTransform()};
};
