// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SKGShooterFrameworkActorConstructionHelpers.generated.h"

class UStaticMesh;
class USkeletalMesh;
class UMeshComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent;
struct FInstancedStruct;
struct FSKGAttachmentComponentData;
static FName MeshComponentTagName {"SKGActorMeshTag"};

UCLASS()
class SKGSHOOTERFRAMEWORKACTORS_API USKGShooterFrameworkActorConstructionHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "ShooterFrameworkActorConstruction")
	static void ConstructAttachment(const FSKGAttachmentComponentData& Data, UMeshComponent* Parent);
	
	static void ConstructAttachment(const TArray<FInstancedStruct>& Data, UMeshComponent* Parent);
	static UMeshComponent* SetupAttachments(TSoftObjectPtr<UStaticMesh> StaticMesh, TSoftObjectPtr<USkeletalMesh> SkeletalMesh, UStaticMeshComponent* StaticMeshComponent, USkeletalMeshComponent* SkeletalMeshComponent, bool& bIsUsingStaticMesh);

	UFUNCTION(BlueprintCallable, Category = "ShooterFrameworkActorConstruction")
	static void SetConstructionDataAsset(AActor* Actor, UDataAsset* DataAsset);
	UFUNCTION(BlueprintPure, Category = "ShooterFrameworkActorConstruction")
	static UPrimaryDataAsset* GetConstructionDataAsset(AActor* Actor);
};
