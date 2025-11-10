// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "Interfaces/SKGActorConstructionInterface.h"
#include "GameFramework/Actor.h"
#include "SKGBaseActor.generated.h"

class UPrimaryDataAsset;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSKGActorDAConstructionBundlesLoaded);

/**
 * The entire purpose of this class is to offer a class blueprint users can inherit from
 * that implements the gameplay tag interface. Likewise, a variation of the attachment system makes use
 * of the gameplay tag interface for handling attachment compatibility
 */
UCLASS()
class SKGSHOOTERFRAMEWORKACTORS_API ASKGBaseActor : public AActor, public IGameplayTagAssetInterface, public ISKGActorConstructionInterface
{
	GENERATED_BODY()

public:
	ASKGBaseActor();
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }
	// Gets called once the bundles have been loaded into memory from the data asset
	UFUNCTION(BlueprintImplementableEvent, Category = "SKGActor|Bundles")
	void DAConstructionBundlesLoaded();
	// Broadcasts once the bundles have been loaded into memory from the data asset
	UPROPERTY(BlueprintAssignable, Category = "SKGActor|Bundles")
	FOnSKGActorDAConstructionBundlesLoaded OnDAConstructionBundlesLoaded;
	UFUNCTION(BlueprintPure, Category = "SKGActor|Bundles")
	bool AreBundlesLoaded() const { return bDAConstructionBundlesLoaded; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGGameplayTags")
	FGameplayTagContainer GameplayTags;

	bool bIsInitialized {false};
	bool bDAConstructionBundlesLoaded {false};

	virtual void OnBundlesLoaded();
};
