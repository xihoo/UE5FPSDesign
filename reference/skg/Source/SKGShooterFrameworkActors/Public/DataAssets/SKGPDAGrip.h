// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/CollisionProfile.h"
#include "DataAssets/SKGPDABase.h"
#include "SKGPDAGrip.generated.h"

class USKGPDAOffhandIKInitialize;
class USKGPDAOffhandIKSettings;
class UStaticMesh;
class USkeletalMesh;

UCLASS()
class SKGSHOOTERFRAMEWORKACTORS_API USKGPDAGrip : public USKGPDABase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary")
	FCollisionProfileName MeshCollisionProfile;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary", meta = (AssetBundles = "Visual"))
	TSoftObjectPtr<UStaticMesh> StaticMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary", meta = (AssetBundles = "Visual"))
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<USKGPDAOffhandIKInitialize> OffhandIKInitializeSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<USKGPDAOffhandIKSettings> OffhandIKSettings;
};
