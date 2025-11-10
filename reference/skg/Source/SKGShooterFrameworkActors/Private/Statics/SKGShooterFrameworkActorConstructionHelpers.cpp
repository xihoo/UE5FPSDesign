// Copyright 2024, Dakota Dawe, All rights reserved


#include "Statics/SKGShooterFrameworkActorConstructionHelpers.h"
#include "Components/SKGAttachmentComponent.h"
#include "DataTypes/SKGShooterFrameworkActorDataTypes.h"
#include "Interfaces/SKGActorConstructionInterface.h"

#include "Engine/World.h"
#include "Components/MeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

void USKGShooterFrameworkActorConstructionHelpers::ConstructAttachment(const FSKGAttachmentComponentData& Data, UMeshComponent* Parent)
{
	USKGAttachmentComponent* AttachmentComponent = NewObject<USKGAttachmentComponent>(Parent->GetOwner());
	AttachmentComponent->SetAttachToMesh(Parent);
	AttachmentComponent->SetAttachToSocketName(Data.SocketToAttachTo);
	AttachmentComponent->ComponentName = Data.AttachmentComponentName;
	AttachmentComponent->SlotTag = Data.SlotTag;
	AttachmentComponent->bUseLegacyAttachmentSystem = false;
	AttachmentComponent->SlotCompatbilityOverride = Data.SlotCompatbilityOverride;
	AttachmentComponent->RegisterComponent();
	if (Data.ActorClass)
	{
		AActor* SpawnedAttachment = Parent->GetWorld()->SpawnActorDeferred<AActor>(Data.ActorClass, FTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (ISKGActorConstructionInterface* ActorInterface = Cast<ISKGActorConstructionInterface>(SpawnedAttachment))
		{
			ActorInterface->SetConstructionDataAsset(Data.ActorClassDefinition);
		}
		UGameplayStatics::FinishSpawningActor(SpawnedAttachment, FTransform());
		AttachmentComponent->TrySetupExistingAttachment(SpawnedAttachment);
		UMeshComponent* FoundMeshComponent = Cast<UMeshComponent>(SpawnedAttachment->FindComponentByTag(UMeshComponent::StaticClass(), MeshComponentTagName));
		ConstructAttachment(Data.Children, FoundMeshComponent);
	}
}

void USKGShooterFrameworkActorConstructionHelpers::ConstructAttachment(const TArray<FInstancedStruct>& Data, UMeshComponent* Parent)
{
	for (const FInstancedStruct& DataInstance : Data)
	{
		const FSKGAttachmentComponentData& AttachmentData = DataInstance.Get<FSKGAttachmentComponentData>();
		ConstructAttachment(AttachmentData, Parent);
	}
}

UMeshComponent* USKGShooterFrameworkActorConstructionHelpers::SetupAttachments(TSoftObjectPtr<UStaticMesh> StaticMesh, TSoftObjectPtr<USkeletalMesh> SkeletalMesh, UStaticMeshComponent* StaticMeshComponent, USkeletalMeshComponent* SkeletalMeshComponent, bool& bIsUsingStaticMesh)
{
	if (!StaticMesh.IsNull())
	{
		SkeletalMeshComponent->DestroyComponent();
		bIsUsingStaticMesh = true;
		return StaticMeshComponent;
	}
	else if (!SkeletalMesh.IsNull())
	{
		StaticMeshComponent->DestroyComponent();
		bIsUsingStaticMesh = false;
		return SkeletalMeshComponent;
	}
	return nullptr;
}

void USKGShooterFrameworkActorConstructionHelpers::SetConstructionDataAsset(AActor* Actor, UDataAsset* DataAsset)
{
	if (ISKGActorConstructionInterface* ActorInterface = Cast<ISKGActorConstructionInterface>(Actor))
	{
		ActorInterface->SetConstructionDataAsset(DataAsset);
	}
}

UPrimaryDataAsset* USKGShooterFrameworkActorConstructionHelpers::GetConstructionDataAsset(AActor* Actor)
{
	if (ISKGActorConstructionInterface* ActorInterface = Cast<ISKGActorConstructionInterface>(Actor))
	{
		return ActorInterface->GetConstructionDataAsset();
	}
	return nullptr;
}
