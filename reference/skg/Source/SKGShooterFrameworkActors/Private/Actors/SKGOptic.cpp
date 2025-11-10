// Copyright 2024, Dakota Dawe, All rights reserved


#include "Actors/SKGOptic.h"
#include "Statics/SKGShooterFrameworkActorConstructionHelpers.h"
#include "Components/SKGOpticComponent.h"
#include "Components/SKGProceduralAnimComponent.h"
#include "Components/SKGOpticSceneCaptureComponent.h"
#include "DataAssets/SKGPDAOptic.h"

#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

ASKGOptic::ASKGOptic()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->ComponentTags.Add(MeshComponentTagName);
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	SkeletalMeshComponent->ComponentTags.Add(MeshComponentTagName);

	OpticSceneCaptureComponent = CreateDefaultSubobject<USKGOpticSceneCaptureComponent>(TEXT("OpticSceneCaptureComponent"));
	OpticSceneCaptureComponent->SetupAttachment(RootComponent);
	
	OpticComponent = CreateDefaultSubobject<USKGOpticComponent>(TEXT("OpticComponent"));
	OpticComponent->bAutoInitialize = false;
	OpticComponent->bOverrideComponentNames = true;
	OpticComponent->OpticSceneCaptureComponentName = OpticSceneCaptureComponent->GetFName();
	ProceduralAnimComponent = CreateDefaultSubobject<USKGProceduralAnimComponent>(TEXT("ProceduralAnimComponent"));
	ProceduralAnimComponent->bAutoInitialize = false;
	ProceduralAnimComponent->bOverrideComponentNames = true;
}

void ASKGOptic::BeginPlay()
{
	Super::BeginPlay();
	
	if (!bIsInitialized && DAConstruction)
	{
		OnRep_DAConstruction();
		if (HasAuthority())
		{
			USKGShooterFrameworkActorConstructionHelpers::ConstructAttachment(DAConstruction->AttachmentData, MeshComponent);
		}
	}
}

void ASKGOptic::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_Always;
	Params.Condition = COND_InitialOnly;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ASKGOptic, DAConstruction, Params);
}

void ASKGOptic::OnBundlesLoaded()
{
	if (bIsUsingStaticMesh)
	{
		Cast<UStaticMeshComponent>(MeshComponent)->SetStaticMesh(DAConstruction->StaticMesh.Get());
	}
	else
	{
		Cast<USkeletalMeshComponent>(MeshComponent)->SetSkeletalMesh(DAConstruction->SkeletalMesh.Get());
	}
	
	if (DAConstruction->bHasSceneCaptureComponent)
	{
		OpticSceneCaptureComponent->AttachToComponent(MeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, DAConstruction->SceneCaptureAttachSocketName);
	}
	
	OpticComponent->InitializeOpticComponent();
	ProceduralAnimComponent->InitializeProceduralAnimComponent();
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	Super::OnBundlesLoaded();
}

void ASKGOptic::OnRep_DAConstruction()
{
	if (!bIsInitialized)
	{
		GameplayTags = DAConstruction->GameplayTags;
		
		MeshComponent = USKGShooterFrameworkActorConstructionHelpers::SetupAttachments(DAConstruction->StaticMesh, DAConstruction->SkeletalMesh, StaticMeshComponent, SkeletalMeshComponent, bIsUsingStaticMesh);
		checkf(MeshComponent, TEXT("NO Static Or Skeletal mesh assigned to data asset: %s"), *DAConstruction->GetName());

		MeshComponent->SetCollisionProfileName(DAConstruction->MeshCollisionProfile.Name);
		
		if (!DAConstruction->bHasSceneCaptureComponent)
		{
			OpticSceneCaptureComponent->DestroyComponent();
		}

		OpticComponent->bOverrideComponentNames = true;
		OpticComponent->InitializationSettingsDataAsset = DAConstruction->OpticInitializeSettings;
		OpticComponent->OpticMeshName = MeshComponent->GetFName();
		OpticComponent->ReticleSettingsDataAsset = DAConstruction->ReticleSetting;
		OpticComponent->MagnificationSettingsDataAsset = DAConstruction->MagnificationSettings;
		OpticComponent->OpticZeroSettingsDataAsset = DAConstruction->OpticZeroSettings;

		ProceduralAnimComponent->bOverrideComponentNames = true;
		ProceduralAnimComponent->InitializationSettingsDataAsset = DAConstruction->ProceduralAnimInitializeSettings;
		ProceduralAnimComponent->ProceduralMeshName = MeshComponent->GetFName();
		ProceduralAnimComponent->AimingSettingsDataAsset = DAConstruction->AimingSettings;
	
		UAssetManager::Get().LoadPrimaryAsset(DAConstruction->GetPrimaryAssetId(), BundlesToLoad, FStreamableDelegate().CreateUObject(this, &ASKGOptic::OnBundlesLoaded));
		bIsInitialized = true;
	}
}

void ASKGOptic::SetConstructionDataAsset(UDataAsset* DataAsset)
{
	if (!bIsInitialized && DAConstruction != DataAsset)
	{
		DAConstruction = CastChecked<USKGPDAOptic>(DataAsset);
		if (HasAuthority())
		{
			MARK_PROPERTY_DIRTY_FROM_NAME(ASKGOptic, DAConstruction, this);
		}
		OnRep_DAConstruction();
		USKGShooterFrameworkActorConstructionHelpers::ConstructAttachment(DAConstruction->AttachmentData, MeshComponent);
	}
}

UPrimaryDataAsset* ASKGOptic::GetConstructionDataAsset()
{
	return DAConstruction;
}