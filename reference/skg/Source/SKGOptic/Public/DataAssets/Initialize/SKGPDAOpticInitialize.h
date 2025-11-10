// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SKGPDAOpticInitialize.generated.h"

/**
 * 
 */
UCLASS()
class SKGOPTIC_API USKGPDAOpticInitialize : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// The name of the mesh that is used for the optic itself (contains the reticle)
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic|Initialize")
	FName OpticMeshName {"StaticMesh"};
	// Optional Scene Capture Component that gets controlled through this class
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic|Initialize")
	FName OpticSceneCaptureComponentName {"SKGOpticSceneCapture"};
	// This is just to aid in use for creating magnifiers
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic|Initialize")
	bool bIsMagnifier {false};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGOptic|Initialize")
	FGameplayTagContainer GameplayTags;
};
