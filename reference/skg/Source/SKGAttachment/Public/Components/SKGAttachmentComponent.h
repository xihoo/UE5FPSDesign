// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "DeveloperSettings/SKGAttachmentDeveloperSettings.h"
#include "Components/ActorComponent.h"
#include "SKGAttachmentComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("SKGAttachmentComponent"), STATGROUP_SKGAttachmentComponent, STATCAT_Advanced);

struct FSKGDAAttachment;
class USKGAttachmentManagerComponent;
class UMeshComponent;
class USKGPDAAttachmentCompatibility;
class UTexture2D;

USTRUCT(BlueprintType)
struct FSKGAttachmentOffsetSettings
{
	GENERATED_BODY()
	// If false, you cannot move attachments using this system (assumes feature is not needed or using your own)
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment")
	bool bUseOffsetSystem {true};
	// If false, FinalizeAttachmentOffset will need to be called in order to replicate from the client
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment")
	bool bReplicateOffsetEachChange {false};
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment")
	TEnumAsByte<EAxis::Type> OffsetAxis {EAxis::Y};
	// 1.046 = picatinny, 4.0 = M-Lok, 2.0 = Keymod
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment")
	float OffsetSnapDistance {1.046f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment")
	float MinimumOffsetAllowed {0.0f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment")
	float MaximumOffsetAllowed {0.0f};
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment")
	float DefaultAttachmentOffset {0.0f};

	int32 GetSnapPointCount() const
	{
		return (MaximumOffsetAllowed + -MinimumOffsetAllowed) / OffsetSnapDistance;
	}
};

USTRUCT(BlueprintType)
struct FSKGAttachmentAttachRules
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment")
	EAttachmentRule AttachLocationRule {EAttachmentRule::SnapToTarget};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment")
	EAttachmentRule AttachRotationRule {EAttachmentRule::SnapToTarget};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment")
	EAttachmentRule AttachScaleRule {EAttachmentRule::SnapToTarget};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment")
	bool bWeldSimulatedBodies {true};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttachmentChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttachmentAdded, AActor*, NewAttachment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttachmentRemoved, AActor*, OldAttachment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttachmentDestroyed, AActor*, OldAttachment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOffsetChanged, float, Offset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPreviewAttachmentAdded, AActor*, PreviewAttachment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPreviewAttachmentRemoved, AActor*, PreviewAttachment);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKGATTACHMENT_API USKGAttachmentComponent : public UActorComponent, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	bool bConstructDefaults {true};
	// Sets default values for this component's properties
	USKGAttachmentComponent();
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }

	FORCEINLINE bool HasAuthority() const { return GetOwnerRole() == ROLE_Authority; }
	// Should only be used when manually setting the value for construction
	void SetAttachToMeshName(const FName& Name) { AttachToMeshName = Name; }
	// Should only be used when manually setting the value for construction
	void SetAttachToSocketName(const FName& Name) { AttachToSocket = Name; }
	
	// If AttachmentToLoad is not loaded it will load and be constructed as an attachment. RemoveAttachment should be called first if theres an existing attachment
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Attachment")
	void TryLoadSetupAttachment(const TSoftClassPtr<AActor>& AttachmentToLoad);
	// If AttachmentToLoad is not loaded it will load and be constructed as an attachment. RemoveAttachment should be called first if theres an existing attachment
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Attachment")
	void TryLoadSetupAttachmentWithData(const TSoftClassPtr<AActor>& AttachmentToLoad, UDataAsset* DataAsset);
	// The provided class will be spawned and constructed as an attachment. RemoveAttachment should be called first if theres an existing attachment
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Attachment")
	void TrySetupAttachment(UClass* AttachmentClass);
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Attachment")
	void TrySetupAttachmentWithData(UClass* AttachmentClass, UDataAsset* DataAsset);
	// The provided actor will be setup as the new attachment. RemoveAttachment should be called first if theres an existing attachment
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Attachment")
	void TrySetupExistingAttachment(AActor* AttachmentToSetup, const bool bAttach = true);
	/**
	 * This function is intended to be used if you already have an attachment system (such as an inventory) that attaches the actor for you.
	 * The provided actor will be setup as the new attachment.
	 * RemoveAttachment should be called first if theres an existing attachment.
	 */
	// Will remove the current attachment from this component. Will not destroy
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Attachment")
	AActor* RemoveAttachment();
	// Will remove the current attachment from this component and destroy it
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Attachment")
	void DestroyAttachment();
	UFUNCTION(BlueprintPure, Category = "SKGAttachment|Attachment")
	bool HasAttachment() const { return Attachment != nullptr; }

	UFUNCTION(BlueprintPure, Category = "SKGAttachment|Compatibility")
	bool GetUsesLegacyAttachmentCompatibilitySystem() const { return bUseLegacyAttachmentSystem; }
	UFUNCTION(BlueprintPure, Category = "SKGAttachment|Compatibility")
	const TArray<FSKGDAAttachment>& GetCompatibleAttachments() const;
	UFUNCTION(BlueprintPure, Category = "SKGAttachment|Compatibility")
	TArray<UPrimaryDataAsset*> GetCompatibleAttachmentsFromPrimaryAssets() const;
	UFUNCTION(BlueprintPure, Category = "SKGAttachment|Compatibility")
	virtual bool IsAttachmentClassCompatible(const UClass* AttachmentClass);
	UFUNCTION(BlueprintPure, Category = "SKGAttachment|Compatibility")
	virtual bool IsAttachmentCompatible(const UObject* Object);
	
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Preview")
	void TryLoadSetupPreviewAttachment(const TSoftClassPtr<AActor>& AttachmentToLoad);
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Preview")
	void TryLoadSetupPreviewAttachmentWithData(const TSoftClassPtr<AActor>& AttachmentToLoad, UDataAsset* DataAsset);
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Preview")
	void SetupPreviewAttachment(UClass* AttachmentClass);
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Preview")
	void SetupPreviewAttachmentWithData(UClass* AttachmentClass, UDataAsset* DataAsset);
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Preview")
	void DestroyPreviewAttachment();

	UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Offset")
	void FinalizeAttachmentOffset();
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Offset")
	void SetAttachmentOffset(float Offset);
	// This will move the attachment forward by 1 OffsetIncrementalAmount
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Offset")
	void IncrementOffset();
	// This will move the attachment backwards by 1 OffsetIncrementalAmount
	UFUNCTION(BlueprintCallable, Category = "SKGAttachment|Offset")
	void DecrementOffset();
	UFUNCTION(BlueprintPure, Category = "SKGAttachment|Offset")
	FORCEINLINE bool CanAttachmentBeOffset() const { return OffsetSettings.MaximumOffsetAllowed != OffsetSettings.MinimumOffsetAllowed; }
	// This will return the position of the current attachment offset
	UFUNCTION(BlueprintPure, Category = "SKGAttachment|Offset")
	FTransform GetAttachOffsetTransform() const;
	// This will return the AttachToSocket transform
	UFUNCTION(BlueprintPure, Category = "SKGAttachment|Offset")
	FTransform GetAttachTransform() const;
	// This will return the Minimum Offset transform
	UFUNCTION(BlueprintPure, Category = "SKGAttachment|Offset")
	FTransform GetAttachStartTransform() const;
	// This will return the Maximum Offset transform
	UFUNCTION(BlueprintPure, Category = "SKGAttachment|Offset")
	FTransform GetAttachEndTransform() const;
	/**
	 * This will return the snap points between the Minimum and Maximum offset at the defined OffsetSnapDistance.
	 * If the OffsetSnapDistance is 0, it will return the Minimum and Maximum only.
	 */
	UFUNCTION(BlueprintPure, Category = "SKGAttachment|Attachment|Offset")
	TArray<FVector> GetAttachSnapPoints() const;

	UFUNCTION(BlueprintGetter)
	FORCEINLINE FName GetAttachToSocket() const { return AttachToSocket; }
	UFUNCTION(BlueprintGetter)
	FORCEINLINE FName GetComponentName() const { return ComponentName; }
	UFUNCTION(BlueprintGetter)
	UTexture2D* GetComponentImage() const { return ComponentImage; }
	UFUNCTION(BlueprintGetter)
	FORCEINLINE TSoftClassPtr<AActor> GetDefaultAttachment() const { return DefaultAttachment; }
	UMeshComponent* GetAttachToMesh() const { return AttachToMesh; }
	void SetAttachToMesh(UMeshComponent* MeshComponent);
	template< typename T >
	FORCEINLINE T* GetAttachToMesh() const { return Cast<T>(AttachToMesh); }
	UFUNCTION(BlueprintGetter)
	FORCEINLINE AActor* GetAttachment() const { return Attachment; }
	template< typename T >
	FORCEINLINE T* GetAttachment() const { return Cast<T>(GetAttachment()); }
	UFUNCTION(BlueprintGetter)
	FORCEINLINE float GetAttachmentOffset() const { return AttachmentOffset; }
	UFUNCTION(BlueprintGetter)
	FORCEINLINE bool GetIsRequiredToFunction() const { return bIsRequiredToFunction; }
	UFUNCTION(BlueprintGetter)
	const FSKGAttachmentOffsetSettings& GetOffsetSettings() const { return OffsetSettings; }

	// Fires on client and server on replication when the attachment is added
	UPROPERTY(BlueprintAssignable, Category = "SKGAttachment|Events")
	FOnAttachmentAdded OnAttachmentAdded;
	// Fires on client and server on replication when the attachment is removed
	UPROPERTY(BlueprintAssignable, Category = "SKGAttachment|Events")
	FOnAttachmentRemoved OnAttachmentRemoved;
	// Fires server side only right before the attachment is marked for destroy
	UPROPERTY(BlueprintAssignable, Category = "SKGAttachment|Events")
	FOnAttachmentDestroyed OnAttachmentDestroyed;
	// Fires on client and server anytime an attachment is added or removed
	UPROPERTY(BlueprintAssignable, Category = "SKGAttachment|Events")
	FOnAttachmentChanged OnAttachmentChanged;
	// Fires on the client and server
	UPROPERTY(BlueprintAssignable, Category = "SKGAttachment|Events")
	FOnOffsetChanged OnOffsetChanged;
	UPROPERTY(BlueprintAssignable, Category = "SKGAttachment|Events")
	FOnPreviewAttachmentAdded OnPreviewAttachmentAdded;
	UPROPERTY(BlueprintAssignable, Category = "SKGAttachment|Events")
	FOnPreviewAttachmentRemoved OnPreviewAttachmentRemoved;

	// The name of the mesh for the spawned attachment to attach to
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment|Initialize")
	FName AttachToMeshName {"StaticMesh"};
	// The name of the socket on the AttachToMeshName mesh for the spawned attachment to attach to
	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintGetter = GetAttachToSocket, Category = "SKGAttachment|Initialize")
	FName AttachToSocket {NAME_None};
	// This is a general variable, useful for customization UI
	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintGetter = GetComponentName, Category = "SKGAttachment|Initialize")
	FName ComponentName {NAME_None};
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetComponentImage, Category = "SKGAttachment|Initialize")
	TObjectPtr<UTexture2D> ComponentImage;
	// Useful if your NetUpdateFrequency is set super low
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment|Initialize")
	bool bAutoCallForceNetUpdate {false};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGAttachment|Initialize")
	FGameplayTagContainer GameplayTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetIsRequiredToFunction, Category = "SKGAttachment|Attachment")
	bool bIsRequiredToFunction {false};
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment|Attachment")
	bool bAutoSetLeaderPoseComponent {false};
	/**
	 *The name of the mesh of the child attachment to be attached, such as a "BackpackMesh" on a character, BackpackMesh would be the name
	 * and attach to the character mesh
	 */
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment|Attachment", meta = (EditCondition = "bAutoSetLeaderPoseComponent"))
	FName LeaderPoseAttachmentMeshName {"SkeletalMesh"};
	// If true, a random attachment from CompatibleAttachments will be used for the default attachment
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment|Attachment")
	bool bRandomDefaultAttachment {false};
	// If set, the class here will be spawned upon construction of this component and spawned
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetDefaultAttachment, Category = "SKGAttachment|Attachment", meta = (EditCondition = "!bRandomDefaultAttachment"))
	TSoftClassPtr<AActor> DefaultAttachment;

	UPROPERTY(EditDefaultsOnly, Replicated, Category = "SKGAttachment|Attachment|Compatibility")
	bool bUseLegacyAttachmentSystem {true};
	// If empty, any attachment can be spawned and attached. If set, only attachments listed here can be attached
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment|Attachment|Compatibility|Legacy", meta = (EditCondition = "bUseLegacyAttachmentSystem"))
	TArray<TObjectPtr<USKGPDAAttachmentCompatibility>> CompatibleAttachments;
	// If this SlotTag is empty, it is assumed you are intending to use the SlotCompatbilityOverride
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "SKGAttachment|Attachment|Compatibility|GameplayTag", meta = (EditCondition = "!bUseLegacyAttachmentSystem"))
	FGameplayTag SlotTag;
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "SKGAttachment|Attachment|Compatibility|GameplayTag", meta = (EditCondition = "!bUseLegacyAttachmentSystem"))
	FSKGAttachmentSlotCompatbility SlotCompatbilityOverride;
	
	// If true, clients can remove, destroy, and add attachments as long as they are the owner of the parent of this component
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment|Attachment|Settings")
	bool bAllowClientSideModification {true};
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetOffsetSettings, Category = "SKGAttachment|Attachment|Settings")
	FSKGAttachmentOffsetSettings OffsetSettings;
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment|Attachment|Settings")
	FSKGAttachmentAttachRules AttachmentRules;

protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Setter, Getter, Category = "SKGAttachment|Attachment")
	TObjectPtr<UMeshComponent> AttachToMesh;

	UPROPERTY(ReplicatedUsing = OnRep_Attachment, BlueprintGetter = GetAttachment, Category = "SKGAttachment|Attachment")
	TObjectPtr<AActor> Attachment;
	UFUNCTION()
	void OnRep_Attachment(AActor* OldAttachment);

	UPROPERTY(ReplicatedUsing = OnRep_AttachmentOffset, BlueprintGetter = GetAttachmentOffset, Category = "SKGAttachment|Attachment")
	float AttachmentOffset {0.0f};
	UFUNCTION()
	void OnRep_AttachmentOffset();
	float OldAttachmentOffset {0.0f};
	
	UPROPERTY()
	TObjectPtr<USKGAttachmentManagerComponent> CachedAttachmentManager;
	TArray<FSKGDAAttachment> CachedCompatibleAttachments; // This should never change so lets cache it

	UPROPERTY()
	TObjectPtr<AActor> PreviewAttachment;
	
	void SetupComponents();
	void SetupCompatibilityCache();
	USKGAttachmentManagerComponent* FindAttachmentManager() const;
	
	FORCEINLINE void TryForceNetUpdate() const;
	FORCEINLINE FVector GetAttachmentDirectionVector() const;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TrySetupAttachment(UClass* AttachmentClass);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TrySetupAttachmentWithData(UClass* AttachmentClass, UDataAsset* DataAsset);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_TrySetupExistingAttachment(AActor* AttachmentToSetup, const bool bAttach);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RemoveAttachment();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DestroyAttachment();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetAttachmentOffset(const float Offset);

	bool IsAttachmentCompatibleWithSlotSystem(const UObject* Object) const;
	
	USKGAttachmentManagerComponent* GetAttachmentManager();
	void RegisterComponentWithManager(USKGAttachmentComponent* Component);
	void UnregisterComponentWithManager(USKGAttachmentComponent* Component);
	void RegisterAttachmentWithManager(AActor* AttachmentToRegister);
	void UnregisterAttachmentWithManager(AActor* AttachmentToUnregister);
	void SetupAttachment(AActor* AttachmentToSetup, const bool bAttach = true);
	void AttachAttachment(AActor* AttachmentToSetup) const;
	void SetupAttachmentWithLeaderPoseComponent(AActor* AttachmentToSetup) const;
	virtual void SetRelativeAttachmentOffset(AActor* AttachmentToSet);
};