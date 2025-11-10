// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SKGPDALightLaserInitialize.generated.h"

/**
 * 
 */
UCLASS()
class SKGLIGHTLASER_API USKGPDALightLaserInitialize : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// Optional name of the Light Component (Such as SpotLight) to be used
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser|Initialize")
	FName LightComponentName {"SpotLight"};
	// Optional name of the laser mesh component to be used
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser|Initialize")
	FName LaserMeshComponentName {"LaserMesh"};
	// Optional name of the laser dot mesh component to be used
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser|Initialize")
	FName LaserDotComponentName {"LaserDot"};
	// Whether this device supports infrared mode
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser|Initialize")
	bool bHasInfraredMode {false};
	// Useful if your NetUpdateFrequency is set super low
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser|Initialize")
	bool bAutoCallForceNetUpdate {true};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGLightLaser|Initialize")
	FGameplayTagContainer GameplayTags;
};
