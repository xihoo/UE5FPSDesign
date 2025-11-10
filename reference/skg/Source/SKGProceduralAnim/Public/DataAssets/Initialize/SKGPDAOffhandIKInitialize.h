// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SKGPDAOffhandIKInitialize.generated.h"

/**
 * 
 */
UCLASS()
class SKGPROCEDURALANIM_API USKGPDAOffhandIKInitialize : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// The mesh used to gather data for the Left Hand IK (think forward grip/handguard)
	UPROPERTY(EditDefaultsOnly, Category = "SKGOffhandIK")
	FName OffhandIKMeshName {"StaticMesh"};
	// Socket used for the Left Hand IK transform (S_OffhandIK in example project)
	UPROPERTY(EditDefaultsOnly, Category = "SKGOffhandIK")
	FName LeftHandIKSocketName {"S_LeftHandIK"};
	// Socket used for the Right Hand IK transform (S_OffhandIK in example project)
	UPROPERTY(EditDefaultsOnly, Category = "SKGOffhandIK")
	FName RightHandIKSocketName {"S_RightHandIK"};
};
