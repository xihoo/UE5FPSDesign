// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/SKGActorConstructionInterface.h"
#include "SKGBaseActor.h"
#include "SKGFirearm.generated.h"

class USKGOffhandIKComponent;
class USKGMuzzleComponent;
struct FInstancedStruct;
struct FSKGAttachmentComponentData;
class USKGProceduralAnimComponent;
class USKGAttachmentManagerComponent;
class USKGFirearmComponent;
class USKGPDAFirearm;


UCLASS()
class SKGSHOOTERFRAMEWORKACTORS_API ASKGFirearm : public ASKGBaseActor
{
	GENERATED_BODY()

public:
	ASKGFirearm();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetConstructionDataAsset(UDataAsset* DataAsset) override;
	virtual UPrimaryDataAsset* GetConstructionDataAsset() override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Firearm")
	TObjectPtr<USkeletalMeshComponent> FirearmMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Firearm")
	TObjectPtr<USKGFirearmComponent> FirearmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Firearm")
	TObjectPtr<USKGAttachmentManagerComponent> AttachmentManagerComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Firearm")
	TObjectPtr<USKGProceduralAnimComponent> ProceduralAnimComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Firearm")
	TObjectPtr<USKGMuzzleComponent> MuzzleComponent;
	UPROPERTY(BlueprintReadOnly, Category = "Firearm")
	TObjectPtr<USKGOffhandIKComponent> OffhandIKComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firearm")
	TArray<FName> BundlesToLoad {"Visual"};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_DAConstruction, Category = "Firearm", meta = (ExposeOnSpawn = true))
	TObjectPtr<USKGPDAFirearm> DAConstruction;
	UFUNCTION()
	virtual void OnRep_DAConstruction();
	
	virtual void OnBundlesLoaded() override;
};