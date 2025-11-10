// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/SKGActorConstructionInterface.h"
#include "SKGBaseActor.h"
#include "SKGOptic.generated.h"

class USKGOpticSceneCaptureComponent;
struct FInstancedStruct;
class USKGPDAOptic;
class USKGProceduralAnimComponent;
class USKGOpticComponent;

UCLASS()
class SKGSHOOTERFRAMEWORKACTORS_API ASKGOptic : public ASKGBaseActor
{
	GENERATED_BODY()

public:
	ASKGOptic();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetConstructionDataAsset(UDataAsset* DataAsset) override;
	virtual UPrimaryDataAsset* GetConstructionDataAsset() override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Optic")
	TObjectPtr<USceneComponent> RootSceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Optic")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Optic")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Optic")
	TObjectPtr<USKGOpticSceneCaptureComponent> OpticSceneCaptureComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Optic")
	TObjectPtr<USKGOpticComponent> OpticComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Optic")
	TObjectPtr<USKGProceduralAnimComponent> ProceduralAnimComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Optic")
	TArray<FName> BundlesToLoad {"Visual"};
	
	UPROPERTY()
	TObjectPtr<UMeshComponent> MeshComponent;
	// false = using skeletal mesh
	bool bIsUsingStaticMesh {true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_DAConstruction, Category = "Optic", meta = (ExposeOnSpawn = true))
	TObjectPtr<USKGPDAOptic> DAConstruction;
	UFUNCTION()
	virtual void OnRep_DAConstruction();
	
	virtual void OnBundlesLoaded() override;
};
