// Copyright 2024, Dakota Dawe, All rights reserved


#include "Actors/SKGGrip.h"
#include "Components/SKGOffhandIKComponent.h"
#include "Statics/SKGShooterFrameworkActorConstructionHelpers.h"

#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DataAssets/SKGPDAGrip.h"
#include "Engine/AssetManager.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


ASKGGrip::ASKGGrip()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->ComponentTags.Add(MeshComponentTagName);
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	SkeletalMeshComponent->ComponentTags.Add(MeshComponentTagName);
	
	OffhandIKComponent = CreateDefaultSubobject<USKGOffhandIKComponent>(TEXT("OffhandIKComponent"));
	OffhandIKComponent->bAutoInitialize = false;
	OffhandIKComponent->bOverrideComponentNames = true;
}

void ASKGGrip::BeginPlay()
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

void ASKGGrip::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_Always;
	Params.Condition = COND_InitialOnly;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ASKGGrip, DAConstruction, Params);
}

void ASKGGrip::OnBundlesLoaded()
{
	if (bIsUsingStaticMesh)
	{
		Cast<UStaticMeshComponent>(MeshComponent)->SetStaticMesh(DAConstruction->StaticMesh.Get());
	}
	else
	{
		Cast<USkeletalMeshComponent>(MeshComponent)->SetSkeletalMesh(DAConstruction->SkeletalMesh.Get());
	}
	
	OffhandIKComponent->InitializeOffhandIKComponent();
	
	Super::OnBundlesLoaded();
}

void ASKGGrip::OnRep_DAConstruction()
{
	if (!bIsInitialized)
	{
		GameplayTags = DAConstruction->GameplayTags;
		
		MeshComponent = USKGShooterFrameworkActorConstructionHelpers::SetupAttachments(DAConstruction->StaticMesh, DAConstruction->SkeletalMesh, StaticMeshComponent, SkeletalMeshComponent, bIsUsingStaticMesh);
		checkf(MeshComponent, TEXT("NO Static Or Skeletal mesh assigned to data asset: %s"), *DAConstruction->GetName());

		MeshComponent->SetCollisionProfileName(DAConstruction->MeshCollisionProfile.Name);
		
		OffhandIKComponent->InitializationSettingsDataAsset = DAConstruction->OffhandIKInitializeSettings;
		OffhandIKComponent->OffhandIKSettingsDataAsset = DAConstruction->OffhandIKSettings;
		OffhandIKComponent->OffhandIKMeshName = MeshComponent->GetFName();
		OffhandIKComponent->GameplayTags.AppendTags(DAConstruction->GameplayTags);
	
		UAssetManager::Get().LoadPrimaryAsset(DAConstruction->GetPrimaryAssetId(), BundlesToLoad, FStreamableDelegate().CreateUObject(this, &ASKGGrip::OnBundlesLoaded));
		bIsInitialized = true;
	}
}

void ASKGGrip::SetConstructionDataAsset(UDataAsset* DataAsset)
{
	if (!bIsInitialized && DAConstruction != DataAsset)
	{
		DAConstruction = CastChecked<USKGPDAGrip>(DataAsset);
		if (HasAuthority())
		{
			MARK_PROPERTY_DIRTY_FROM_NAME(ASKGGrip, DAConstruction, this);
		}
		OnRep_DAConstruction();
		USKGShooterFrameworkActorConstructionHelpers::ConstructAttachment(DAConstruction->AttachmentData, MeshComponent);
	}
}

UPrimaryDataAsset* ASKGGrip::GetConstructionDataAsset()
{
	return DAConstruction;
}
