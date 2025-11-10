// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SKGPDAFirearmInitializeSettings.generated.h"

class USKGPDAFirearmStats;
/**
 * 
 */
UCLASS()
class SKGSHOOTERFRAMEWORK_API USKGPDAFirearmInitializeSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// The name of the mesh to be used/considered the firearm
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFirearmComponent|Initialize")
	FName FirearmMeshComponentName {"FirearmMesh"};
	// The name of the MANDATORY attachment manager added to this parent actor class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFirearmComponent|Initialize")
	FName AttachmentManagerComponentName {NAME_None};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGFirearmComponent|Initialize")
	TObjectPtr<USKGPDAFirearmStats> FirearmStatsDataAsset;
};
