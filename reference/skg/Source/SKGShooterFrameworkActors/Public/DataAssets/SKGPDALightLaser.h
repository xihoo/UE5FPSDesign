// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/CollisionProfile.h"
#include "DataAssets/SKGPDABase.h"
#include "SKGPDALightLaser.generated.h"

class USKGPDALightLaserInitialize;
class USKGPDALightLaserSettings;
class UStaticMesh;
class USkeletalMesh;

UCLASS()
class SKGSHOOTERFRAMEWORKACTORS_API USKGPDALightLaser : public USKGPDABase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary")
	FCollisionProfileName MeshCollisionProfile;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary", meta = (AssetBundles = "Visual"))
	TSoftObjectPtr<UStaticMesh> StaticMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary", meta = (AssetBundles = "Visual"))
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary", meta = (AssetBundles = "Visual"))
	TSoftObjectPtr<UStaticMesh> LaserMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary", meta = (AssetBundles = "Visual"))
	TSoftObjectPtr<UStaticMesh> LaserDotMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<USKGPDALightLaserInitialize> LightLaserInitializeSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<USKGPDALightLaserSettings> LightLaserSettings;
	// If false, it will destroy any laser related component
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	bool bIsLaser {true};
	// If false, it will destroy the light component
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	bool bIsLight {true};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	FName LaserMeshAttachSocket {FName("S_Laser")};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	float LaserDotScale {1.0f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	FName LightAttachSocket {FName("S_Light")};
};
