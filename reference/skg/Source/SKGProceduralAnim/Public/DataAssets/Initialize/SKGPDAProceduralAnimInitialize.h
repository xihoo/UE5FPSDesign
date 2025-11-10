// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SKGPDAProceduralAnimInitialize.generated.h"

/**
 * 
 */
UCLASS()
class SKGPROCEDURALANIM_API USKGPDAProceduralAnimInitialize : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// The mesh name to be found for the to be used for an optic, laser, firearm, etc. to aim with
	UPROPERTY(EditDefaultsOnly, Category = "SKGProceduralAnim|Initialize")
	FName ProceduralMeshName {"StaticMesh"};
	// The sockets on the ProceduralMeshName that can be used for aiming
	UPROPERTY(EditDefaultsOnly, Category = "SKGProceduralAnim|Initialize")
	TArray<FName> ProceduralAimSocketNames {"S_Aim"};
	/** The index to use if you decide to point aim. Primarily useful for the
	 * procedural anim component on a firearm. -1 = no point aim socket
	 */
	UPROPERTY(EditDefaultsOnly, Category = "SKGProceduralAnim|Initialize")
	int32 RightHandDominatePointAimAimSocketIndex {INDEX_NONE};
	/** The index to use if you decide to point aim. Primarily useful for the
	 * procedural anim component on a firearm. -1 = no point aim socket
	 */
	UPROPERTY(EditDefaultsOnly, Category = "SKGProceduralAnim|Initialize")
	int32 LeftHandDominatePointAimAimSocketIndex {INDEX_NONE};
	// If this is true, this allows you to enter left hand mode if you have the anims setup.
	UPROPERTY(EditDefaultsOnly, Category = "SKGProceduralAnim|Initialize")
	bool bCanUseLeftHandDominate {false};
	// Used to tell what you are holding (such as a rifle, pistol, optic, etc)
	UPROPERTY(EditDefaultsOnly, Category = "SKGProceduralAnim|Initialize")
	FGameplayTag ProceduralAnimGameplayTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGProceduralAnim|Initialize")
	FGameplayTagContainer GameplayTags;
};
