// Copyright 2025, Dakota Dawe, All rights reserved


#include "Components/SKGThermalComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/MeshComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


USKGThermalComponent::USKGThermalComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void USKGThermalComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(USKGThermalComponent, ReplicatedHeat, Params);
}

int32 USKGThermalComponent::GetStencilValue(const float Heat) const
{
	return FMath::RoundToInt(FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(ThermalStencilMinValue, ThermalStencilMaxValue), Heat));
}

void USKGThermalComponent::SetMeshComponentsRenderCustomDepth(const bool bCustomDepth)
{
	for (UMeshComponent* Component : MeshComponents)
	{
		Component->SetRenderCustomDepth(bCustomDepth);
	}
}

void USKGThermalComponent::SetMeshComponentsStencilValue(const int32 StencilValue)
{
	for (UMeshComponent* Component : MeshComponents)
	{
		Component->SetCustomDepthStencilValue(StencilValue);
	}
}

void USKGThermalComponent::SetupThermalComponent(USKGPDAThermalSettings* Settings, UMaterialParameterCollection* MPC, TArray<UMeshComponent*> ComponentOverride)
{
	ThermalSettings = Settings;
	if (ComponentOverride.Num())
	{
		MeshComponents = ComponentOverride;
	}
	else
	{
		TArray<UActorComponent*> FoundComponents = GetOwner()->GetComponentsByTag(UMeshComponent::StaticClass(), ThermalSettings->HotComponentTag);
		MeshComponents.Reserve(FoundComponents.Num());
		for (UActorComponent* FoundComponent : FoundComponents)
		{
			MeshComponents.Add(Cast<UMeshComponent>(FoundComponent));
		}
	}
	ensureAlwaysMsgf(MeshComponents.Num() > 0, TEXT("NO mesh components found for SKGThermalComponent On Actor: %s"), *GetOwner()->GetName());
	
	MPCThermalInstance = GetWorld()->GetParameterCollectionInstance(MPC);
	HeatValue = ThermalSettings->StartingHeatValue;
	CoolingInterval = 1.0f / ThermalSettings->TimeToCool;
	CoolingStepAmount = CoolingInterval / ThermalSettings->TimeToCool;

	SetMeshComponentsRenderCustomDepth(true);
	SetMeshComponentsStencilValue(GetStencilValue(HeatValue));
}

void USKGThermalComponent::SetHeatValue(const float Value)
{
	HeatValue = Value;
	SetMeshComponentsStencilValue(GetStencilValue(HeatValue));
}

void USKGThermalComponent::StartCooling()
{
	if (HeatValue > 0.0f)
	{
		ReplicatedHeat.HeatValue = HeatValue;
		ReplicatedHeat.bIsCooling = true;
		MARK_PROPERTY_DIRTY_FROM_NAME(USKGThermalComponent, ReplicatedHeat, this);
		OnRep_ReplicatedHeat();
	}
}

void USKGThermalComponent::StopCooling()
{
	ReplicatedHeat.HeatValue = HeatValue;
	ReplicatedHeat.bIsCooling = false;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGThermalComponent, ReplicatedHeat, this);
	OnRep_ReplicatedHeat();
}

void USKGThermalComponent::OnRep_ReplicatedHeat()
{
	HeatValue = ReplicatedHeat.HeatValue;
	if (ReplicatedHeat.bIsCooling)
	{
		GetWorld()->GetTimerManager().SetTimer(TCoolingHandle, [this]()
		{
			HeatValue -= CoolingStepAmount;
			SetMeshComponentsStencilValue(GetStencilValue(HeatValue));
			if (HasAuthority())
			{	// Replicate the new heat value every second to aid in keeping clients in sync
				const float TimeSinceLastHeatReplication = GetWorld()->GetTimeSeconds() - ServerReplicateHeatTimestamp;
				if (TimeSinceLastHeatReplication > 1.0f)
				{
					ReplicatedHeat.HeatValue = HeatValue;
					MARK_PROPERTY_DIRTY_FROM_NAME(USKGThermalComponent, ReplicatedHeat, this);
				}
			}
			if (HeatValue <= 0.0f)
			{
				StopCooling();
			}
		}, CoolingInterval, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TCoolingHandle);
	}
}
