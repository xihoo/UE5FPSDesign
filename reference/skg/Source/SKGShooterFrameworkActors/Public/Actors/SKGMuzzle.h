// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/SKGActorConstructionInterface.h"
#include "SKGBaseActor.h"
#include "SKGMuzzle.generated.h"

class USKGPDAMuzzle;
class USKGMuzzleComponent;
struct FInstancedStruct;

UCLASS()
class SKGSHOOTERFRAMEWORKACTORS_API ASKGMuzzle : public ASKGBaseActor
{
	GENERATED_BODY()

public:
	ASKGMuzzle();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetConstructionDataAsset(UDataAsset* DataAsset) override;
	virtual UPrimaryDataAsset* GetConstructionDataAsset() override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Muzzle")
	TObjectPtr<USceneComponent> RootSceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Muzzle")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Muzzle")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Muzzle")
	TObjectPtr<USKGMuzzleComponent> MuzzleComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Muzzle")
	TArray<FName> BundlesToLoad {"Visual"};
	
	UPROPERTY()
    TObjectPtr<UMeshComponent> MeshComponent;
    // false = using skeletal mesh
    bool bIsUsingStaticMesh {true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_DAConstruction, Category = "Muzzle", meta = (ExposeOnSpawn = true))
	TObjectPtr<USKGPDAMuzzle> DAConstruction;
	UFUNCTION()
	virtual void OnRep_DAConstruction();
	
	virtual void OnBundlesLoaded() override;
};
