// Copyright 2024, Dakota Dawe, All rights reserved


#include "Actors/SKGLightLaser.h"
#include "Components/SKGLightLaserComponent.h"
#include "Components/SpotLightComponent.h"
#include "DataAssets/SKGPDALightLaser.h"
#include "Statics/SKGShooterFrameworkActorConstructionHelpers.h"

#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


ASKGLightLaser::ASKGLightLaser()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->ComponentTags.Add(MeshComponentTagName);
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	SkeletalMeshComponent->ComponentTags.Add(MeshComponentTagName);
	
	LaserMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserMeshComponent"));
	LaserMeshComponent->SetupAttachment(RootComponent);
	LaserDotMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserDotMeshComponent"));
	LaserDotMeshComponent->SetupAttachment(RootComponent);
	SpotlightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotlightComponent"));
	SpotlightComponent->SetupAttachment(RootComponent);
	SpotlightComponent->Intensity = 50.0f;
	SpotlightComponent->AttenuationRadius = 10000.0f;
	SpotlightComponent->InnerConeAngle = 8.0f;
	SpotlightComponent->OuterConeAngle = 18.0f;
	SpotlightComponent->bUseInverseSquaredFalloff = false;
	
	LightLaserComponent = CreateDefaultSubobject<USKGLightLaserComponent>(TEXT("LightLaserComponent"));
	LightLaserComponent->bAutoInitialize = false;
	LightLaserComponent->bOverrideComponentNames = true;
	LightLaserComponent->LaserMeshComponentName = LaserMeshComponent->GetFName();
	LightLaserComponent->LaserDotComponentName = LaserDotMeshComponent->GetFName();
	LightLaserComponent->LightComponentName = SpotlightComponent->GetFName();
}

void ASKGLightLaser::BeginPlay()
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

void ASKGLightLaser::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_Always;
	Params.Condition = COND_InitialOnly;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ASKGLightLaser, DAConstruction, Params);
}

void ASKGLightLaser::OnBundlesLoaded()
{
	if (bIsUsingStaticMesh)
	{
		Cast<UStaticMeshComponent>(MeshComponent)->SetStaticMesh(DAConstruction->StaticMesh.Get());
	}
	else
	{
		Cast<USkeletalMeshComponent>(MeshComponent)->SetSkeletalMesh(DAConstruction->SkeletalMesh.Get());
	}
	
	if (DAConstruction->bIsLaser)
	{
		LaserMeshComponent->SetStaticMesh(DAConstruction->LaserMesh.Get());
		LaserDotMeshComponent->SetStaticMesh(DAConstruction->LaserDotMesh.Get());
		
		LaserMeshComponent->AttachToComponent(MeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, DAConstruction->LaserMeshAttachSocket);
		LaserDotMeshComponent->SetWorldScale3D(FVector(DAConstruction->LaserDotScale));
	}
	else
	{
		LaserMeshComponent->DestroyComponent();
		LaserDotMeshComponent->DestroyComponent();
	}
	
	if (DAConstruction->bIsLight)
	{
		SpotlightComponent->AttachToComponent(MeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, DAConstruction->LightAttachSocket);
	}
	else
	{
		SpotlightComponent->DestroyComponent();
	}

	LightLaserComponent->InitializeLightLaserComponent();
	
	Super::OnBundlesLoaded();
}

void ASKGLightLaser::OnRep_DAConstruction()
{
	if (!bIsInitialized)
	{
		GameplayTags = DAConstruction->GameplayTags;
		
		MeshComponent = USKGShooterFrameworkActorConstructionHelpers::SetupAttachments(DAConstruction->StaticMesh, DAConstruction->SkeletalMesh, StaticMeshComponent, SkeletalMeshComponent, bIsUsingStaticMesh);
		checkf(MeshComponent, TEXT("NO Static Or Skeletal mesh assigned to data asset: %s"), *DAConstruction->GetName());
		
		MeshComponent->SetCollisionProfileName(DAConstruction->MeshCollisionProfile.Name);
		
		LightLaserComponent->InitializationSettingsDataAsset = DAConstruction->LightLaserInitializeSettings;
		LightLaserComponent->LightLaserSettingsDataAsset = DAConstruction->LightLaserSettings;
	
		UAssetManager::Get().LoadPrimaryAsset(DAConstruction->GetPrimaryAssetId(), BundlesToLoad, FStreamableDelegate().CreateUObject(this, &ASKGLightLaser::OnBundlesLoaded));
		bIsInitialized = true;
	}
}

void ASKGLightLaser::SetConstructionDataAsset(UDataAsset* DataAsset)
{
	if (!bIsInitialized && DAConstruction != DataAsset)
	{
		DAConstruction = CastChecked<USKGPDALightLaser>(DataAsset);
		if (HasAuthority())
		{
			MARK_PROPERTY_DIRTY_FROM_NAME(ASKGLightLaser, DAConstruction, this);
		}
		OnRep_DAConstruction();
		USKGShooterFrameworkActorConstructionHelpers::ConstructAttachment(DAConstruction->AttachmentData, MeshComponent);
	}
}

UPrimaryDataAsset* ASKGLightLaser::GetConstructionDataAsset()
{
	return DAConstruction;
}
