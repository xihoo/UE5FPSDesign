// Copyright 2023, Dakota Dawe, All rights reserved


#include "Components/SKGOpticComponent.h"
#include "Components/SKGOpticSceneCaptureComponent.h"

#include "GameFramework/Actor.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DataAssets/Initialize/SKGPDAOpticInitialize.h"

USKGOpticComponent::USKGOpticComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(false);
}

USKGOpticComponent* USKGOpticComponent::GetOpticComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<USKGOpticComponent>() : nullptr;
}

void USKGOpticComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (bAutoInitialize)
	{
		InitializeOpticComponent();
	}
}

void USKGOpticComponent::InitializeOpticComponent()
{
	InitializeComponentFromData();

	ensureAlwaysMsgf(ReticleSettings.ReticleMaterials.Num(), TEXT("NO Reticle Materials Found in ReticleData.ReticleMaterials on Actor: %s in SKGOpticComponent: %s"), *GetOwner()->GetName(), *GetName());
	
	SetComponents();
	ReticleSettings.ConstructDynamicMaterials(OpticMesh, OpticSceneCaptureComponent, ReticleSettings.EyeboxPositionSocket);
	OpticZeroSettings.Initialize();
	SetStartingZero();
	UpdateOpticMaterialInstance();
	
	StopSceneCapture();
}

void USKGOpticComponent::TryForceNetUpdate() const
{
	if (bAutoCallForceNetUpdate)
	{
		GetOwner()->ForceNetUpdate();
	}
}

void USKGOpticComponent::InitializeComponentFromData()
{
	if (InitializationSettingsDataAsset)
	{
		if (!bOverrideComponentNames)
		{
			OpticMeshName = InitializationSettingsDataAsset->OpticMeshName;
			OpticSceneCaptureComponentName = InitializationSettingsDataAsset->OpticSceneCaptureComponentName;
		}
		bIsMagnifier = InitializationSettingsDataAsset->bIsMagnifier;
		GameplayTags = InitializationSettingsDataAsset->GameplayTags;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InitializationSettingsDataAsset NOT set on Actor: %s on Component: %s Please utilize this data asset as it will be mandatory moving forwards"), *GetOwner()->GetName(), *GetName());
	}
	
	if (ReticleSettingsDataAsset)
	{
		ReticleSettings = ReticleSettingsDataAsset->ReticleSettings;
	}
	if (MagnificationSettingsDataAsset)
	{
		MagnificationSettings = MagnificationSettingsDataAsset->MagnificationSettings;
	}
	if (OpticZeroSettingsDataAsset)
	{
		OpticZeroSettings = OpticZeroSettingsDataAsset->OpticZeroSettings;
	}
}

void USKGOpticComponent::SetComponents()
{
	ensureAlwaysMsgf(!OpticMeshName.IsEqual(NAME_None), TEXT("Optic Mesh Name must be valid on Actor: %s in the SKGOpticComponent: %s"), *GetOwner()->GetName(), *GetName());
	for (UActorComponent* Component : GetOwner()->GetComponents())
	{
		if (Component)
		{
			if (Component->GetFName().IsEqual(OpticMeshName))
			{
				OpticMesh = Cast<UMeshComponent>(Component);
			}
			else if (Component->GetFName().IsEqual(OpticSceneCaptureComponentName))
			{
				OpticSceneCaptureComponent = Cast<USKGOpticSceneCaptureComponent>(Component);
				if (OpticSceneCaptureComponent)
				{
					OpticSceneCaptureComponent->Initialize(MagnificationSettings.bIsFirstFocalPlane, MagnificationSettings.GetCurrentMagnification(), MagnificationSettings.bSmoothZoom, MagnificationSettings.SmoothZoomRate, MagnificationSettings.bShrinkEyeboxWithMagnification, MagnificationSettings.ShrinkEyeboxMultiplier);
					if (ReticleSettings.WeightedBlendables.Array.Num())
					{
						OpticSceneCaptureComponent->PostProcessSettings.WeightedBlendables = ReticleSettings.WeightedBlendables;
					}
				}
			}
		}
	}
	checkf(OpticMesh, TEXT("Optic Mesh NOT found on Actor: %s in the SKGOpticComponent: %s, Make sure Optic Mesh Name matches the name of the Mesh Component you want to use for your optic"), *GetOwner()->GetName(), *GetName());
}

void USKGOpticComponent::SetStartingZero()
{
	if (OpticSceneCaptureComponent)
	{
		if (OpticZeroSettings.StartingZeroSettings.bStartWithDefaultZero)
		{
			const int32 ElevationClicks = OpticZeroSettings.StartingZeroSettings.DefaultElevationClicks;
			const int32 WindageClicks = OpticZeroSettings.StartingZeroSettings.DefaultWindageClicks;
			const double Elevation = OpticZeroSettings.GetAdjustmentAmount() * ElevationClicks;
			const double Windage = OpticZeroSettings.GetAdjustmentAmount() * WindageClicks;
			OpticSceneCaptureComponent->PointOfImpactUp(Elevation);
			OpticSceneCaptureComponent->PointOfImpactRight(Windage);
		}
		else if (OpticZeroSettings.StartingZeroSettings.bStartWithRandomZero)
		{
			const int32 ElevationClicks = FMath::RandRange(-OpticZeroSettings.StartingZeroSettings.RandomMaxElevationClicks, OpticZeroSettings.StartingZeroSettings.RandomMaxElevationClicks);
			const int32 WindageClicks = FMath::RandRange(-OpticZeroSettings.StartingZeroSettings.RandomMaxWindageClicks, OpticZeroSettings.StartingZeroSettings.RandomMaxWindageClicks);
			const double Elevation = OpticZeroSettings.GetAdjustmentAmount() * ElevationClicks;
			const double Windage = OpticZeroSettings.GetAdjustmentAmount() * WindageClicks;
			OpticSceneCaptureComponent->PointOfImpactUp(Elevation);
			OpticSceneCaptureComponent->PointOfImpactRight(Windage);
		}
	}
}

void USKGOpticComponent::UpdateOpticMaterialInstance()
{
	checkf(ReticleSettings.GetReticleMaterial().DynamicReticleMaterial, TEXT("Reticle Material INVALID on Actor: %s in Component: %s"), *GetOwner()->GetName(), *GetName());
	OpticMesh->SetMaterial(ReticleSettings.ReticleMaterialIndex, ReticleSettings.GetReticleMaterial().DynamicReticleMaterial);
	UpdateReticleBrightness();
	if (OpticSceneCaptureComponent)
	{
		OpticSceneCaptureComponent->Zoom(ReticleSettings.GetReticleMaterial(), MagnificationSettings.GetCurrentMagnification());
	}
}

void USKGOpticComponent::UpdateReticleBrightness()
{
	checkf(ReticleSettings.GetReticleMaterial().DynamicReticleMaterial, TEXT("Reticle Material INVALID on Actor: %s in Component: %s"), *GetOwner()->GetName(), *GetName());
	ReticleSettings.GetReticleMaterial().DynamicReticleMaterial->SetScalarParameterValue(ReticleSettings.ReticleBrightnessParameterName, ReticleSettings.GetReticleBrightness());
}

void USKGOpticComponent::CycleReticle()
{
	const bool bSuccess = ReticleSettings.CycleReticleMaterial();
	if (bSuccess)
	{
		UpdateOpticMaterialInstance();
	}
	OnReticleCycled.Broadcast(bSuccess);
}

void USKGOpticComponent::CycleNextWeightedBlendable()
{
	if (OpticSceneCaptureComponent)
	{
		if (BlendablesIndex < OpticSceneCaptureComponent->PostProcessSettings.WeightedBlendables.Array.Num())
		{
			OpticSceneCaptureComponent->PostProcessSettings.WeightedBlendables.Array[BlendablesIndex].Weight = 0.0f;
			if (++BlendablesIndex >= OpticSceneCaptureComponent->PostProcessSettings.WeightedBlendables.Array.Num())
			{
				BlendablesIndex = 0;
			}
			OpticSceneCaptureComponent->PostProcessSettings.WeightedBlendables.Array[BlendablesIndex].Weight = 1.0f;
		}
	}
}

void USKGOpticComponent::IncreaseReticleBrightness()
{
	const bool bSuccess = ReticleSettings.IncreaseReticleBrightnessSetting();
	if (bSuccess)
	{
		UpdateReticleBrightness();
	}
	OnReticleBrightnessChanged.Broadcast(true, bSuccess);
}

void USKGOpticComponent::DecreaseReticleBrightness()
{
	const bool bSuccess = ReticleSettings.DecreaseReticleBrightnessSetting();
	if (bSuccess)
	{
		UpdateReticleBrightness();
	}
	OnReticleBrightnessChanged.Broadcast(false, bSuccess);
}
void USKGOpticComponent::ToggleReticleNightVisionSetting()
{
	const bool bSuccess = ReticleSettings.CycleReticleNightVisionMode();
	if (bSuccess)
	{
		UpdateReticleBrightness();
	}
	OnNightVisionModeChanged.Broadcast(ReticleSettings.bUsingReticleNightVisionBrightness, bSuccess);
}

void USKGOpticComponent::ZoomIn()
{
	if (OpticSceneCaptureComponent && MagnificationSettings.SetNextMagnification())
	{
		OpticSceneCaptureComponent->Zoom(ReticleSettings.GetReticleMaterial(), MagnificationSettings.GetCurrentMagnification());
		OnZoomChanged.Broadcast(true);
	}
}

void USKGOpticComponent::ZoomOut()
{
	if (OpticSceneCaptureComponent && MagnificationSettings.SetPreviousMagnification())
	{
		OpticSceneCaptureComponent->Zoom(ReticleSettings.GetReticleMaterial(), MagnificationSettings.GetCurrentMagnification());
		OnZoomChanged.Broadcast(false);
	}
}

int32 USKGOpticComponent::PointOfImpactUpDownDefault()
{
	const int32 Clicks = ZeroUpDownClicks;
	ZeroUpDownClicks = 0;
	if (OpticSceneCaptureComponent)
	{
		OpticSceneCaptureComponent->PointOfImpactUpDownDefault();
		OnPointOfImpactUpDownReset.Broadcast();
	}
	return Clicks;
}

int32 USKGOpticComponent::PointOfImpactLeftRightDefault()
{
	const int32 Clicks = ZeroLeftRightClicks;
	ZeroLeftRightClicks = 0;
	if (OpticSceneCaptureComponent)
	{
		OpticSceneCaptureComponent->PointOfImpactLeftRightDefault();
		OnPointOfImpactLeftRightReset.Broadcast();
	}
	return Clicks;
}

void USKGOpticComponent::PointOfImpactUp(const int32 Clicks)
{
	if (OpticSceneCaptureComponent)
	{
		ZeroUpDownClicks += Clicks;
		const double AmountToAdjust = OpticZeroSettings.GetAdjustmentAmount() * Clicks;
		OpticSceneCaptureComponent->PointOfImpactUp(AmountToAdjust);
		OnPointOfImpactChanged.Broadcast();
		OnPointOfImpactUpChanged.Broadcast(Clicks);
	}
}

void USKGOpticComponent::PointOfImpactDown(const int32 Clicks)
{
	if (OpticSceneCaptureComponent)
	{
		ZeroUpDownClicks -= Clicks;
		const double AmountToAdjust = OpticZeroSettings.GetAdjustmentAmount() * Clicks;
		OpticSceneCaptureComponent->PointOfImpactDown(AmountToAdjust);
		OnPointOfImpactChanged.Broadcast();
		OnPointOfImpactDownChanged.Broadcast(Clicks);
	}
}

void USKGOpticComponent::PointOfImpactLeft(const int32 Clicks)
{
	if (OpticSceneCaptureComponent)
	{
		ZeroLeftRightClicks += Clicks;
		const double AmountToAdjust = OpticZeroSettings.GetAdjustmentAmount() * Clicks;
		OpticSceneCaptureComponent->PointOfImpactLeft(AmountToAdjust);
		OnPointOfImpactChanged.Broadcast();
		OnPointOfImpactLeftChanged.Broadcast(Clicks);
	}
}

void USKGOpticComponent::PointOfImpactRight(const int32 Clicks)
{
	if (OpticSceneCaptureComponent)
	{
		ZeroLeftRightClicks -= Clicks;
		const double AmountToAdjust = OpticZeroSettings.GetAdjustmentAmount() * Clicks;
		OpticSceneCaptureComponent->PointOfImpactRight(AmountToAdjust);
		OnPointOfImpactChanged.Broadcast();
		OnPointOfImpactRightChanged.Broadcast(Clicks);
	}
}

void USKGOpticComponent::ApplyLookAtRotationZero(const FRotator& LookAtRotation)
{
	if (OpticSceneCaptureComponent)
	{
		OpticSceneCaptureComponent->ApplyLookAtRotationZero(LookAtRotation);
	}
	else // Apply via material here
	{

	}
}

void USKGOpticComponent::StartSceneCapture()
{
	if (OpticSceneCaptureComponent)
	{
		OpticMesh->SetMaterial(ReticleSettings.ReticleMaterialIndex, ReticleSettings.GetReticleMaterial().DynamicReticleMaterial);
		UpdateReticleBrightness();
		OpticSceneCaptureComponent->Zoom(ReticleSettings.GetReticleMaterial(), MagnificationSettings.GetCurrentMagnification());
		OpticSceneCaptureComponent->StartCapture();
		OnSceneCaptureStateChanged.Broadcast(true);
	}
}

void USKGOpticComponent::StartedAiming()
{
	GetWorld()->GetTimerManager().ClearTimer(UnAimedTimerHandle);
	StartSceneCapture();
}

void USKGOpticComponent::StopSceneCapture()
{
	if (OpticSceneCaptureComponent)
	{
		OpticSceneCaptureComponent->StopCapture();
		if (ReticleSettings.UnAimedReticleMaterial)
		{
			OpticMesh->SetMaterial(ReticleSettings.ReticleMaterialIndex, ReticleSettings.UnAimedReticleMaterial.DynamicReticleMaterial);
			OpticSceneCaptureComponent->Zoom(ReticleSettings.UnAimedReticleMaterial, MagnificationSettings.GetCurrentMagnification());
		}
		OnSceneCaptureStateChanged.Broadcast(false);
	}
}

void USKGOpticComponent::StoppedAiming()
{
	if (ReticleSettings.UnAimedCaptureDelay > 0.0f && OpticSceneCaptureComponent)
	{
		GetWorld()->GetTimerManager().SetTimer(UnAimedTimerHandle, this, &USKGOpticComponent::StopSceneCapture, ReticleSettings.UnAimedCaptureDelay, false);
	}
	else
	{
		StopSceneCapture();
	}
}
