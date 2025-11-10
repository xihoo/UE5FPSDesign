// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SKGPDAMuzzleInitialize.generated.h"

/**
 * 
 */
UCLASS()
class SKGMUZZLE_API USKGPDAMuzzleInitialize : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// The name of the mesh to be used for the muzzle (to gather the muzzle transform from)
	UPROPERTY(EditDefaultsOnly, Category = "SKGMuzzle")
	FName MuzzleMeshComponentName {"StaticMesh"};
	// The name of the socket on the MuzzleMeshComponentName mesh to be used to gather the muzzle transform from
	UPROPERTY(EditDefaultsOnly, Category = "SKGMuzzle")
	FName MuzzleSocketName {"S_Muzzle"};
};
