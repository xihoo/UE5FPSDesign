// Copyright 2025, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/SKGPDAThermalSettings.h"
#include "Components/ActorComponent.h"
#include "SKGThermalComponent.generated.h"

class UMaterialParameterCollection;
class UMaterialParameterCollectionInstance;
class UMeshComponent;

USTRUCT()
struct FSKGThermalReplicatedHeat
{
	GENERATED_BODY()
	UPROPERTY()
	float HeatValue {0.0f};
	UPROPERTY()
	bool bIsCooling {false};
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKGTHERMAL_API USKGThermalComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USKGThermalComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	FORCEINLINE bool HasAuthority() const { return GetOwnerRole() == ROLE_Authority; }

	/**
	 * @param ComponentOverride If set, HotComponentTags search is ignored from the data asset
	 */
	UFUNCTION(BlueprintCallable, Category = "SKGThermal")
	void SetupThermalComponent(USKGPDAThermalSettings* Settings, UMaterialParameterCollection* MPC, TArray<UMeshComponent*> ComponentOverride);
	// Value between 0 and 1
	UFUNCTION(BlueprintCallable, Category = "SKGThermal")
	void SetHeatValue(const float Value);
	UFUNCTION(BlueprintCallable, Category = "SKGThermal")
	void StartCooling();
	UFUNCTION(BlueprintCallable, Category = "SKGThermal")
	void StopCooling();
	
protected:
	UPROPERTY()
	TObjectPtr<USKGPDAThermalSettings> ThermalSettings;
	
	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedHeat)
	FSKGThermalReplicatedHeat ReplicatedHeat;
	UFUNCTION()
	void OnRep_ReplicatedHeat();
	
	UPROPERTY()
	TArray<TObjectPtr<UMeshComponent>> MeshComponents;
	UPROPERTY()
	TObjectPtr<UMaterialParameterCollectionInstance> MPCThermalInstance;
	float ThermalStencilMinValue {100.0f};
	float ThermalStencilMaxValue {255.0f};

	float HeatValue {1.0f};
	float CoolingInterval {0.0f};
	float CoolingStepAmount {0.0f};
	float ServerReplicateHeatTimestamp {0.0f};
	FTimerHandle TCoolingHandle;

	int32 GetStencilValue(const float Heat) const;
	void SetMeshComponentsRenderCustomDepth(const bool bCustomDepth);
	void SetMeshComponentsStencilValue(const int32 StencilValue);
};
