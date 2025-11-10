// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/SKGActorConstructionInterface.h"
#include "SKGBaseActor.h"
#include "SKGGrip.generated.h"

class USKGOffhandIKComponent;
struct FInstancedStruct;
class USKGPDAGrip;


UCLASS()
class SKGSHOOTERFRAMEWORKACTORS_API ASKGGrip : public ASKGBaseActor
{
	GENERATED_BODY()

public:
	ASKGGrip();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetConstructionDataAsset(UDataAsset* DataAsset) override;
	virtual UPrimaryDataAsset* GetConstructionDataAsset() override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grip")
	TObjectPtr<USceneComponent> RootSceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grip")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grip")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grip")
	TObjectPtr<USKGOffhandIKComponent> OffhandIKComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grip")
	TArray<FName> BundlesToLoad {"Visual"};
	
	UPROPERTY()
	TObjectPtr<UMeshComponent> MeshComponent;
	// false = using skeletal mesh
	bool bIsUsingStaticMesh {true};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_DAConstruction, Category = "Grip", meta = (ExposeOnSpawn = true))
	TObjectPtr<USKGPDAGrip> DAConstruction;
	UFUNCTION()
	virtual void OnRep_DAConstruction();
	
	virtual void OnBundlesLoaded() override;
};
