// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagAssetInterface.h"
#include "Components/SKGMuzzleComponent.h"
#include "Components/SKGProceduralAnimComponent.h"
#include "DataAssets/SKGPDAFirearmCollisionSettings.h"
#include "DataTypes/SKGShooterFrameworkDataTypes.h"
#include "SKGFirearmComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("SKGShooterFrameworkFirearmComponentStatGroup"), STATGROUP_SKGShooterFrameworkFirearmComponent,
                    STATCAT_Advanced);

class USKGFirearmAttachmentStatComponent;
class USKGMuzzleComponent;
class USKGOffhandIKComponent;
class USKGLightLaserComponent;
class USKGProceduralAnimComponent;
class USKGAttachmentComponent;
class USKGAttachmentManagerComponent;
class USKGOpticComponent;
class USKGPDAFirearmStats;
class USKGStockComponent;
class UCurveVector;
class UMeshComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnProceduralAnimComponentsUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMuzzleComponentUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHeld);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAimingDeviceCycled, USKGProceduralAnimComponent*, ProceduralAnimComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFirearmStatsChanged, FSKGFirearmStats, FirearmStats);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCalculateProceduralStats, const FSKGProceduralStats&, ProceduralStats);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKGSHOOTERFRAMEWORK_API USKGFirearmComponent : public UActorComponent, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USKGFirearmComponent();
	UFUNCTION(BlueprintPure, Category = "SKGShooterFrameworkStatics|Getters")
	static USKGFirearmComponent* GetFirearmComponent(const AActor* Actor);
	// Searches for the firearm component from the Actor that it is attached to
	UFUNCTION(BlueprintPure, Category = "SKGShooterFrameworkStatics|Getters")
	static USKGFirearmComponent* GetParentFirearmComponent(const AActor* Actor);
	// Searches for the firearm that Actor is attached to
	UFUNCTION(BlueprintPure, Category = "SKGShooterFrameworkStatics|Getters")
	static AActor* GetParentWithFirearmComponent(const AActor* Actor);
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }
	void InitializeFirearmComponent();

	// This function will set the cached procedural data used by various firearm components
	UFUNCTION()
	void SetInitialProceduralData();
	void InitializeComponentFromData();
	FORCEINLINE bool HasAuthority() const { return GetOwnerRole() == ROLE_Authority; }
	// Should only be used when manually setting the value for construction
	void SetFirearmMeshComponentName(const FName& Name) { FirearmMeshComponentName = Name; }
	// Should only be used when manually setting the value for construction
	void SetAttachmentManagerComponentName(const FName& Name) { AttachmentManagerComponentName = Name; }
	
	FGameplayTag GetProceduralGameplayTag() const;
	FSKGProceduralAnimInstanceData& GetProceduralData(bool bIsAiming, bool bOffhandIKIsLeftHand);
	UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|ProceduralStats")
	void SetProceduralStats(const FSKGProceduralStats& ProceduralStatsData);
	UFUNCTION(BlueprintPure, Category = "SKGFirearmComponent|ProceduralStats")
	const FSKGProceduralStats& GetProceduralStats() const { return ProceduralAnimData.ProceduralStats; }

	UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Attachments")
	void AddAttachment(AActor* Attachment);
	UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Attachments")
	void RemoveAttachment(AActor* Attachment);
	UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Attachments")
	void OnStockOffsetChanged(const float Offset);
	
	void Held();
	
	UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Zero")
	void ZeroOpticsForZeroAtLocation(const FVector& Location);

	// To be called when you fire your firearm to work with muzzle temperature
	UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Muzzle")
	void ShotPerformed();
	UFUNCTION(BlueprintPure, Category = "SKGFirearmComponent|Muzzle")
	FSKGMuzzleTransform GetMuzzleProjectileTransform(float ZeroDistanceMeters = 100.0f, float MOA = 1.0f) const;
	UFUNCTION(BlueprintPure, Category = "SKGFirearmComponent|Muzzle")
	TArray<FSKGMuzzleTransform> GetMuzzleProjectileTransforms(float ZeroDistanceMeters = 100.0f, float MOA = 1.0f, const int32 ProjectileCount = 8) const;
	UFUNCTION(BlueprintPure, Category = "SKGFirearmComponent|Muzzle")
	FTransform GetMuzzleTransform() const;
	UFUNCTION(BlueprintPure, Category = "SKGFirearm")
	bool IsSuppressed() const;

	UFUNCTION(BlueprintPure, Category = "SKGFirearmComponent|Pose")
	bool GetPose(FGameplayTag Tag, FSKGToFromCurveSettings& PoseData);
	
	UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Aim")
	bool CycleAimingDevice(bool bIsAiming);
	UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Aim")
	bool SetAimingDevice(USKGProceduralAnimComponent* AnimComponent, const bool bIsAiming);
	UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Aim")
	void StartPointAiming(bool bRightHandDominant);
	UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|Aim")
	void StopPointAiming(bool bIsAiming);
	UFUNCTION(BlueprintPure, Category = "SKGFirearmComponent|Aim")
	bool IsPointAiming() const { return bIsPointAiming; }
	UFUNCTION(BlueprintPure, Category = "SKGFirearmComponent|Aim")
	FTransform GetAimSocketWorldTransform() const;
	UFUNCTION(BlueprintPure, Category = "SKGFirearmComponent|Aim")
	TArray<USKGOpticComponent*> GetMagnifiers();

	UFUNCTION(BlueprintCallable, Category = "SKGFirearmComponent|FirearmStats")
	void SetFirearmStats(const FSKGFirearmStats& Stats) { FirearmStats = Stats; }

	UFUNCTION(BlueprintGetter)
	USKGProceduralAnimComponent* GetFirearmProceduralAnimComponent() const { return FirearmProceduralAnimComponent; }
	UFUNCTION(BlueprintGetter)
	USKGOffhandIKComponent* GetFirearmOffhandIKComponent() const { return FirearmOffhandIKComponent; }
	UFUNCTION(BlueprintGetter)
	USKGMuzzleComponent* GetFirearmMuzzleComponent() const { return FirearmMuzzleComponent; }
	
	UFUNCTION(BlueprintGetter)
	const TArray<USKGProceduralAnimComponent*>& GetProceduralAnimComponents() const { return ProceduralAnimComponents; }
	UFUNCTION(BlueprintGetter)
	const TArray<USKGOffhandIKComponent*>& GetOffhandIKComponents() const { return OffhandIKComponents; }
	UFUNCTION(BlueprintGetter)
	const TArray<USKGLightLaserComponent*>& GetLightLaserComponents() const { return LightLaserComponents; }
	UFUNCTION(BlueprintGetter)
	const TArray<USKGMuzzleComponent*>& GetMuzzleComponents() const { return MuzzleComponents; }
	UFUNCTION(BlueprintGetter)
	const TArray<USKGFirearmAttachmentStatComponent*>& GetAttachmentStatComponents() const { return AttachmentStatComponents; }
	UFUNCTION(BlueprintGetter)
	const FSKGFirearmStats& GetFirearmStats() const { return FirearmStats; }
	UFUNCTION(BlueprintGetter)
	const USKGAttachmentManagerComponent* GetAttachmentManagerComponent() const { return AttachmentManager; }
	UFUNCTION(BlueprintGetter)
	USKGProceduralAnimComponent* GetCurrentProceduralAnimComponent() const { return CurrentProceduralAnimComponent; }	
	UFUNCTION(BlueprintGetter)
	USKGOffhandIKComponent* GetCurrentOffhandIKComponent() const { return CurrentOffhandIKComponent; }
	UFUNCTION(BlueprintGetter)
	USKGMuzzleComponent* GetCurrentMuzzleComponent() const { return CurrentMuzzleComponent; }
	UFUNCTION(BlueprintGetter)
	USKGOpticComponent* GetCurrentOpticComponent() const { return CurrentOpticComponent; }
	UFUNCTION(BlueprintGetter)
	USKGStockComponent* GetCurrentStockComponent() const { return CurrentStockComponent; }
	
	UPROPERTY(BlueprintAssignable, Category = "SKGFirearmComponent|Events")
	FOnAimingDeviceCycled OnAimingDeviceCycled;
	UPROPERTY(BlueprintAssignable, Category = "SKGFirearmComponent|Events")
	FOnProceduralAnimComponentsUpdated OnProceduralAnimComponentsUpdated;
	UPROPERTY(BlueprintAssignable, Category = "SKGFirearmComponent|Events")
	FOnMuzzleComponentUpdated OnMuzzleComponentUpdated;
	UPROPERTY(BlueprintAssignable, Category = "SKGFirearmComponent|Events")
	FOnHeld OnHeld;
	UPROPERTY(BlueprintAssignable, Category = "SKGFirearmComponent|Events")
	FOnFirearmStatsChanged OnFirearmStatsChanged;
	/*
	 * The purpose of this delegate/event is so you do not have to make a child class of this component to override CalculateProceduralValues.
	 * This is intended moreso for blueprint so you simply override this event, create your SKGProceduralStats with values you want,
	 * then call SetProceduralStats. You can get the current values by calling GetProceduralStats as well. This event fires from the
	 * CalculateProceduralValues_Implementation AFTER the defaults are set. If you override CalculateProceduralValues_Implementation without
	 * calling the parent this event will not fire.
	 */
	UPROPERTY(BlueprintAssignable, Category = "SKGFirearmComponent|Events")
	FCalculateProceduralStats CalculateProceduralStats;

	UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent|Initialize")
	bool bAutoInitialize {true};
	// The name of the mesh to be used/considered the firearm
	UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent|Initialize")
	FName FirearmMeshComponentName {NAME_None};
	// The name of the OPTIONAL attachment manager added to this parent actor class
	UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent|Initialize")
	FName AttachmentManagerComponentName {NAME_None};
	UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent|Initialize")
	TObjectPtr<USKGPDAFirearmStats> FirearmStatsDataAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFirearmComponent|Initialize")
	FGameplayTagContainer GameplayTags;
	UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent|FirearmCollision")
	TObjectPtr<USKGPDAFirearmCollisionSettings> FirearmCollisionSettingsDataAsset;

protected:
	FSKGFirearmCollisionSettings FirearmCollisionSettings;

	UPROPERTY()
	TObjectPtr<UMeshComponent> FirearmMesh;
	UPROPERTY(BlueprintGetter = GetAttachmentManagerComponent, Category = "SKGFirearmComponent|Components")
	TObjectPtr<USKGAttachmentManagerComponent> AttachmentManager;

	UPROPERTY(ReplicatedUsing = OnRep_FirearmStats, BlueprintGetter = GetFirearmStats, Category = "SKGFirearmComponent|FirearmStats")
	FSKGFirearmStats FirearmStats;
	UFUNCTION()
	void OnRep_FirearmStats();
	
	// Consider removing and making Index 0 reserved for firearm specific components
	UPROPERTY(BlueprintGetter = GetFirearmProceduralAnimComponent, Category = "SKGFirearmComponent|Components")
	TObjectPtr<USKGProceduralAnimComponent> FirearmProceduralAnimComponent;
	UPROPERTY(BlueprintGetter = GetFirearmOffhandIKComponent, Category = "SKGFirearmComponent|Components")
	TObjectPtr<USKGOffhandIKComponent> FirearmOffhandIKComponent;
	UPROPERTY(BlueprintGetter = GetFirearmMuzzleComponent, Category = "SKGFirearmComponent|Components")
	TObjectPtr<USKGMuzzleComponent> FirearmMuzzleComponent;

	bool bIsInitialized {false};
	
	int32 ProceduralAnimComponentIndex {0};
	UPROPERTY(BlueprintGetter = GetProceduralAnimComponents, ReplicatedUsing = OnRep_ProceduralAnimComponents, Category = "SKGFirearmComponent|Components")
	TArray<TObjectPtr<USKGProceduralAnimComponent>> ProceduralAnimComponents;
	UFUNCTION()
	virtual void OnRep_ProceduralAnimComponents();
	
	UPROPERTY(BlueprintGetter = GetOffhandIKComponents, ReplicatedUsing = OnRep_OffhandIKComponents, Category = "SKGFirearmComponent|Components")
	TArray<TObjectPtr<USKGOffhandIKComponent>> OffhandIKComponents;
	UFUNCTION()
	virtual void OnRep_OffhandIKComponents() {}
	
	UPROPERTY(BlueprintGetter = GetLightLaserComponents, ReplicatedUsing = OnRep_LightLaserComponents, Category = "SKGFirearmComponent|Components")
	TArray<TObjectPtr<USKGLightLaserComponent>> LightLaserComponents;
	UFUNCTION()
	virtual void OnRep_LightLaserComponents() {}
	
	UPROPERTY(BlueprintGetter = GetMuzzleComponents, ReplicatedUsing = OnRep_MuzzleComponents, Category = "SKGFirearmComponent|Components")
	TArray<TObjectPtr<USKGMuzzleComponent>> MuzzleComponents;
	UFUNCTION()
	virtual void OnRep_MuzzleComponents() {}

	UPROPERTY(BlueprintGetter = GetAttachmentStatComponents, ReplicatedUsing = OnRep_AttachmentStatComponents, Category = "SKGFirearmComponent|Components")
	TArray<TObjectPtr<USKGFirearmAttachmentStatComponent>> AttachmentStatComponents;
	UFUNCTION()
	virtual void OnRep_AttachmentStatComponents();
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentProceduralAnimComponent, BlueprintGetter = GetCurrentProceduralAnimComponent, Category = "SKGFirearmComponent|Components")
	TObjectPtr<USKGProceduralAnimComponent> CurrentProceduralAnimComponent;
	UFUNCTION()
	virtual void OnRep_CurrentProceduralAnimComponent();
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentOffhandIKComponent, BlueprintGetter = GetCurrentOffhandIKComponent, Category = "SKGFirearmComponent|Components")
	TObjectPtr<USKGOffhandIKComponent> CurrentOffhandIKComponent;
	UFUNCTION()
	virtual void OnRep_CurrentOffhandIKComponent() {}
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentMuzzleComponent, BlueprintGetter = GetCurrentMuzzleComponent, Category = "SKGFirearmComponent|Components")
	TObjectPtr<USKGMuzzleComponent> CurrentMuzzleComponent;
	UFUNCTION()
	virtual void OnRep_CurrentMuzzleComponent();
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentOpticComponent, BlueprintGetter = GetCurrentOpticComponent, Category = "SKGFirearmComponent|Components")
	TObjectPtr<USKGOpticComponent> CurrentOpticComponent;
	UFUNCTION()
	virtual void OnRep_CurrentOpticComponent() {}

	UPROPERTY(ReplicatedUsing = OnRep_CurrentStockComponent, BlueprintGetter = GetCurrentStockComponent, Category = "SKGFirearmComponent|Components")
	TObjectPtr<USKGStockComponent> CurrentStockComponent;
	UFUNCTION()
	virtual void OnRep_CurrentStockComponent() {}

	UPROPERTY()
	TObjectPtr<USKGProceduralAnimComponent> BeforePointAimProceduralAnimComponent;
	bool bIsPointAiming {false};

	FSKGProceduralAnimInstanceData ProceduralAnimData;
	FVector CachedAimingOffset {-FVector::OneVector};
	void CacheAimingOffset();
	
	bool bOldOffhandIKIsLeftHand {true};
	
	void SetBestMuzzleComponent();
	void SetBestOffhandIKComponent();
	void SetBestProceduralAnimComponent();
	
	void SetupComponents();
	// Returns true if changed
	bool SetOpticComponent();

	UFUNCTION(BlueprintNativeEvent, Category = "SKGFirearmComponent|Stats")
	void AddFirearmAttachmentStats(USKGFirearmAttachmentStatComponent* StatComponent);
	virtual void AddFirearmAttachmentStats_Implementation(USKGFirearmAttachmentStatComponent* StatComponent);
	UFUNCTION(BlueprintNativeEvent, Category = "SKGFirearmComponent|Stats")
	void RemoveFirearmAttachmentStats(USKGFirearmAttachmentStatComponent* StatComponent);
	virtual void RemoveFirearmAttachmentStats_Implementation(USKGFirearmAttachmentStatComponent* StatComponent);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGFirearmComponent|Stats")
	void CalculateProceduralValues();
	virtual void CalculateProceduralValues_Implementation();

	// DELEGATES BOUND FROM COMPONENTS
	UFUNCTION()
	void OnAttachmentAdded(AActor* Attachment);
	UFUNCTION()
	void OnAttachmentRemoved(AActor* Attachment);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetAimingDevice(USKGProceduralAnimComponent* AnimComponent);
	bool SetupNewAimingDevice(USKGProceduralAnimComponent* AnimComponent, const bool bIsAiming);
};