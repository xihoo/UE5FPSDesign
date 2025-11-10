// Copyright 2024, Dakota Dawe, All rights reserved


#include "Actors/SKGMuzzle.h"
#include "Components/SKGMuzzleComponent.h"
#include "Statics/SKGShooterFrameworkActorConstructionHelpers.h"
#include "DataAssets/SKGPDAMuzzle.h"

#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


ASKGMuzzle::ASKGMuzzle()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->ComponentTags.Add(MeshComponentTagName);
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	SkeletalMeshComponent->ComponentTags.Add(MeshComponentTagName);
	
	MuzzleComponent = CreateDefaultSubobject<USKGMuzzleComponent>(TEXT("MuzzleComponent"));
	MuzzleComponent->bAutoInitialize = false;
	MuzzleComponent->bOverrideComponentNames = true;
	MuzzleComponent->MuzzleMeshComponentName = FName("MeshComponent");
}

void ASKGMuzzle::BeginPlay()
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

void ASKGMuzzle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_Always;
	Params.Condition = COND_InitialOnly;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ASKGMuzzle, DAConstruction, Params);
}

void ASKGMuzzle::OnBundlesLoaded()
{
	if (bIsUsingStaticMesh)
	{
		Cast<UStaticMeshComponent>(MeshComponent)->SetStaticMesh(DAConstruction->StaticMesh.Get());
	}
	else
	{
		Cast<USkeletalMeshComponent>(MeshComponent)->SetSkeletalMesh(DAConstruction->SkeletalMesh.Get());
	}
	
	MuzzleComponent->InitializeMuzzleComponent();
	
	Super::OnBundlesLoaded();
}

void ASKGMuzzle::OnRep_DAConstruction()
{
	if (!bIsInitialized)
	{
		GameplayTags = DAConstruction->GameplayTags;
		
		MeshComponent = USKGShooterFrameworkActorConstructionHelpers::SetupAttachments(DAConstruction->StaticMesh, DAConstruction->SkeletalMesh, StaticMeshComponent, SkeletalMeshComponent, bIsUsingStaticMesh);
		checkf(MeshComponent, TEXT("NO Static Or Skeletal mesh assigned to data asset: %s"), *DAConstruction->GetName());
		
		MeshComponent->SetCollisionProfileName(DAConstruction->MeshCollisionProfile.Name);

		MuzzleComponent->InitializationSettingsDataAsset = DAConstruction->MuzzleInitializeSettings;
		MuzzleComponent->MuzzleMeshComponentName = MeshComponent->GetFName();
		MuzzleComponent->MuzzleSettingsDataAsset = DAConstruction->MuzzleSettings;
	
		UAssetManager::Get().LoadPrimaryAsset(DAConstruction->GetPrimaryAssetId(), BundlesToLoad, FStreamableDelegate().CreateUObject(this, &ASKGMuzzle::OnBundlesLoaded));
		bIsInitialized = true;
	}
}

void ASKGMuzzle::SetConstructionDataAsset(UDataAsset* DataAsset)
{
	if (!bIsInitialized && DAConstruction != DataAsset)
	{
		DAConstruction = CastChecked<USKGPDAMuzzle>(DataAsset);
		if (HasAuthority())
		{
			MARK_PROPERTY_DIRTY_FROM_NAME(ASKGMuzzle, DAConstruction, this);
		}
		OnRep_DAConstruction();
		USKGShooterFrameworkActorConstructionHelpers::ConstructAttachment(DAConstruction->AttachmentData, MeshComponent);
	}
}

UPrimaryDataAsset* ASKGMuzzle::GetConstructionDataAsset()
{
	return DAConstruction;
}
