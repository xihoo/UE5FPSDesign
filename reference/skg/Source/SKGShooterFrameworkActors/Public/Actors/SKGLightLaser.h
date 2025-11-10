// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/SKGActorConstructionInterface.h"
#include "SKGBaseActor.h"
#include "SKGLightLaser.generated.h"

class USpotLightComponent;
class USKGPDALightLaser;
class USKGLightLaserComponent;
class UMeshComponent;
struct FInstancedStruct;

UCLASS()
class SKGSHOOTERFRAMEWORKACTORS_API ASKGLightLaser : public ASKGBaseActor
{
	GENERATED_BODY()

public:
	ASKGLightLaser();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetConstructionDataAsset(UDataAsset* DataAsset) override;
	virtual UPrimaryDataAsset* GetConstructionDataAsset() override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LightLaser")
	TObjectPtr<USceneComponent> RootSceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LightLaser")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LightLaser")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LightLaser")
	TObjectPtr<UStaticMeshComponent> LaserMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LightLaser")
	TObjectPtr<UStaticMeshComponent> LaserDotMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LightLaser")
	TObjectPtr<USpotLightComponent> SpotlightComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LightLaser")
	TObjectPtr<USKGLightLaserComponent> LightLaserComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LightLaser")
	TArray<FName> BundlesToLoad {"Visual"};
	
	UPROPERTY()
	TObjectPtr<UMeshComponent> MeshComponent;
	// false = using skeletal mesh
	bool bIsUsingStaticMesh {true};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_DAConstruction, Category = "LightLaser", meta = (ExposeOnSpawn = true))
	TObjectPtr<USKGPDALightLaser> DAConstruction;
	UFUNCTION()
	virtual void OnRep_DAConstruction();
	
	virtual void OnBundlesLoaded() override;
};
