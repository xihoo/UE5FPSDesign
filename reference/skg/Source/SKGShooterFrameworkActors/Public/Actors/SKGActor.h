// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/SKGActorConstructionInterface.h"
#include "SKGBaseActor.h"
#include "SKGActor.generated.h"

class USKGPDAActor;
class USKGMuzzleComponent;
struct FInstancedStruct;

UCLASS()
class SKGSHOOTERFRAMEWORKACTORS_API ASKGActor : public ASKGBaseActor
{
	GENERATED_BODY()

public:
	ASKGActor();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetConstructionDataAsset(UDataAsset* DataAsset) override;
    virtual UPrimaryDataAsset* GetConstructionDataAsset() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SKGActor")
	TObjectPtr<USceneComponent> RootSceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grip")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grip")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grip")
	TArray<FName> BundlesToLoad {"Visual"};
	
	UPROPERTY()
	TObjectPtr<UMeshComponent> MeshComponent;
	// false = using skeletal mesh
	bool bIsUsingStaticMesh {true};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_DAConstruction, Category = "SKGActor", meta = (ExposeOnSpawn = true))
	TObjectPtr<USKGPDAActor> DAConstruction;
	UFUNCTION()
	virtual void OnRep_DAConstruction();
	
	virtual void OnBundlesLoaded() override;
};
