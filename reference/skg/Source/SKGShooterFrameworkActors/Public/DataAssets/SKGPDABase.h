// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/SKGShooterFrameworkActorDataTypes.h"
#include "Engine/CollisionProfile.h"
#include "GameplayTagAssetInterface.h"
#include "Engine/DataAsset.h"
#include "SKGPDABase.generated.h"

class UStaticMesh;
class USkeletalMesh;

// This asset only exists as a base for the other data assets to have gameplay tags and attachment data by default
UCLASS()
class SKGSHOOTERFRAMEWORKACTORS_API USKGPDABase : public UPrimaryDataAsset, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
public:
	// Tags to be used with the gameplay tag asset interface
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayTags")
	FGameplayTagContainer GameplayTags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachment", meta = (BaseStruct = "/Script/SKGShooterFrameworkActors.SKGAttachmentComponentData"))
	TArray<FInstancedStruct> AttachmentData;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }
};
