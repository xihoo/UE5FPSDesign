// Copyright 2024, Dakota Dawe, All rights reserved


#include "Actors/SKGActor.h"
#include "Statics/SKGShooterFrameworkActorConstructionHelpers.h"
#include "DataAssets/SKGPDAActor.h"

#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


ASKGActor::ASKGActor()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->ComponentTags.Add(MeshComponentTagName);
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	SkeletalMeshComponent->ComponentTags.Add(MeshComponentTagName);
}

void ASKGActor::BeginPlay()
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

void ASKGActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_Always;
	Params.Condition = COND_InitialOnly;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ASKGActor, DAConstruction, Params);
}

void ASKGActor::OnBundlesLoaded()
{
	if (bIsUsingStaticMesh)
	{
		Cast<UStaticMeshComponent>(MeshComponent)->SetStaticMesh(DAConstruction->StaticMesh.Get());
	}
	else
	{
		Cast<USkeletalMeshComponent>(MeshComponent)->SetSkeletalMesh(DAConstruction->SkeletalMesh.Get());
	}

	Super::OnBundlesLoaded();
}

void ASKGActor::OnRep_DAConstruction()
{
	if (!bIsInitialized)
	{
		GameplayTags = DAConstruction->GameplayTags;
		
		MeshComponent = USKGShooterFrameworkActorConstructionHelpers::SetupAttachments(DAConstruction->StaticMesh, DAConstruction->SkeletalMesh, StaticMeshComponent, SkeletalMeshComponent, bIsUsingStaticMesh);
		checkf(MeshComponent, TEXT("NO Static Or Skeletal mesh assigned to data asset: %s"), *DAConstruction->GetName());
		
		MeshComponent->SetCollisionProfileName(DAConstruction->MeshCollisionProfile.Name);
		
		UAssetManager::Get().LoadPrimaryAsset(DAConstruction->GetPrimaryAssetId(), BundlesToLoad, FStreamableDelegate().CreateUObject(this, &ASKGActor::OnBundlesLoaded));
		bIsInitialized = true;
	}
}

void ASKGActor::SetConstructionDataAsset(UDataAsset* DataAsset)
{
	if (!bIsInitialized && DAConstruction != DataAsset)
	{
		DAConstruction = CastChecked<USKGPDAActor>(DataAsset);
		if (HasAuthority())
		{
			MARK_PROPERTY_DIRTY_FROM_NAME(ASKGActor, DAConstruction, this);
		}
		OnRep_DAConstruction();
		USKGShooterFrameworkActorConstructionHelpers::ConstructAttachment(DAConstruction->AttachmentData, MeshComponent);
	}
}

UPrimaryDataAsset* ASKGActor::GetConstructionDataAsset()
{
	return DAConstruction;
}
