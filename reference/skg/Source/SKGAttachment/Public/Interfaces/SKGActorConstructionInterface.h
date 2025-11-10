// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SKGActorConstructionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class SKGATTACHMENT_API USKGActorConstructionInterface : public UInterface
{
	GENERATED_BODY()
};

class UDataAsset;
class UPrimaryDataAsset;
class UMeshComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnActorConstructed, AActor*);
class SKGATTACHMENT_API ISKGActorConstructionInterface
{
	GENERATED_BODY()

public:
	virtual void SetConstructionDataAsset(UDataAsset* DataAsset) {}
	virtual UPrimaryDataAsset* GetConstructionDataAsset() { return nullptr; }

	FOnActorConstructed OnActorConstructed;
};
