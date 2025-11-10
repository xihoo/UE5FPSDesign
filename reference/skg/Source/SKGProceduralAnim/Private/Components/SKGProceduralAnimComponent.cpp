// Copyright 2023, Dakota Dawe, All rights reserved


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

#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "Components/MeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


// Sets default values for this component's properties
USKGProceduralAnimComponent::USKGProceduralAnimComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
}

USKGProceduralAnimComponent* USKGProceduralAnimComponent::GetProceduralAnimComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<USKGProceduralAnimComponent>() : nullptr;
}

void USKGProceduralAnimComponent::InitializeProceduralAnimComponent()
{
	InitializeComponentFromData();
	bUsedForAiming = ProceduralAimSocketNames.Num() > 0;
	SetupComponents();
}

void USKGProceduralAnimComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void USKGProceduralAnimComponent::InitializeComponent()
{
	Super::InitializeComponent();
	if (bAutoInitialize)
	{
		InitializeProceduralAnimComponent();
	}
}

void USKGProceduralAnimComponent::InitializeComponentFromData()
{
	if (InitializationSettingsDataAsset)
	{
		if (!bOverrideComponentNames)
		{
			ProceduralMeshName = InitializationSettingsDataAsset->ProceduralMeshName;
		}
		ProceduralAimSocketNames = InitializationSettingsDataAsset->ProceduralAimSocketNames;
		RightHandDominatePointAimAimSocketIndex = InitializationSettingsDataAsset->RightHandDominatePointAimAimSocketIndex;
		LeftHandDominatePointAimAimSocketIndex = InitializationSettingsDataAsset->LeftHandDominatePointAimAimSocketIndex;
		bCanUseLeftHandDominate = InitializationSettingsDataAsset->bCanUseLeftHandDominate;
		ProceduralAnimGameplayTag = InitializationSettingsDataAsset->ProceduralAnimGameplayTag;
		GameplayTags = InitializationSettingsDataAsset->GameplayTags;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InitializationSettingsDataAsset NOT set on Actor: %s on Component: %s Please utilize this data asset as it will be mandatory moving forwards"), *GetOwner()->GetName(), *GetName());
	}
	
	if (AimingSettingsDataAsset)
	{
		AimingSettings = AimingSettingsDataAsset->AimingSettings;
	}

	if (ProceduralOffsetsDataAsset)
	{
		BasePoseOffset.FirstPersonOffset = ProceduralOffsetsDataAsset->FirstPersonBasePoseOffset;
		BasePoseOffset.ThirdPersonOffset = ProceduralOffsetsDataAsset->ThirdPersonBasePoseOffset;
		ThirdPersonLeftHandedAimingOffset = ProceduralOffsetsDataAsset->ThirdPersonLeftHandedAimingOffset;
		ThirdPersonRightHandedAimingOffset = ProceduralOffsetsDataAsset->ThirdPersonRightHandedAimingOffset;
	}

	if (CycleAimingPointSettingsDataAsset)
	{
		CycleAimingPointSettings = CycleAimingPointSettingsDataAsset->CycleAimingPointSettings;
	}
	
	if (MovementSwaySettingsDataAsset)
	{
		MovementSwaySettings.LocationSettings = MovementSwaySettingsDataAsset->LocationSettings;
		MovementSwaySettings.RotationSettings = MovementSwaySettingsDataAsset->RotationSettings;
	}
	else
	{
		MovementSwaySettings.bUseMovementSway = false;
	}

	if (MovementLagSettingsDataAsset)
	{
		MovementLagSettings = MovementLagSettingsDataAsset->MovementLagSettings;
	}
	else
	{
		MovementLagSettings.bUseMovementLag = false;
	}

	if (RotationLagSettingsDataAsset)
	{
		RotationLagSettings = RotationLagSettingsDataAsset->RotationLagSettings;
	}
	else
	{
		RotationLagSettings.bUseRotationLag = false;
	}

	if (DeadzoneSettingsDataAsset)
	{
		DeadzoneSettings = DeadzoneSettingsDataAsset->DeadzoneSettings;
	}
	else
	{
		DeadzoneSettings.bUseDeadzone = false;
	}

	if (RecoilSettingsDataAsset)
	{
		RecoilSettings = RecoilSettingsDataAsset->RecoilSettings;
	}

	if (PoseSettingsDataAssets.Num())
	{
		PoseSettings.Empty(PoseSettingsDataAssets.Num());
		for (const USKGPDAProceduralPoseSettings* PoseSetting : PoseSettingsDataAssets)
		{
			PoseSettings.Emplace(PoseSetting->PoseSettings);
		}
	}
}

void USKGProceduralAnimComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(USKGProceduralAnimComponent, AimSocketIndex, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGProceduralAnimComponent, AimingSettingsDataAsset, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGProceduralAnimComponent, ProceduralOffsetsDataAsset, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGProceduralAnimComponent, CycleAimingPointSettingsDataAsset, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGProceduralAnimComponent, MovementSwaySettingsDataAsset, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGProceduralAnimComponent, MovementLagSettingsDataAsset, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGProceduralAnimComponent, RotationLagSettingsDataAsset, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGProceduralAnimComponent, DeadzoneSettingsDataAsset, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGProceduralAnimComponent, RecoilSettingsDataAsset, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGProceduralAnimComponent, PoseSettingsDataAssets, Params);
}

void USKGProceduralAnimComponent::SetupComponents()
{
	//ensureAlwaysMsgf(ProceduralAimSocketNames.Num(), TEXT("Aim Socket Name must be valid on Actor: %s"), *GetOwner()->GetName());
	for (UActorComponent* Component : GetOwner()->GetComponents())
	{
		if (Component && Component->GetFName().IsEqual(ProceduralMeshName))
		{
			ProceduralAnimMesh = Cast<UMeshComponent>(Component);
			break;
		}
	}

	ensureAlwaysMsgf(ProceduralAnimMesh, TEXT("Could not find mesh with name: %s on Actor: %s. Ensure it exists and the name matches what is entered in ProceduralMeshName"), *ProceduralMeshName.ToString(), *GetOwner()->GetName());

	if (CurrentAimSocketTransform.Equals(FTransform()))
	{
		UpdateAimOffset(ProceduralAnimMesh);
	}
}

void USKGProceduralAnimComponent::OnRep_AimSocketIndex() const
{
	OnAimSocketCycledReplicated.Broadcast(ProceduralAimSocketNames[AimSocketIndex]);
}

bool USKGProceduralAnimComponent::Server_SetAimSocketIndex_Validate(uint8 Index)
{
	return true;
}

void USKGProceduralAnimComponent::Server_SetAimSocketIndex_Implementation(uint8 Index)
{
	if (Index < ProceduralAimSocketNames.Num())
	{
		AimSocketIndex = Index;
		MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, AimSocketIndex, this);
		OnRep_AimSocketIndex();
	}
}

bool USKGProceduralAnimComponent::CycleAimSocket()
{
	if (ProceduralAimSocketNames.Num())
	{
		PreviousAimSocketIndex = AimSocketIndex;
		if (++AimSocketIndex >= ProceduralAimSocketNames.Num())
		{
			AimSocketIndex = 0;
		}

		if (AimSocketIndex != PreviousAimSocketIndex)
		{
			if (HasAuthority())
			{
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, AimSocketIndex, this);
			}
			else
			{
				Server_SetAimSocketIndex(AimSocketIndex);
			}
			OnAimSocketCycled.Broadcast(ProceduralAimSocketNames[AimSocketIndex]);
		}
		return AimSocketIndex != PreviousAimSocketIndex;
	}
	return false;
}

bool USKGProceduralAnimComponent::StartPointAiming(bool bRightHandDominant)
{
	const int32 Index = bRightHandDominant ? RightHandDominatePointAimAimSocketIndex : LeftHandDominatePointAimAimSocketIndex;
	if (Index != INDEX_NONE)
	{
		if (AimSocketIndex != Index)
		{
			PreviousAimSocketIndex = AimSocketIndex;
			AimSocketIndex = Index;
			if (HasAuthority())
			{
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, AimSocketIndex, this);
			}
			else
			{
				Server_SetAimSocketIndex(AimSocketIndex);
			}
			OnPointAimStateChanged.Broadcast(true);
		}
		return true;
	}
	return false;
}

void USKGProceduralAnimComponent::StopPointAiming()
{
	if (AimSocketIndex != PreviousAimSocketIndex)
	{
		AimSocketIndex = PreviousAimSocketIndex;
		if (HasAuthority())
		{
			MARK_PROPERTY_DIRTY_FROM_NAME(USKGProceduralAnimComponent, AimSocketIndex, this);
		}
		else
		{
			Server_SetAimSocketIndex(AimSocketIndex);
		}
		OnPointAimStateChanged.Broadcast(false);
	}
}

FTransform USKGProceduralAnimComponent::GetAimWorldTransform() const
{
	if (bUsedForAiming && ProceduralAnimMesh)
	{
		return ProceduralAnimMesh->GetSocketTransform(ProceduralAimSocketNames[AimSocketIndex], RTS_World);
	}
	return FTransform();
}

FTransform USKGProceduralAnimComponent::GetAimMuzzleTransform_Implementation()
{
	FTransform Transform = FTransform();
	if (bUsedForAiming && ProceduralAnimMesh)
	{
		const FTransform World = GetAimWorldTransform();
		FTransform Relative = ProceduralAnimMesh->GetSocketTransform(ProceduralAimSocketNames[AimSocketIndex], RTS_ParentBoneSpace);
	
		FRotator Rotator = Relative.Rotator();
		Rotator.Yaw += 90.0f;
		Relative.SetRotation(Rotator.Quaternion());
	
		Transform = UKismetMathLibrary::ComposeTransforms(Relative, World);
		Transform.SetLocation(World.GetTranslation());
	}
	return Transform;
}

const FTransform& USKGProceduralAnimComponent::GetThirdPersonAimingOffset(bool bRightHanded) const
{
	return bRightHanded ? ThirdPersonRightHandedAimingOffset : ThirdPersonLeftHandedAimingOffset;
}

void USKGProceduralAnimComponent::UpdateAimOffset(UMeshComponent* ComponentRelativeTo, FVector BaseDistance)
{
	if (bUsedForAiming && ProceduralAnimMesh)
	{
		FTransform Relative;
		if (ComponentRelativeTo)
		{
			Relative = UKismetMathLibrary::MakeRelativeTransform(GetAimWorldTransform(), ComponentRelativeTo->GetComponentTransform());
		}
		else
		{
			Relative = UKismetMathLibrary::MakeRelativeTransform(GetAimWorldTransform(), ProceduralAnimMesh->GetComponentTransform());
		}
		
		FVector Location = Relative.GetTranslation();
		const FRotator RelativeRotator = Relative.Rotator();
		FRotator Rotation;
		
		if (AimingSettings.ForwardAxis == EAxis::Type::Y)
		{
			Location.X = -Location.X;
			const float Distance = AimingSettings.bUseFixedAimingDistance ? -Location.Y : BaseDistance.Y;
			Location.Y = Distance + AimingSettings.AimingDistance;
			Location.Z = -Location.Z;
			Rotation = FRotator(-RelativeRotator.Pitch, RelativeRotator.Yaw, RelativeRotator.Roll);
			
		}
		else if (AimingSettings.ForwardAxis == EAxis::Type::X)
		{
			Location.Y = -Location.Y;
			const float Distance = AimingSettings.bUseFixedAimingDistance ? -Location.X : BaseDistance.X;
			Location.X = Distance + AimingSettings.AimingDistance;
			Location.Z = -Location.Z;
			Rotation = FRotator(RelativeRotator.Roll, RelativeRotator.Yaw, RelativeRotator.Pitch);
		}
		else
		{
			Location.X = -Location.X;
			Location.Y = -Location.Y;
			const float Distance = AimingSettings.bUseFixedAimingDistance ? -Location.Z : BaseDistance.Z;
			Location.Z = Distance + AimingSettings.AimingDistance;
			Rotation = FRotator(-RelativeRotator.Pitch, RelativeRotator.Yaw, RelativeRotator.Roll);
		}
		
		CurrentAimSocketTransform = FTransform(Rotation, Location);
	}
	LastRelativeToOffset = ComponentRelativeTo;
}

void USKGProceduralAnimComponent::UpdateAimOffsetWithSocket(UMeshComponent* ComponentRelativeTo, const FName& Socket)
{
	if (bUsedForAiming && ProceduralAnimMesh)
	{
		FTransform Relative;
		if (ComponentRelativeTo)
		{
			Relative = UKismetMathLibrary::MakeRelativeTransform(GetAimWorldTransform(), ComponentRelativeTo->GetSocketTransform(Socket));
		}
		else
		{
			Relative = UKismetMathLibrary::MakeRelativeTransform(GetAimWorldTransform(), ProceduralAnimMesh->GetComponentTransform());
		}
		FVector Location = Relative.GetLocation();
		Location.X = -Location.X;
		Location.Y = Location.X + 10;
		Location.Z = -Location.Z; // Consider adding Z offset option
		const FRotator Rotation = FRotator(-Relative.Rotator().Pitch, 0.0f, 0.0f);
		
		CurrentAimSocketTransform = FTransform(Rotation, Location);
	}
	LastRelativeToOffset = ComponentRelativeTo;
}

bool USKGProceduralAnimComponent::GetPose(FGameplayTag Tag, FSKGToFromCurveSettings& PoseData)
{
	for (const FSKGToFromCurveSettings& Pose : PoseSettings)
	{
		if (Pose.CurveTag.MatchesTag(Tag))
		{
			PoseData = Pose;
			return true;
		}
	}
	return false;
}
