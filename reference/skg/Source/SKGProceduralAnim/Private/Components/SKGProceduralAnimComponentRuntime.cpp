#include "Animation/AnimExecutionContext.h"
#include "Components/SKGProceduralAnimComponent.h"
#include "DataAssets/Initialize/SKGPDAProceduralAnimInitialize.h"
#include "DataAssets/SKGPDAAimingSettings.h"
#include "DataAssets/SKGPDAMovementSwaySettings.h"
#include "DataAssets/SKGPDAMovementLagSettings.h"
#include "DataAssets/SKGPDARotationLagSettings.h"
#include "DataAssets/SKGPDADeadzoneSettings.h"
#include "DataAssets/SKGPDAProceduralOffsets.h"
#include "DataAssets/SKGPDARecoilSettings.h"
#include "DataAssets/SKGPDAProceduralPoseSettings.h"
#include "DataAssets/SKGPDACycleAimingPointSettings.h"

#include "Net/Core/PushModel/PushModel.h"


void USKGProceduralAnimComponent::SetAimingSettingsSettings(USKGPDAAimingSettings* DataAsset)
{
	if (!DefaultProceduralSettingsDataAssets.DefaultAimingSettings)
	{
		DefaultProceduralSettingsDataAssets.DefaultAimingSettings = AimingSettingsDataAsset;
	}
	AimingSettingsDataAsset = DataAsset;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, AimingSettingsDataAsset, this);
	OnRep_SettingsDataAssetChanged();
}

void USKGProceduralAnimComponent::RevertAimingSettings()
{
	SetAimingSettingsSettings(DefaultProceduralSettingsDataAssets.DefaultAimingSettings);
}

void USKGProceduralAnimComponent::SetProceduralOffsets(USKGPDAProceduralOffsets* DataAsset)
{
	if (!DefaultProceduralSettingsDataAssets.DefaultProceduralOffsets)
	{
		DefaultProceduralSettingsDataAssets.DefaultProceduralOffsets = ProceduralOffsetsDataAsset;
	}
	ProceduralOffsetsDataAsset = DataAsset;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, ProceduralOffsetsDataAsset, this);
	OnRep_SettingsDataAssetChanged();
}

void USKGProceduralAnimComponent::RevertProceduralOffsetsSettings()
{
	SetProceduralOffsets(DefaultProceduralSettingsDataAssets.DefaultProceduralOffsets);
}

void USKGProceduralAnimComponent::SetCycleAimingPointSettings(USKGPDACycleAimingPointSettings* DataAsset)
{
	if (!DefaultProceduralSettingsDataAssets.DefaultCycleAimingPointSettings)
	{
		DefaultProceduralSettingsDataAssets.DefaultCycleAimingPointSettings = CycleAimingPointSettingsDataAsset;
	}
	CycleAimingPointSettingsDataAsset = DataAsset;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, CycleAimingPointSettingsDataAsset, this);
	OnRep_SettingsDataAssetChanged();
}


void USKGProceduralAnimComponent::RevertCycleAimingPointSettings()
{
	SetCycleAimingPointSettings(DefaultProceduralSettingsDataAssets.DefaultCycleAimingPointSettings);
}

void USKGProceduralAnimComponent::SetMovementSwaySettings(USKGPDAMovementSwaySettings* DataAsset)
{
	if (!DefaultProceduralSettingsDataAssets.DefaultMovementSwaySettings)
	{
		DefaultProceduralSettingsDataAssets.DefaultMovementSwaySettings = MovementSwaySettingsDataAsset;
	}
	MovementSwaySettingsDataAsset = DataAsset;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, MovementSwaySettingsDataAsset, this);
	OnRep_SettingsDataAssetChanged();
}

void USKGProceduralAnimComponent::RevertMovementSwaySettings()
{
	SetMovementSwaySettings(DefaultProceduralSettingsDataAssets.DefaultMovementSwaySettings);
}

void USKGProceduralAnimComponent::SetMovementLagSettings(USKGPDAMovementLagSettings* DataAsset)
{
	if (!DefaultProceduralSettingsDataAssets.DefaultMovementLagSettings)
	{
		DefaultProceduralSettingsDataAssets.DefaultMovementLagSettings = MovementLagSettingsDataAsset;
	}
	MovementLagSettingsDataAsset = DataAsset;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, MovementLagSettingsDataAsset, this);
	OnRep_SettingsDataAssetChanged();
}

void USKGProceduralAnimComponent::RevertMovementLagSettings()
{
	SetMovementLagSettings(DefaultProceduralSettingsDataAssets.DefaultMovementLagSettings);
}

void USKGProceduralAnimComponent::SetRotationLagSettings(USKGPDARotationLagSettings* DataAsset)
{
	if (!DefaultProceduralSettingsDataAssets.DefaultRotationLagSettings)
	{
		DefaultProceduralSettingsDataAssets.DefaultRotationLagSettings = RotationLagSettingsDataAsset;
	}
	RotationLagSettingsDataAsset = DataAsset;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, RotationLagSettingsDataAsset, this);
	OnRep_SettingsDataAssetChanged();
}

void USKGProceduralAnimComponent::RevertRotationLagSettings()
{
	SetRotationLagSettings(DefaultProceduralSettingsDataAssets.DefaultRotationLagSettings);
}

void USKGProceduralAnimComponent::SetDeadzoneSettings(USKGPDADeadzoneSettings* DataAsset)
{
	if (!DefaultProceduralSettingsDataAssets.DefaultDeadzoneSettings)
	{
		DefaultProceduralSettingsDataAssets.DefaultDeadzoneSettings = DeadzoneSettingsDataAsset;
	}
	DeadzoneSettingsDataAsset = DataAsset;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, DeadzoneSettingsDataAsset, this);
	OnRep_SettingsDataAssetChanged();
}

void USKGProceduralAnimComponent::RevertDeadzoneSettings()
{
	SetDeadzoneSettings(DefaultProceduralSettingsDataAssets.DefaultDeadzoneSettings);
}

void USKGProceduralAnimComponent::SetRecoilSettings(USKGPDARecoilSettings* DataAsset)
{
	if (!DefaultProceduralSettingsDataAssets.DefaultRecoilSettings)
	{
		DefaultProceduralSettingsDataAssets.DefaultRecoilSettings = RecoilSettingsDataAsset;
	}
	RecoilSettingsDataAsset = DataAsset;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, RecoilSettingsDataAsset, this);
	OnRep_SettingsDataAssetChanged();
}

void USKGProceduralAnimComponent::RevertRecoilSettings()
{
	SetRecoilSettings(DefaultProceduralSettingsDataAssets.DefaultRecoilSettings);
}

void USKGProceduralAnimComponent::SetProceduralPoseSettings(TArray<USKGPDAProceduralPoseSettings*> NewPoses)
{
	if (DefaultProceduralSettingsDataAssets.DefaultPoses.Num() == 0)
	{
		DefaultProceduralSettingsDataAssets.DefaultPoses.Reserve(PoseSettingsDataAssets.Num());
		for (USKGPDAProceduralPoseSettings* Pose : PoseSettingsDataAssets)
		{
			DefaultProceduralSettingsDataAssets.DefaultPoses.Add(Pose);
		}
	}
	PoseSettingsDataAssets = NewPoses;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, PoseSettingsDataAssets, this);
	OnRep_SettingsDataAssetChanged();
}

void USKGProceduralAnimComponent::RevertProceduralPoseSettings()
{
	SetProceduralPoseSettings(DefaultProceduralSettingsDataAssets.DefaultPoses);
}

void USKGProceduralAnimComponent::RevertAllSettingsToDefault()
{
	AimingSettingsDataAsset = DefaultProceduralSettingsDataAssets.DefaultAimingSettings;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, AimingSettingsDataAsset, this);
	ProceduralOffsetsDataAsset = DefaultProceduralSettingsDataAssets.DefaultProceduralOffsets;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, ProceduralOffsetsDataAsset, this);
	CycleAimingPointSettingsDataAsset = DefaultProceduralSettingsDataAssets.DefaultCycleAimingPointSettings;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, CycleAimingPointSettingsDataAsset, this);
	MovementSwaySettingsDataAsset = DefaultProceduralSettingsDataAssets.DefaultMovementSwaySettings;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, MovementSwaySettingsDataAsset, this);
	MovementLagSettingsDataAsset = DefaultProceduralSettingsDataAssets.DefaultMovementLagSettings;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, MovementLagSettingsDataAsset, this);
	RotationLagSettingsDataAsset = DefaultProceduralSettingsDataAssets.DefaultRotationLagSettings;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, RotationLagSettingsDataAsset, this);
	DeadzoneSettingsDataAsset = DefaultProceduralSettingsDataAssets.DefaultDeadzoneSettings;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, DeadzoneSettingsDataAsset, this);
	RecoilSettingsDataAsset = DefaultProceduralSettingsDataAssets.DefaultRecoilSettings;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, RecoilSettingsDataAsset, this);
	PoseSettingsDataAssets = DefaultProceduralSettingsDataAssets.DefaultPoses;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, PoseSettingsDataAssets, this);

	OnRep_SettingsDataAssetChanged();
}

void USKGProceduralAnimComponent::OnRep_SettingsDataAssetChanged()
{
	InitializeComponentFromData();
	OnProceduralDataAssetsChanged.Broadcast();
}
