// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SKGMuzzleWorldSubsystem.generated.h"

class USKGMuzzleComponent;

UCLASS()
class SKGMUZZLE_API USKGMuzzleWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual bool IsTickable() const override { return Components.Num() > 0; }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(USKGMuzzleWorldSubsystem, STATGROUP_Tickables); }
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY()
	TArray<TObjectPtr<USKGMuzzleComponent>> Components;

public:
	void RegisterComponent(USKGMuzzleComponent* Component);
	void UnregisterComponent(USKGMuzzleComponent* Component);
};
