// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/CollisionProfile.h"
#include "DataAssets/SKGPDABase.h"
#include "SKGPDAActor.generated.h"

class UStaticMesh;
class USkeletalMesh;

UCLASS()
class SKGSHOOTERFRAMEWORKACTORS_API USKGPDAActor : public USKGPDABase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary")
	FCollisionProfileName MeshCollisionProfile;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary", meta = (AssetBundles = "Visual"))
	TSoftObjectPtr<UStaticMesh> StaticMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary", meta = (AssetBundles = "Visual"))
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
};
