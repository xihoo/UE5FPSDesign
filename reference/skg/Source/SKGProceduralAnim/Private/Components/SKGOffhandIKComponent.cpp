// Copyright 2023, Dakota Dawe, All rights reserved

#include "Components/SKGOffhandIKComponent.h"
#include "DataAssets/Initialize/SKGPDAOffhandIKInitialize.h"
#include "DataAssets/SKGPDAOffhandIKSettings.h"

#include "Components/MeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

namespace SKGGAMEPLAYTAGS
{
	UE_DEFINE_GAMEPLAY_TAG(OffhandIKComponentFirearm, "OffhandIKComponentType.Firearm");
	UE_DEFINE_GAMEPLAY_TAG(OffhandIKComponentHandguard, "OffhandIKComponentType.Handguard");
	UE_DEFINE_GAMEPLAY_TAG(OffhandIKComponentForwardGrip, "OffhandIKComponentType.ForwardGrip");
}

USKGOffhandIKComponent::USKGOffhandIKComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bWantsInitializeComponent = true;
}

USKGOffhandIKComponent* USKGOffhandIKComponent::GetOffhandIKComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<USKGOffhandIKComponent>() : nullptr;
}

void USKGOffhandIKComponent::InitializeOffhandIKComponent()
{
	InitializeComponentFromData();
	// Temporary logic until full change over to data asset
	if (!OffhandIKSettingsDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("USKGOffhandIKComponent on Actor: %s is NOT using the OffhandIKSettingsDataAsset for configuration. Please switch to the new system before updating as it will be mandatory in a future version"), *GetOwner()->GetName());
	}

	SetupComponents();
}

void USKGOffhandIKComponent::BeginPlay()
{
	Super::BeginPlay();

}

void USKGOffhandIKComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	if (bAutoInitialize)
	{
		InitializeOffhandIKComponent();
	}
}

void USKGOffhandIKComponent::InitializeComponentFromData()
{
	if (InitializationSettingsDataAsset)
	{
		if (!bOverrideComponentNames)
		{
			OffhandIKMeshName = InitializationSettingsDataAsset->OffhandIKMeshName;
		}
		LeftHandIKSocketName = InitializationSettingsDataAsset->LeftHandIKSocketName;
		RightHandIKSocketName = InitializationSettingsDataAsset->RightHandIKSocketName;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InitializationSettingsDataAsset NOT set on Actor: %s on Component: %s Please utilize this data asset as it will be mandatory moving forwards"), *GetOwner()->GetName(), *GetName());
	}
	if (OffhandIKSettingsDataAsset)
	{
		LeftHandIKPose = OffhandIKSettingsDataAsset->LeftHandIKPose;
		RightHandIKPose = OffhandIKSettingsDataAsset->RightHandIKPose;
	}
}

void USKGOffhandIKComponent::SetupComponents()
{
	ensureAlwaysMsgf(!OffhandIKMeshName.IsEqual(NAME_None), TEXT("Off Hand IK Mesh Name must be valid on Actor: %s in the SKGOffhandIKComponent: %s"), *GetOwner()->GetName(), *GetName());
	for (UActorComponent* Component : GetOwner()->GetComponents())
	{
		if (Component && Component->GetFName().IsEqual(OffhandIKMeshName))
		{
			OffhandIKMesh = Cast<UMeshComponent>(Component);
			ensureAlwaysMsgf(OffhandIKMesh->DoesSocketExist(LeftHandIKSocketName) || OffhandIKMesh->DoesSocketExist(RightHandIKSocketName), TEXT("Attach To Mesh on Actor: %s on Component: %s of Name: %s Does NOT contain either socket: %s | %s in the SKGOffhandIKComponent: %s"), *GetOwner()->GetName(), *Component->GetName(), *OffhandIKMeshName.ToString(), *LeftHandIKSocketName.ToString(), *RightHandIKSocketName.ToString(), *GetName());
			// Incase it's not attached to anything (such as holding a light/optic it will work)
			break;
		}
	}

	ensureAlwaysMsgf(OffhandIKMesh, TEXT("Could not find mesh with name: %s on Actor: %s in the SKGOffhandIKComponent: %s. Ensure it exists and the name matches what is entered in Off Hand IK Mesh Name"), *OffhandIKMeshName.ToString(), *GetOwner()->GetName(), *GetName());

	if (OffhandIKOffset.Equals(FTransform()))
	{
		UpdateOffhandIK(OffhandIKMesh, true);
	}
}

FTransform USKGOffhandIKComponent::GetOffhandIKWorldTransform(bool bLeftHand) const
{
	return OffhandIKMesh->GetSocketTransform(GetHandSocketName(bLeftHand), RTS_World);
}

const FName& USKGOffhandIKComponent::GetHandSocketName(bool bLeftHand) const
{
	return bLeftHand ? LeftHandIKSocketName : RightHandIKSocketName;
}

void USKGOffhandIKComponent::UpdateOffhandIK(UPrimitiveComponent* ComponentRelativeTo, bool bLeftHand)
{
	if (OffhandIKMesh)
	{
		if (ComponentRelativeTo)
		{
			OffhandIKOffset = UKismetMathLibrary::MakeRelativeTransform(GetOffhandIKWorldTransform(bLeftHand), ComponentRelativeTo->GetComponentTransform());
		}
		else
		{
			UpdateOffhandIK(OffhandIKMesh, true);
		}
	}
	OffhandIKPose = bLeftHand ? LeftHandIKPose : RightHandIKPose;
}
