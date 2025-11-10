// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Actor.h"
#include "DataTypes/SKGAttachmentDataTypes.h"
#include "SKGAttachmentManagerComponent.generated.h"

class USKGAttachmentComponent;
class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttachmentComponentsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttachmentsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttachmentComponentAdded, USKGAttachmentComponent*, AttachmentComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttachmentComponentRemoved, USKGAttachmentComponent*, AttachmentComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttachmentComponentAttachmentAdded, AActor*, Attachment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttachmentComponentAttachmentRemoved, AActor*, Attachment);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKGATTACHMENT_API USKGAttachmentManagerComponent : public UActorComponent, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USKGAttachmentManagerComponent();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }
	static TArray<USKGAttachmentComponent*> GetAllActorAttachmentComponentsWithAttachments(const AActor* Actor);
	static TArray<USKGAttachmentComponent*> GetActorAttachmentComponentsWithAttachments(const AActor* Actor);

	FORCEINLINE bool HasAuthority() const { return GetOwnerRole() == ROLE_Authority; }
	void RegisterAttachmentComponent(USKGAttachmentComponent* AttachmentComponent);
	void UnregisterAttachmentComponent(USKGAttachmentComponent* AttachmentComponent);
	void RegisterAttachmentComponentsAttachment(USKGAttachmentComponent* AttachmentComponent, AActor* Attachment);
	void UnregisterAttachmentComponentsAttachment(USKGAttachmentComponent* AttachmentComponent, AActor* Attachment);
	void SetCanSpawnDefaultAttachments(bool bCanSpawn) { bCanSpawnDefaultAttachments = bCanSpawn; }
	bool CanSpawnDefaultAttachments() const { return bCanSpawnDefaultAttachments; }

	UFUNCTION(BlueprintPure, Category = "SKGAttachmentManagerComponent|Attachments")
	const TArray<FSKGAttachmentComponentItem>& GetAttachmentComponents() const { return AttachmentComponents.Items; }
	UFUNCTION(BlueprintPure, Category = "SKGAttachmentManagerComponent|Attachments")
	const TArray<AActor*>& GetAttachments() const { return Attachments; }
	const TArray<AActor*>& GetAttachments() { return Attachments; }
	// Will return an array of attachment components that are compatible with the Object
	UFUNCTION(BlueprintPure, Category = "SKGAttachmentManagerComponent|Attachments")
	bool GetCompatibleAttachmentComponentsFromObject(TArray<USKGAttachmentComponent*>& Components, const UObject* Object) const;
	// Will return an array of attachment components that are compatible with the Class
	UFUNCTION(BlueprintPure, Category = "SKGAttachmentManagerComponent|Attachments")
	bool GetCompatibleAttachmentComponentsFromClass(TArray<USKGAttachmentComponent*>& Components, const UClass* Class) const;
	// Will return an array of attachment components that are compatible with the construction data asset
	UFUNCTION(BlueprintPure, Category = "SKGAttachmentManagerComponent|Attachments")
	bool GetCompatibleAttachmentComponentsFromConstructionDataAsset(TArray<USKGAttachmentComponent*>& Components, const UDataAsset* DataAsset) const;
	// Will return the attachment component that holds the attachment
	UFUNCTION(BlueprintPure, Category = "SKGAttachmentManagerComponent|Attachments")
	USKGAttachmentComponent* GetAttachmentComponentWithAttachment(AActor* Attachment);
	// Will return the attachment component that contains the given tag (from the gameplay tag interface)
	UFUNCTION(BlueprintPure, Category = "SKGAttachmentManagerComponent|Attachments")
	USKGAttachmentComponent* GetAttachmentComponentWithTag(const FGameplayTag& Tag);
	// Will return the attachment component that matches the given query (from the gameplay tag interface)
	UFUNCTION(BlueprintPure, Category = "SKGAttachmentManagerComponent|Attachments")
	USKGAttachmentComponent* GetAttachmentComponentWithQuery(const FGameplayTagQuery& Query);
	// Will return the attachment components that matches the given query (from the gameplay tag interface)
	UFUNCTION(BlueprintPure, Category = "SKGAttachmentManagerComponent|Attachments")
	TArray<USKGAttachmentComponent*> GetAttachmentComponentsWithQuery(const FGameplayTagQuery& Query);
	// Will return the attachment component that contains the given slot tag (used in the new attachment compatibility system)
	UFUNCTION(BlueprintPure, Category = "SKGAttachmentManagerComponent|Attachments")
	USKGAttachmentComponent* GetAttachmentComponentWithSlotTag(const FGameplayTag& SlotTag);
	// Will return the attachment that contains the given tag
	UFUNCTION(BlueprintPure, Category = "SKGAttachmentManagerComponent|Attachments")
	AActor* GetAttachmentWithTag(const FGameplayTag& Tag);
	
	UFUNCTION(BlueprintCallable, Category = "SKGAttachmentManagerComponent|AttachmentState")
	void HideAllAttachments();
	UFUNCTION(BlueprintCallable, Category = "SKGAttachmentManagerComponent|AttachmentState")
	void UnhideAllAttachments();
	UFUNCTION(BlueprintCallable, Category = "SKGAttachmentManagerComponent|AttachmentState")
	void DisableCollisionAllAttachments();
	UFUNCTION(BlueprintCallable, Category = "SKGAttachmentManagerComponent|AttachmentState")
	void EnableCollisionAllAttachments();
	UFUNCTION(BlueprintCallable, Category = "SKGAttachmentManagerComponent|AttachmentState")
	void DestroyAllAttachments(const bool bIncludeSelf = true);

	UFUNCTION(BlueprintGetter)
	FName GetManagerName() const { return ManagerName; }

	// Called when the AttachmentComponents array changes via an add or removed both from client and server
	UPROPERTY(BlueprintAssignable, Category = "SKGAttachmentManagerComponent|Events")
	FOnAttachmentComponentsChanged OnAttachmentComponentsChanged;
	// Called when the Attachments array changes via an add or removed both from client and server
	UPROPERTY(BlueprintAssignable, Category = "SKGAttachmentManagerComponent|Events")
	FOnAttachmentsChanged OnAttachmentsChanged;
	/*
	 * Called on the Client and Server when an attachment component is registered with the manager.
	 * This happens locally, The AttachmentComponents array is out of date
	 * when this event is called on the client (awaiting replication), but up to date on the server.
	 * Consider Binding to OnAttachmentComponentsChanged as this fires once replication is received on
	 * the client.
	 */
	UPROPERTY(BlueprintAssignable, Category = "SKGAttachmentManagerComponent|Events")
	FOnAttachmentComponentAdded OnAttachmentComponentAdded;
	/*
	 * Called on the Client and Server when an attachment component is unregistered with the manager.
	 * This happens locally, The AttachmentComponents array is out of date
	* when this event is called on the client (awaiting replication), but up to date on the server.
	 * Consider Binding to OnAttachmentComponentsChanged as this fires once replication is received on
	 * the client.
	 */
	UPROPERTY(BlueprintAssignable, Category = "SKGAttachmentManagerComponent|Events")
	FOnAttachmentComponentRemoved OnAttachmentComponentRemoved;
	// Called on the server only when an attachment component is added
	UPROPERTY(BlueprintAssignable, Category = "SKGAttachmentManagerComponent|Events")
	FOnAttachmentComponentAttachmentAdded OnAttachmentComponentAttachmentAdded;
	// Called on the server only when an attachment component is added
	UPROPERTY(BlueprintAssignable, Category = "SKGAttachmentManagerComponent|Events")
	FOnAttachmentComponentAttachmentRemoved OnAttachmentComponentAttachmentRemoved;

	// Can be used for whatever. In the example it is used for the save directory of saving/loading presets
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetManagerName, Category = "SKGAttachmentManager|Initialize")
	FName ManagerName {""};
	// If true, clients can trigger a replicated hide/unhide and collision/nocollision to all attachments
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachmentManager|Initialize")
	bool bAllowClientSideModification {true};
	// Useful if your NetUpdateFrequency is set super low
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachmentManager|Initialize")
	bool bAutoCallForceNetUpdate {false};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGAttachmentManager|Initialize")
	FGameplayTagContainer GameplayTags;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_AttachmentComponents)
	FSKGAttachmentComponentItems AttachmentComponents;
	UFUNCTION()
	void OnRep_AttachmentComponents();

	// Array of all attachments added to the parent
	UPROPERTY(ReplicatedUsing = OnRep_Attachments)
	TArray<TObjectPtr<AActor>> Attachments;
	UFUNCTION()
	void OnRep_Attachments(TArray<AActor*> OldAttachments);

	UPROPERTY(ReplicatedUsing = OnRep_AttachmentsStates)
	bool bIsHidden {false};
	UPROPERTY(ReplicatedUsing = OnRep_AttachmentsStates)
	bool bHasCollision {true};
	UFUNCTION()
	void OnRep_AttachmentsStates();
	void SetAttachmentsToState();

	bool bCanSpawnDefaultAttachments {true};
	
	FORCEINLINE void TryForceNetUpdate() const;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_HideAllAttachments(bool Hide);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetAllAttachmentsCollision(bool EnableCollision);
};
