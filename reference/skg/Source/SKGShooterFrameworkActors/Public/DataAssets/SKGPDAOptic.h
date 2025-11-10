// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/CollisionProfile.h"
#include "DataAssets/SKGPDABase.h"
#include "SKGPDAOptic.generated.h"

class UStaticMesh;
class USkeletalMesh;
class USKGPDAOpticInitialize;
class USKGPDAProceduralAnimInitialize;
class USKGPDAOpticReticleSettings;
class USKGPDAOpticMagnificationSettings;
class USKGPDAOpticZeroSettings;
class USKGPDAAimingSettings;

UCLASS()
class SKGSHOOTERFRAMEWORKACTORS_API USKGPDAOptic : public USKGPDABase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary")
	FCollisionProfileName MeshCollisionProfile;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary", meta = (AssetBundles = "Visual"))
	TSoftObjectPtr<UStaticMesh> StaticMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary", meta = (AssetBundles = "Visual"))
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|SceneCapture")
	bool bHasSceneCaptureComponent {false};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|SceneCapture", meta = (EditCondition = "bHasSceneCaptureComponent"))
	FName SceneCaptureAttachSocketName {"S_SceneCapture"};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<USKGPDAOpticInitialize> OpticInitializeSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<USKGPDAProceduralAnimInitialize> ProceduralAnimInitializeSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<USKGPDAAimingSettings> AimingSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<USKGPDAOpticReticleSettings> ReticleSetting;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<USKGPDAOpticMagnificationSettings> MagnificationSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<USKGPDAOpticZeroSettings> OpticZeroSettings;
};
