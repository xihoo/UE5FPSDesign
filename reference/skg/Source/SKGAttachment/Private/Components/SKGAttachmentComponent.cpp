// Copyright 2023, Dakota Dawe, All rights reserved

#include "Components/SKGAttachmentComponent.h"
#include "DataAssets/SKGPDAAttachmentCompatibility.h"
#include "Components/SKGAttachmentManagerComponent.h"
#include "Statics/SKGAttachmentHelpers.h"

#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/World.h"
#include "Interfaces/SKGActorConstructionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

DECLARE_CYCLE_STAT(TEXT("SKGDestroyAttachment"), STAT_SKGDestroyAttachment, STATGROUP_SKGAttachmentComponent);

// Sets default values for this component's properties
USKGAttachmentComponent::USKGAttachmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
	
}

#if WITH_EDITOR
void USKGAttachmentComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (OffsetSettings.OffsetSnapDistance > 0.0f)
	{
		if (OffsetSettings.MinimumOffsetAllowed != 0.0f)
		{
			OffsetSettings.MinimumOffsetAllowed = FMath::RoundToInt(OffsetSettings.MinimumOffsetAllowed / OffsetSettings.OffsetSnapDistance) * OffsetSettings.OffsetSnapDistance;
		}
		if (OffsetSettings.MaximumOffsetAllowed != 0.0f)
		{
			OffsetSettings.MaximumOffsetAllowed = FMath::RoundToInt(OffsetSettings.MaximumOffsetAllowed / OffsetSettings.OffsetSnapDistance) * OffsetSettings.OffsetSnapDistance;
		}
		if (OffsetSettings.DefaultAttachmentOffset != 0.0f)
		{
			OffsetSettings.DefaultAttachmentOffset = FMath::Clamp(FMath::RoundToInt(OffsetSettings.DefaultAttachmentOffset / OffsetSettings.OffsetSnapDistance) * OffsetSettings.OffsetSnapDistance, OffsetSettings.MinimumOffsetAllowed, OffsetSettings.MaximumOffsetAllowed);
		}
	}
}
#endif

void USKGAttachmentComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupComponents();

	RegisterComponentWithManager(this);
	if (HasAuthority())
	{
		if (const USKGAttachmentManagerComponent* AttachmentManagerComponent = CachedAttachmentManager.Get())
		{
			if (AttachmentManagerComponent->CanSpawnDefaultAttachments())
			{
				TSoftClassPtr<AActor> InitialAttachment = DefaultAttachment;
				if (bRandomDefaultAttachment && CompatibleAttachments.Num())
				{
					if (USKGPDAAttachmentCompatibility* PDA  = CompatibleAttachments[FMath::RandRange(0, CompatibleAttachments.Num() - 1)])
					{
						const int32 Index = FMath::RandRange(0, PDA->Attachments.Num() - 1);
						InitialAttachment = PDA->Attachments[Index].Class;
					}
				}
				TryLoadSetupAttachment(InitialAttachment);
			}
		}

		if (AttachmentOffset != OffsetSettings.DefaultAttachmentOffset)
		{
			// Clamp it regardless due to weird editor bug not fully correcting it in PostEditChangeProperties
			OffsetSettings.DefaultAttachmentOffset = FMath::Clamp(OffsetSettings.DefaultAttachmentOffset, OffsetSettings.MinimumOffsetAllowed, OffsetSettings.MaximumOffsetAllowed);
			AttachmentOffset = OffsetSettings.DefaultAttachmentOffset;
			MARK_PROPERTY_DIRTY_FROM_NAME(USKGAttachmentComponent, AttachmentOffset, this);
			TryForceNetUpdate();
			OnRep_AttachmentOffset();
		}
	}
}

void USKGAttachmentComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetupCompatibilityCache();
}

void USKGAttachmentComponent::SetupComponents()
{
	if (HasAuthority())
	{
		for (UActorComponent* Component : GetOwner()->GetComponents())
		{
			if (Component && Component->GetFName().IsEqual(AttachToMeshName))
			{
				AttachToMesh = Cast<UMeshComponent>(Component);
				break;
			}
		}

		//ensureAlwaysMsgf(AttachToMesh, TEXT("Could not find mesh with name: %s on Actor: %s in the SKGAttachmentComponent: %s. Ensure it exists and the name matches what is entered in AttachToMeshName"), *AttachToMeshName.ToString(), *GetOwner()->GetName(), *GetName());
		if (AttachToMesh)
		{
			MARK_PROPERTY_DIRTY_FROM_NAME(USKGAttachmentComponent, AttachToMesh, this);
		}
	}
}

void USKGAttachmentComponent::SetupCompatibilityCache()
{
	CachedCompatibleAttachments.Empty();
	for (const USKGPDAAttachmentCompatibility* AttachmentsData : CompatibleAttachments)
	{
		if (ensureAlwaysMsgf(AttachmentsData, TEXT("Attachments Data is invalid in CompatibleAttachments on Actor: %s in SKGAttachmentComponent: %s (CompatibleAttachments has an element in it that does not contain a data asset"), *GetOwner()->GetName(), *GetName()))
		{
			CachedCompatibleAttachments.Reserve(CachedCompatibleAttachments.Num() + AttachmentsData->Attachments.Num());
			for (const FSKGDAAttachment& AttachmentData : AttachmentsData->Attachments)
			{
				CachedCompatibleAttachments.Add(AttachmentData);
			}
		}
	}
}

void USKGAttachmentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (EndPlayReason == EEndPlayReason::Type::Destroyed)
	{
		DestroyAttachment();
		UnregisterComponentWithManager(this);
	}
}

void USKGAttachmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(USKGAttachmentComponent, Attachment, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGAttachmentComponent, AttachToMesh, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGAttachmentComponent, AttachToSocket, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGAttachmentComponent, ComponentName, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGAttachmentComponent, bUseLegacyAttachmentSystem, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGAttachmentComponent, SlotTag, Params);

	FDoRepLifetimeParams ParamsCustom;
	ParamsCustom.bIsPushBased = true;
	ParamsCustom.Condition = COND_Custom;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGAttachmentComponent, AttachmentOffset, ParamsCustom);
}

void USKGAttachmentComponent::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
	DOREPLIFETIME_ACTIVE_OVERRIDE(USKGAttachmentComponent, AttachmentOffset, OffsetSettings.bUseOffsetSystem);
}

void USKGAttachmentComponent::TryForceNetUpdate() const
{
	if (bAutoCallForceNetUpdate)
	{
		GetOwner()->ForceNetUpdate();
	}
}

FVector USKGAttachmentComponent::GetAttachmentDirectionVector() const
{
	FVector DirectionalVector = FVector::ZeroVector;
	if (AttachToMesh)
	{
		switch (OffsetSettings.OffsetAxis)
		{
		case EAxis::Y : DirectionalVector = AttachToMesh->GetRightVector(); break;
		case EAxis::X : DirectionalVector = AttachToMesh->GetForwardVector(); break;
		case EAxis::Z : DirectionalVector = AttachToMesh->GetUpVector(); break;
		default : DirectionalVector = AttachToMesh->GetForwardVector();
		}
	}
	return DirectionalVector;
}

USKGAttachmentManagerComponent* USKGAttachmentComponent::FindAttachmentManager() const
{
	for (const AActor* CurrentOwner = GetOwner(); CurrentOwner; CurrentOwner = CurrentOwner->GetOwner())
	{
		if (USKGAttachmentManagerComponent* AttachmentManager = USKGAttachmentHelpers::GetAttachmentManagerComponent(CurrentOwner))
		{
			return AttachmentManager;
		}
	}
	return nullptr;
}

bool USKGAttachmentComponent::Server_TrySetupAttachment_Validate(UClass* AttachmentClass)
{
	return bAllowClientSideModification && IsAttachmentClassCompatible(AttachmentClass);
}

void USKGAttachmentComponent::Server_TrySetupAttachment_Implementation(UClass* AttachmentClass)
{
	TrySetupAttachment(AttachmentClass);
}

bool USKGAttachmentComponent::Server_TrySetupAttachmentWithData_Validate(UClass* AttachmentClass, UDataAsset* DataAsset)
{
	return bAllowClientSideModification && IsAttachmentCompatible(DataAsset);
}

void USKGAttachmentComponent::Server_TrySetupAttachmentWithData_Implementation(UClass* AttachmentClass,	UDataAsset* DataAsset)
{
	TrySetupAttachmentWithData(AttachmentClass, DataAsset);
}

bool USKGAttachmentComponent::Server_TrySetupExistingAttachment_Validate(AActor* AttachmentToSetup, const bool bAttach)
{
	return bAllowClientSideModification && IsAttachmentCompatible(AttachmentToSetup);
}

void USKGAttachmentComponent::Server_TrySetupExistingAttachment_Implementation(AActor* AttachmentToSetup, const bool bAttach)
{
	TrySetupExistingAttachment(AttachmentToSetup, bAttach);
}

bool USKGAttachmentComponent::Server_RemoveAttachment_Validate()
{
	return bAllowClientSideModification;
}

void USKGAttachmentComponent::Server_RemoveAttachment_Implementation()
{
	RemoveAttachment();
}

bool USKGAttachmentComponent::Server_DestroyAttachment_Validate()
{
	return bAllowClientSideModification;
}

void USKGAttachmentComponent::Server_DestroyAttachment_Implementation()
{
	DestroyAttachment();
}

USKGAttachmentManagerComponent* USKGAttachmentComponent::GetAttachmentManager()
{
	USKGAttachmentManagerComponent* AttachmentManager = CachedAttachmentManager.Get();
	if (!AttachmentManager)
	{
		AttachmentManager = FindAttachmentManager();
		CachedAttachmentManager = AttachmentManager;
	}
	return AttachmentManager;
}

void USKGAttachmentComponent::RegisterComponentWithManager(USKGAttachmentComponent* Component)
{
	if (Component)
	{
		if (USKGAttachmentManagerComponent* AttachmentManager = GetAttachmentManager())
		{
			AttachmentManager->RegisterAttachmentComponent(Component);
		}
	}
}

void USKGAttachmentComponent::UnregisterComponentWithManager(USKGAttachmentComponent* Component)
{
	if (Component)
	{
		if (USKGAttachmentManagerComponent* AttachmentManager = GetAttachmentManager())
		{
			AttachmentManager->UnregisterAttachmentComponent(Component);
		}
	}
}

void USKGAttachmentComponent::RegisterAttachmentWithManager(AActor* AttachmentToRegister)
{
	if (AttachmentToRegister)
	{
		if (USKGAttachmentManagerComponent* AttachmentManager = GetAttachmentManager())
		{
			AttachmentManager->RegisterAttachmentComponentsAttachment(this, AttachmentToRegister);
		}
	}
}

void USKGAttachmentComponent::UnregisterAttachmentWithManager(AActor* AttachmentToUnregister)
{
	if (AttachmentToUnregister)
	{
		if (USKGAttachmentManagerComponent* AttachmentManager = GetAttachmentManager())
		{
			AttachmentManager->UnregisterAttachmentComponentsAttachment(this, AttachmentToUnregister);
			TArray<AActor*> Attachments = USKGAttachmentHelpers::GetAllAttachedAttachments(AttachmentToUnregister);
			for (AActor* Actor : Attachments)
			{
				AttachmentManager->UnregisterAttachmentComponentsAttachment(this, Actor);
			}
			
			TArray<USKGAttachmentComponent*> AttachedComponents = USKGAttachmentHelpers::GetAllAttachedAttachmentComponents(AttachmentToUnregister);
			for (USKGAttachmentComponent* AttachmentComponent : AttachedComponents)
			{
				AttachmentManager->UnregisterAttachmentComponent(AttachmentComponent);
			}
		}
	}
}

void USKGAttachmentComponent::OnRep_Attachment(AActor* OldAttachment)
{
	// Destroy as a precaution to make it easier
	DestroyPreviewAttachment();
	OnAttachmentChanged.Broadcast();
	if (Attachment)
	{
		OnAttachmentAdded.Broadcast(Attachment);
		SetupAttachmentWithLeaderPoseComponent(Attachment);
		SetRelativeAttachmentOffset(Attachment);
	}
	if (OldAttachment)
	{
		OnAttachmentRemoved.Broadcast(OldAttachment);
	}
}

void USKGAttachmentComponent::TryLoadSetupAttachment(const TSoftClassPtr<AActor>& AttachmentToLoad)
{
	if (!AttachmentToLoad.IsNull())
	{
		UAssetManager::GetStreamableManager().RequestAsyncLoad({AttachmentToLoad.ToSoftObjectPath()}, [this, AttachmentToLoad] ()
		{
			TrySetupAttachment(AttachmentToLoad.Get());
		}, FStreamableManager::DefaultAsyncLoadPriority, false, false);
	}
}

void USKGAttachmentComponent::TryLoadSetupAttachmentWithData(const TSoftClassPtr<AActor>& AttachmentToLoad, UDataAsset* DataAsset)
{
	if (!AttachmentToLoad.IsNull())
	{
		UAssetManager::GetStreamableManager().RequestAsyncLoad({AttachmentToLoad.ToSoftObjectPath()}, [this, AttachmentToLoad, DataAsset] ()
		{
			TrySetupAttachmentWithData(AttachmentToLoad.Get(), DataAsset);
		}, FStreamableManager::DefaultAsyncLoadPriority, false, false);
	}
}

void USKGAttachmentComponent::TrySetupAttachment(UClass* AttachmentClass)
{
	if (HasAuthority())
	{
		if (IsAttachmentClassCompatible(AttachmentClass))
		{
			FActorSpawnParameters Params;
			Params.Owner = GetOwner();
			if (AActor* SpawnedAttachment = GetWorld()->SpawnActor(AttachmentClass, &GetOwner()->GetActorTransform(), Params))
			{
				SetupAttachment(SpawnedAttachment);
			}
		}
	}
	else if (bAllowClientSideModification && IsAttachmentClassCompatible(AttachmentClass))
	{
		Server_TrySetupAttachment(AttachmentClass);
	}
}

void USKGAttachmentComponent::TrySetupAttachmentWithData(UClass* AttachmentClass, UDataAsset* DataAsset)
{
	if (HasAuthority())
	{
		if (IsAttachmentCompatible(DataAsset))
		{
			FActorSpawnParameters Params;
			Params.Owner = GetOwner();
			if (AActor* SpawnedAttachment = GetWorld()->SpawnActor(AttachmentClass, &GetOwner()->GetActorTransform(), Params))
			{
				ISKGActorConstructionInterface* ActorConstructionInterface = Cast<ISKGActorConstructionInterface>(SpawnedAttachment);
				if (ensureAlwaysMsgf(ActorConstructionInterface, TEXT("Attachment: %s does NOT implement the SKGActorConstructionInterface to be constructed from data"), *SpawnedAttachment->GetName()))
				{
					ActorConstructionInterface->SetConstructionDataAsset(DataAsset);
					SetupAttachment(SpawnedAttachment);
				}
				else
				{
					SpawnedAttachment->Destroy();
				}
			}
		}
	}
	else if (bAllowClientSideModification && IsAttachmentCompatible(DataAsset))
	{
		Server_TrySetupAttachmentWithData(AttachmentClass, DataAsset);
	}
}

void USKGAttachmentComponent::TrySetupExistingAttachment(AActor* AttachmentToSetup, const bool bAttach)
{
	if (AttachmentToSetup)
	{
		if (HasAuthority())
		{
			if (IsAttachmentCompatible(AttachmentToSetup))
			{
				SetupAttachment(AttachmentToSetup, bAttach);
				
				TArray<USKGAttachmentComponent*> AttachmentComponents = USKGAttachmentHelpers::GetAllAttachedAttachmentComponents(AttachmentToSetup);
				for (USKGAttachmentComponent* Component : AttachmentComponents)
				{
					RegisterComponentWithManager(Component);
				}

				TArray<AActor*> AttachedActors = USKGAttachmentHelpers::GetAllAttachedAttachments(AttachmentToSetup);
				for (AActor* Attached : AttachedActors)
				{
					RegisterAttachmentWithManager(Attached);
				}
			}
		}
		else if (bAllowClientSideModification && IsAttachmentCompatible(AttachmentToSetup))
		{
			Server_TrySetupExistingAttachment(AttachmentToSetup, bAttach);
		}
	}
}

AActor* USKGAttachmentComponent::RemoveAttachment()
{
	if (Attachment)
	{
		if (HasAuthority())
		{
			AActor* CurrentAttachment = Attachment;
			UnregisterAttachmentWithManager(Attachment);
			
			Attachment = nullptr;
			MARK_PROPERTY_DIRTY_FROM_NAME(USKGAttachmentComponent, Attachment, this);
			OnRep_Attachment(CurrentAttachment);
			TryForceNetUpdate();

			return CurrentAttachment;
		}
		else if (bAllowClientSideModification)
		{
			Server_RemoveAttachment();
		}
	}
	return nullptr;
}

void USKGAttachmentComponent::DestroyAttachment()
{
	if (Attachment)
	{
		if (HasAuthority())
		{
			SCOPE_CYCLE_COUNTER(STAT_SKGDestroyAttachment);
			UnregisterAttachmentWithManager(Attachment);
			
			TArray<AActor*> AttachedActors = USKGAttachmentHelpers::GetAllAttachedAttachments(Attachment);
			for (AActor* Actor : AttachedActors)
			{
				if (Actor)
				{
					Actor->Destroy();
				}
			}
		
			OnAttachmentDestroyed.Broadcast(Attachment);
			Attachment->Destroy();
			Attachment = nullptr;
			MARK_PROPERTY_DIRTY_FROM_NAME(USKGAttachmentComponent, Attachment, this);
			OnRep_Attachment(nullptr);
			TryForceNetUpdate();
		}
		else if (bAllowClientSideModification)
		{
			Server_DestroyAttachment();
		}
	}
}

const TArray<FSKGDAAttachment>& USKGAttachmentComponent::GetCompatibleAttachments() const
{
	return CachedCompatibleAttachments;
}

TArray<UPrimaryDataAsset*> USKGAttachmentComponent::GetCompatibleAttachmentsFromPrimaryAssets() const
{
	TArray<UPrimaryDataAsset*> DataAssets;
	const TArray<FPrimaryAssetType>& PrimaryAssetTypes = USKGAttachmentDeveloperSettings::GetPrimarAssetTypes();
	for (const FPrimaryAssetType& PrimaryAssetType : PrimaryAssetTypes)
	{
		TArray<FPrimaryAssetId> FoundAssetIds;
		UAssetManager::Get().GetPrimaryAssetIdList(PrimaryAssetType, FoundAssetIds);
		for (const FPrimaryAssetId& AssetId : FoundAssetIds)
		{
			if (UPrimaryDataAsset* DataAsset = Cast<UPrimaryDataAsset>(UAssetManager::Get().GetPrimaryAssetObject(AssetId)))
			{
				if (IsAttachmentCompatibleWithSlotSystem(DataAsset))
				{
					DataAssets.Emplace(DataAsset);
				}
			}
		}
	}
	return DataAssets;
}

bool USKGAttachmentComponent::IsAttachmentCompatibleWithSlotSystem(const UObject* Object) const
{
	if (const IGameplayTagAssetInterface* AttachmentGameplayTagAssetInterface = Cast<IGameplayTagAssetInterface>(Object))
	{
		FGameplayTagContainer AttachmentTags;
		AttachmentGameplayTagAssetInterface->GetOwnedGameplayTags(AttachmentTags);
		
		if (SlotTag.IsValid())
		{
			const FSKGAttachmentSlotCompatbility& SlotCompatibility = USKGAttachmentDeveloperSettings::GetSlotCompatibility(SlotTag);
			return SlotCompatibility.Query.Matches(AttachmentTags);
		}
		if (SlotCompatbilityOverride.SlotTag.IsValid())
		{
			return SlotCompatbilityOverride.Query.Matches(AttachmentTags);
		}
		return true;
	}
	//ensureAlwaysMsgf(false, TEXT("Attachment: %s is trying to be used on Component: %s/%s with the gameplay tag compatibility system and does NOT implement the gameplay tag asset interface"), *Object->GetName(), *GetName(), *GetComponentName().ToString());
	return false;
}

bool USKGAttachmentComponent::IsAttachmentClassCompatible(const UClass* AttachmentClass)
{
	if (bUseLegacyAttachmentSystem)
	{
		if (AttachmentClass && CompatibleAttachments.Num())
		{
			for (const FSKGDAAttachment& AttachmentData : CachedCompatibleAttachments)
			{
				if (AttachmentData.Class == AttachmentClass)
				{
					return true;
				}
			}
			return false;
		}
		// If no compatible attachments specified, allow all
		return AttachmentClass ? true : false;
	}

	// If not using legacy system, check with slot system
	return IsAttachmentCompatibleWithSlotSystem(AttachmentClass->IsChildOf(UDataAsset::StaticClass()) ? AttachmentClass : NewObject<UObject>(this, AttachmentClass));
}

bool USKGAttachmentComponent::IsAttachmentCompatible(const UObject* Object)
{	// If actor is valid, return if its class is allowed, otherwise false
	if (bUseLegacyAttachmentSystem)
	{
		return Object ? IsAttachmentClassCompatible(Object->GetClass()) : false;
	}
	
	// If not using legacy system, check with slot system
	return IsAttachmentCompatibleWithSlotSystem(Object);
}

void USKGAttachmentComponent::TryLoadSetupPreviewAttachment(const TSoftClassPtr<AActor>& AttachmentToLoad)
{
	if (!AttachmentToLoad.IsNull())
	{
		UAssetManager::GetStreamableManager().RequestAsyncLoad({AttachmentToLoad.ToSoftObjectPath()}, [this, AttachmentToLoad] ()
			{
				SetupPreviewAttachment(AttachmentToLoad.Get());
			}, FStreamableManager::DefaultAsyncLoadPriority, false, false);
	}
}

void USKGAttachmentComponent::TryLoadSetupPreviewAttachmentWithData(const TSoftClassPtr<AActor>& AttachmentToLoad, UDataAsset* DataAsset)
{
	if (!AttachmentToLoad.IsNull())
	{
		UAssetManager::GetStreamableManager().RequestAsyncLoad({AttachmentToLoad.ToSoftObjectPath()}, [this, AttachmentToLoad, DataAsset] ()
			{
				SetupPreviewAttachmentWithData(AttachmentToLoad.Get(), DataAsset);
			}, FStreamableManager::DefaultAsyncLoadPriority, false, false);
	}
}

void USKGAttachmentComponent::SetupPreviewAttachment(UClass* AttachmentClass)
{
	if (IsAttachmentClassCompatible(AttachmentClass))
	{
		DestroyPreviewAttachment();
		
		PreviewAttachment = GetWorld()->SpawnActorDeferred<AActor>(AttachmentClass, FTransform());
		if (PreviewAttachment)
		{
			PreviewAttachment->SetActorEnableCollision(false);
			PreviewAttachment->SetReplicates(false);
			UGameplayStatics::FinishSpawningActor(PreviewAttachment, FTransform());
			AttachAttachment(PreviewAttachment);
			SetupAttachmentWithLeaderPoseComponent(PreviewAttachment);
			SetRelativeAttachmentOffset(PreviewAttachment);

			if (Attachment && !Attachment->IsHidden())
			{
				TArray<AActor*> AttachedActors;
				Attachment->GetAttachedActors(AttachedActors, true);
				// Hide anything attached
				for (AActor* Actor : AttachedActors)
				{
					Actor->SetActorHiddenInGame(true);
				}
				Attachment->SetActorHiddenInGame(true);
			}

			OnPreviewAttachmentAdded.Broadcast(PreviewAttachment);
		}
	}
}

void USKGAttachmentComponent::SetupPreviewAttachmentWithData(UClass* AttachmentClass, UDataAsset* DataAsset)
{
	if (IsAttachmentCompatible(DataAsset))
	{
		DestroyPreviewAttachment();
		
		PreviewAttachment = GetWorld()->SpawnActorDeferred<AActor>(AttachmentClass, FTransform());
		ISKGActorConstructionInterface* ActorConstructionInterface = Cast<ISKGActorConstructionInterface>(PreviewAttachment);
		if (ensureAlwaysMsgf(ActorConstructionInterface, TEXT("Attachment: %s does NOT implement the SKGActorConstructionInterface to be constructed from data"), *PreviewAttachment->GetName()))
		{
			PreviewAttachment->SetActorEnableCollision(false);
			PreviewAttachment->SetReplicates(false);
			UGameplayStatics::FinishSpawningActor(PreviewAttachment, FTransform());
			ActorConstructionInterface->SetConstructionDataAsset(DataAsset);
			AttachAttachment(PreviewAttachment);
			SetupAttachmentWithLeaderPoseComponent(PreviewAttachment);
			SetRelativeAttachmentOffset(PreviewAttachment);
			OnPreviewAttachmentAdded.Broadcast(PreviewAttachment);

			if (Attachment && !Attachment->IsHidden())
			{
				TArray<AActor*> AttachedActors;
				Attachment->GetAttachedActors(AttachedActors, true);
				// Hide anything attached
				for (AActor* Actor : AttachedActors)
				{
					Actor->SetActorHiddenInGame(true);
				}
				Attachment->SetActorHiddenInGame(true);
			}
		}
		else
		{
			DestroyPreviewAttachment();
		}
	}
}

void USKGAttachmentComponent::DestroyPreviewAttachment()
{
	if (PreviewAttachment)
	{
		OnPreviewAttachmentRemoved.Broadcast(PreviewAttachment);
		TArray<AActor*> AttachedActors;
		PreviewAttachment->GetAttachedActors(AttachedActors, true);
		// Destroy anything attached just incase
		for (AActor* Actor : AttachedActors)
		{
			Actor->Destroy();
		}
		PreviewAttachment->Destroy();
		PreviewAttachment = nullptr;
	}
	if (Attachment && Attachment->IsHidden())
	{
		TArray<AActor*> AttachedActors;
		Attachment->GetAttachedActors(AttachedActors, true);
		// Hide anything attached
		for (AActor* Actor : AttachedActors)
		{
			Actor->SetActorHiddenInGame(false);
		}
		Attachment->SetActorHiddenInGame(false);
	}
}

FTransform USKGAttachmentComponent::GetAttachOffsetTransform() const
{
	FTransform StartTransform = GetAttachTransform();
	if (OffsetSettings.MinimumOffsetAllowed < 0.0f)
	{
		const FVector StartLocation = StartTransform.GetLocation() + GetAttachmentDirectionVector() * AttachmentOffset;
		StartTransform.SetLocation(StartLocation);
	}
	return StartTransform;
}

FTransform USKGAttachmentComponent::GetAttachTransform() const
{
	return AttachToMesh ? AttachToMesh->GetSocketTransform(AttachToSocket) : FTransform();
}

FTransform USKGAttachmentComponent::GetAttachStartTransform() const
{
	FTransform StartTransform = GetAttachTransform();
	if (OffsetSettings.MinimumOffsetAllowed < 0.0f)
	{
		const FVector StartLocation = StartTransform.GetLocation() + GetAttachmentDirectionVector() * OffsetSettings.MinimumOffsetAllowed;
		StartTransform.SetLocation(StartLocation);
	}
	return StartTransform;
}

FTransform USKGAttachmentComponent::GetAttachEndTransform() const
{
	FTransform StartTransform = GetAttachTransform();
	if (OffsetSettings.MaximumOffsetAllowed > 0.0f)
	{
		const FVector StartLocation = StartTransform.GetLocation() + GetAttachmentDirectionVector() * OffsetSettings.MaximumOffsetAllowed;
		StartTransform.SetLocation(StartLocation);
	}
	return StartTransform;
}

TArray<FVector> USKGAttachmentComponent::GetAttachSnapPoints() const
{
	TArray<FVector> WorldSnapPoints;
	const int32 SnapPointCount = OffsetSettings.GetSnapPointCount();
	WorldSnapPoints.Reserve(SnapPointCount + 2);// +2 to account for start/end point

	const FVector Direction = GetAttachmentDirectionVector();
	FVector WorldSnapPoint = AttachToMesh->GetSocketLocation(AttachToSocket) + Direction * OffsetSettings.MinimumOffsetAllowed;
	for (int i = 0; i < SnapPointCount + 1; ++i)
	{
		WorldSnapPoints.Add(WorldSnapPoint);
		WorldSnapPoint += Direction * OffsetSettings.OffsetSnapDistance;
	}
	return WorldSnapPoints;
}

void USKGAttachmentComponent::SetAttachToMesh(UMeshComponent* MeshComponent)
{
	AttachToMesh = MeshComponent;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGAttachmentComponent, AttachToMesh, this);
}

void USKGAttachmentComponent::SetupAttachment(AActor* AttachmentToSetup, const bool bAttach)
{
	if (AttachmentToSetup)
	{
		AActor* OldAttachment = Attachment;
		Attachment = AttachmentToSetup;
		Attachment->SetOwner(GetOwner());
		if (bAttach)
		{
			AttachAttachment(Attachment);
		}
		RegisterAttachmentWithManager(Attachment);
		DestroyPreviewAttachment();
		MARK_PROPERTY_DIRTY_FROM_NAME(USKGAttachmentComponent, Attachment, this);
		OnRep_Attachment(OldAttachment);
		TryForceNetUpdate();
	}
}

void USKGAttachmentComponent::AttachAttachment(AActor* AttachmentToSetup) const
{
	const FAttachmentTransformRules TransformRules(AttachmentRules.AttachLocationRule, AttachmentRules.AttachRotationRule, AttachmentRules.AttachScaleRule, AttachmentRules.bWeldSimulatedBodies);
	AttachmentToSetup->AttachToComponent(AttachToMesh, TransformRules, AttachToSocket);
}

void USKGAttachmentComponent::SetupAttachmentWithLeaderPoseComponent(AActor* AttachmentToSetup) const
{
	if (bAutoSetLeaderPoseComponent)
	{
		if (USkeletalMeshComponent* AttachToSkeletalMesh = Cast<USkeletalMeshComponent>(AttachToMesh))
		{
			TArray<USkeletalMeshComponent*> Components;
			AttachmentToSetup->GetComponents<USkeletalMeshComponent>(Components);
			for (USkeletalMeshComponent* MeshComponent : Components)
			{
				if (MeshComponent && MeshComponent->GetFName().IsEqual(LeaderPoseAttachmentMeshName))
				{
					MeshComponent->SetLeaderPoseComponent(AttachToSkeletalMesh);
				}
			}
		}
	}
}

void USKGAttachmentComponent::OnRep_AttachmentOffset()
{
	SetRelativeAttachmentOffset(Attachment);
	OnOffsetChanged.Broadcast(AttachmentOffset);
}

void USKGAttachmentComponent::SetRelativeAttachmentOffset(AActor* AttachmentToSet)
{
	if (OffsetSettings.bUseOffsetSystem && AttachmentToSet)
	{
		FVector RelativeOffset = FVector(0.0f, AttachmentOffset, 0.0f);
		switch (OffsetSettings.OffsetAxis)
		{
		case EAxis::X : RelativeOffset = FVector(AttachmentOffset, 0.0f, 0.0f); break;
		case EAxis::Z : RelativeOffset = FVector(0.0f, 0.0f, AttachmentOffset); break;
		}
		AttachmentToSet->SetActorRelativeLocation(RelativeOffset);
	}
}

bool USKGAttachmentComponent::Server_SetAttachmentOffset_Validate(const float Offset)
{
	return bAllowClientSideModification;
}

void USKGAttachmentComponent::Server_SetAttachmentOffset_Implementation(const float Offset)
{
	if (Offset != AttachmentOffset)
	{
		AttachmentOffset = Offset;
		MARK_PROPERTY_DIRTY_FROM_NAME(USKGAttachmentComponent, AttachmentOffset, this);
		TryForceNetUpdate();
		OnRep_AttachmentOffset();
	}
}

void USKGAttachmentComponent::FinalizeAttachmentOffset()
{
	if (OldAttachmentOffset != AttachmentOffset)
	{
		OldAttachmentOffset = AttachmentOffset;
		if (HasAuthority())
		{
			Server_SetAttachmentOffset_Implementation(AttachmentOffset);
		}
		else if (bAllowClientSideModification)
		{
			Server_SetAttachmentOffset(AttachmentOffset);
			OnRep_AttachmentOffset();
		}
	}
}

void USKGAttachmentComponent::SetAttachmentOffset(float Offset)
{
	if (OffsetSettings.OffsetSnapDistance > 0.0f)
	{
		Offset = FMath::RoundToInt(Offset / OffsetSettings.OffsetSnapDistance) * OffsetSettings.OffsetSnapDistance;
	}
	Offset = FMath::Clamp(Offset, OffsetSettings.MinimumOffsetAllowed, OffsetSettings.MaximumOffsetAllowed);
	if (Offset != AttachmentOffset)
	{
		AttachmentOffset = Offset;
		if (OffsetSettings.bReplicateOffsetEachChange)
		{
			FinalizeAttachmentOffset();
		}
		OnRep_AttachmentOffset();
	}
}

void USKGAttachmentComponent::IncrementOffset()
{
	SetAttachmentOffset(AttachmentOffset + OffsetSettings.OffsetSnapDistance);
}

void USKGAttachmentComponent::DecrementOffset()
{
	SetAttachmentOffset(AttachmentOffset - OffsetSettings.OffsetSnapDistance);
}
