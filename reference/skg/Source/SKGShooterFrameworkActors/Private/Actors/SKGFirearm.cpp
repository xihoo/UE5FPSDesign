// Copyright 2024, Dakota Dawe, All rights reserved


#include "Actors/SKGFirearm.h"
#include "Components/SKGAttachmentManagerComponent.h"
#include "Components/SKGFirearmComponent.h"
#include "Components/SKGProceduralAnimComponent.h"
#include "Components/SKGOffhandIKComponent.h"
#include "DataAssets/SKGPDAFirearm.h"
#include "Statics/SKGShooterFrameworkActorConstructionHelpers.h"

#include "Engine/SkeletalMesh.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


ASKGFirearm::ASKGFirearm()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
#if ENGINE_MINOR_VERSION >= 5
	SetNetUpdateFrequency(15.0f);
#else
	NetUpdateFrequency = 15.0f;
#endif

	FirearmMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirearmMeshComponent"));
	FirearmMeshComponent->ComponentTags.Add(MeshComponentTagName);
	RootComponent = FirearmMeshComponent;

	AttachmentManagerComponent = CreateDefaultSubobject<USKGAttachmentManagerComponent>(TEXT("AttachmentManagerComponent"));

	FirearmComponent = CreateDefaultSubobject<USKGFirearmComponent>(TEXT("FirearmComponent"));
	FirearmComponent->bAutoInitialize = false;
	FirearmComponent->FirearmMeshComponentName = FirearmMeshComponent->GetFName();
	FirearmComponent->AttachmentManagerComponentName = AttachmentManagerComponent->GetFName();
	
	ProceduralAnimComponent = CreateDefaultSubobject<USKGProceduralAnimComponent>(TEXT("ProceduralAnimComponent"));
	ProceduralAnimComponent->bAutoInitialize = false;
	ProceduralAnimComponent->bOverrideComponentNames = true;
	ProceduralAnimComponent->ProceduralMeshName = FirearmMeshComponent->GetFName();

	MuzzleComponent = CreateDefaultSubobject<USKGMuzzleComponent>(TEXT("MuzzleComponent"));
	MuzzleComponent->bAutoInitialize = false;
	MuzzleComponent->bOverrideComponentNames = true;
	MuzzleComponent->MuzzleMeshComponentName = FirearmMeshComponent->GetFName();
}

void ASKGFirearm::BeginPlay()
{
	Super::BeginPlay();
	if (!bIsInitialized && DAConstruction)
	{
		OnRep_DAConstruction();
	}
}

void ASKGFirearm::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_Always;
	Params.Condition = COND_InitialOnly;

	DOREPLIFETIME_WITH_PARAMS_FAST(ASKGFirearm, DAConstruction, Params);
}

void ASKGFirearm::OnBundlesLoaded()
{
	FirearmMeshComponent->SetSkeletalMesh(DAConstruction->Mesh.Get());
	ProceduralAnimComponent->InitializeProceduralAnimComponent();
	if (MuzzleComponent)
	{
		MuzzleComponent->InitializeMuzzleComponent();
	}
	if (OffhandIKComponent)
	{
		OffhandIKComponent->InitializeOffhandIKComponent();
	}
	
	FirearmComponent->InitializeFirearmComponent();

	if (HasAuthority())
	{
		USKGShooterFrameworkActorConstructionHelpers::ConstructAttachment(DAConstruction->AttachmentData, FirearmMeshComponent);
	}

	Super::OnBundlesLoaded();
}

void ASKGFirearm::OnRep_DAConstruction()
{
	if (!bIsInitialized)
	{
		GameplayTags = DAConstruction->GameplayTags;
		FirearmMeshComponent->SetCollisionProfileName(DAConstruction->MeshCollisionProfile.Name);
	
		ProceduralAnimComponent->InitializationSettingsDataAsset = DAConstruction->ProceduralAnimInitializeSettings;
		ProceduralAnimComponent->AimingSettingsDataAsset = DAConstruction->AimingSettings;
		ProceduralAnimComponent->ProceduralOffsetsDataAsset = DAConstruction->ProceduralOffsetsSettings;
		ProceduralAnimComponent->CycleAimingPointSettingsDataAsset = DAConstruction->CycleAimingPointSettings;
		ProceduralAnimComponent->MovementSwaySettingsDataAsset = DAConstruction->MovementSwaySettings;
		ProceduralAnimComponent->MovementLagSettingsDataAsset = DAConstruction->MovementLagSettings;
		ProceduralAnimComponent->RotationLagSettingsDataAsset = DAConstruction->RotationLagSettings;
		ProceduralAnimComponent->RecoilSettingsDataAsset = DAConstruction->RecoilSettings;
		ProceduralAnimComponent->DeadzoneSettingsDataAsset = DAConstruction->DeadzoneSettings;
		ProceduralAnimComponent->PoseSettingsDataAssets = DAConstruction->Poses;

		FirearmComponent->FirearmCollisionSettingsDataAsset = DAConstruction->FirearmCollisionSettings;

		if (!DAConstruction->bHasMuzzle)
		{
			MuzzleComponent->DestroyComponent();
		}
		if (DAConstruction->bHasOffhandIKComponent)
		{
			OffhandIKComponent = NewObject<USKGOffhandIKComponent>(this);
			OffhandIKComponent->bAutoInitialize = false;
			OffhandIKComponent->bOverrideComponentNames = true;
			OffhandIKComponent->OffhandIKMeshName = FirearmMeshComponent->GetFName();
			OffhandIKComponent->InitializationSettingsDataAsset = DAConstruction->OffhandIKInitializeSettings;
			OffhandIKComponent->OffhandIKSettingsDataAsset = DAConstruction->OffhandIKSettings;
			OffhandIKComponent->RegisterComponent();
		}

		UAssetManager::Get().LoadPrimaryAsset(DAConstruction->GetPrimaryAssetId(), BundlesToLoad, FStreamableDelegate().CreateUObject(this, &ASKGFirearm::OnBundlesLoaded));
		bIsInitialized = true;
	}
}

void ASKGFirearm::SetConstructionDataAsset(UDataAsset* DataAsset)
{
	if (!bIsInitialized && DAConstruction != DataAsset)
	{
		DAConstruction = CastChecked<USKGPDAFirearm>(DataAsset);
		if (HasAuthority())
		{
			MARK_PROPERTY_DIRTY_FROM_NAME(ASKGFirearm, DAConstruction, this);
		}
		OnRep_DAConstruction();
	}
}

UPrimaryDataAsset* ASKGFirearm::GetConstructionDataAsset()
{
	return DAConstruction;
}
