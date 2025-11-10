// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "Components/ActorComponent.h"
#include "DataTypes/SKGProceduralDataTypes.h"
#include "SKGProceduralAnimComponent.generated.h"

class USKGPDAProceduralAnimInitialize;
class USKGPDACycleAimingPointSettings;
class USKGPDAProceduralPoseSettings;
class USKGPDAProceduralOffsets;
class USKGPDARecoilSettings;
class USKGPDAMovementLagSettings;
class USKGPDARotationLagSettings;
class USKGPDADeadzoneSettings;
class USKGPDAMovementSwaySettings;
class USKGPDAAimingSettings;
class UCurveVector;
class UMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCanAimChanged, const USKGProceduralAnimComponent*, Component, const bool, bCanAim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPointAimStateChanged, bool, bPointAiming);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAimSocketCycled, const FName&, NewSocket);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAimSocketCycledReplicated, const FName&, NewSocket);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnProceduralDataAssetsChanged);

// Component to be applied for procedurals such as aiming, sway, poses, recoil, etc
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKGPROCEDURALANIM_API USKGProceduralAnimComponent : public UActorComponent, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	struct FDefaultProceduralSettingsDataAssets
	{
		USKGPDAAimingSettings* DefaultAimingSettings;
		USKGPDAProceduralOffsets* DefaultProceduralOffsets;
		USKGPDACycleAimingPointSettings* DefaultCycleAimingPointSettings;
		USKGPDAMovementSwaySettings* DefaultMovementSwaySettings;
		USKGPDAMovementLagSettings* DefaultMovementLagSettings;
		USKGPDARotationLagSettings* DefaultRotationLagSettings;
		USKGPDADeadzoneSettings* DefaultDeadzoneSettings;
		USKGPDARecoilSettings* DefaultRecoilSettings;
		TArray<USKGPDAProceduralPoseSettings*> DefaultPoses;
	};

	FDefaultProceduralSettingsDataAssets DefaultProceduralSettingsDataAssets;
	
public:
	// Sets default values for this component's properties
	USKGProceduralAnimComponent();
	UFUNCTION(BlueprintPure, Category = "SKGShooterFrameworkStatics|Getters")
	static USKGProceduralAnimComponent* GetProceduralAnimComponent(const AActor* Actor);
	
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }
	void InitializeProceduralAnimComponent();

	void InitializeComponentFromData();
	FORCEINLINE bool HasAuthority() const { return GetOwnerRole() == ROLE_Authority; }
	// Should only be used when manually setting the value for construction
	void SetProceduralMeshName(const FName& Name) { ProceduralMeshName = Name; }
	// Should only be used when manually setting the value for construction
	void SetProceduralAimSocketNames(const TArray<FName>& Names) { ProceduralAimSocketNames = Names; }
	// Should only be used when manually setting the value for construction
	void SetRightHandDominatePointAimAimSocketIndex(int32 Index) { RightHandDominatePointAimAimSocketIndex = Index; }
	// Should only be used when manually setting the value for construction
	void SetLeftHandDominatePointAimAimSocketIndex(int32 Index) { LeftHandDominatePointAimAimSocketIndex = Index; }
	// Should only be used when manually setting the value for construction
	void SetProceduralAnimGameplayTag(const FGameplayTag& Tag) { ProceduralAnimGameplayTag = Tag; }
	
	UFUNCTION(BlueprintPure, Category = "SKGProceduralAnimComponent|Aim")
	const FSKGProceduralAimingSettings& GetProceduralAimingSettings() const { return AimingSettings; }
	UFUNCTION(BlueprintPure, Category = "SKGProceduralAnimComponent|Aim")
	bool CanAim() const { return AimingSettings.bCanAim; }
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|Aim")
	void SetCanAim(bool bCanAim) { if (AimingSettings.bCanAim != bCanAim) { AimingSettings.bCanAim = bCanAim; OnCanAimChanged.Broadcast(this, bCanAim);} }
	UFUNCTION(BlueprintPure, Category = "SKGProceduralAnimComponent|Aim")
	FTransform GetAimWorldTransform() const;
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|Aim")
	bool CycleAimSocket();
	// Returns true if point aim possible and set successfully
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|Aim")
	bool StartPointAiming(bool bRightHandDominant);
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|Aim")
	void StopPointAiming();
	// If ComponentRelativeTo is null, then self will be used
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnim|Aim")
	void UpdateAimOffset(UMeshComponent* ComponentRelativeTo, FVector BaseDistance = FVector::ZeroVector);
	// If ComponentRelativeTo is null, then self will be used
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnim|Aim")
	void UpdateAimOffsetWithSocket(UMeshComponent* ComponentRelativeTo, const FName& Socket);

	UFUNCTION(BlueprintPure, Category = "SKGProceduralAnim|Pose")
	bool GetPose(FGameplayTag Tag, FSKGToFromCurveSettings& PoseData);
	
	/**
	 * Overrideable, utilizes the aim socket to calculate where to zero the muzzle at
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGProceduralAnimComponent|Muzzle")
	FTransform GetAimMuzzleTransform();
	virtual FTransform GetAimMuzzleTransform_Implementation();
	
	const FGameplayTag GetProceduralGameplayTag() const { return ProceduralAnimGameplayTag; }
	const FSKGFirstAndThirdPersonBasePoseOffset& GetBasePoseOffset() const { return BasePoseOffset; }
	const FTransform& GetThirdPersonAimingOffset(bool bRightHanded) const;
	const FSKGCycleAimingPointSettings& GetCycleAimingPointSettings() const { return CycleAimingPointSettings; }
	const FSKGMovementSwaySettings& GetMovementSwaySettings() const { return MovementSwaySettings; }
	const FSKGMovementLagSettings& GetMovementLagSettings() const { return MovementLagSettings; }
	const FSKGRotationLagSettings& GetRotationLagSettings() const { return RotationLagSettings; }
	const FSKGDeadzoneSettings& GetDeadzoneSettings() const { return DeadzoneSettings; }
	const FSKGRecoilSettings& GetRecoilSettings() const { return RecoilSettings; }

	UFUNCTION(BlueprintGetter)
	bool CanUseLeftHandDominate() const { return bCanUseLeftHandDominate; }
	UFUNCTION(BlueprintGetter)
	FORCEINLINE FTransform GetAimOffset() const { return CurrentAimSocketTransform; }
	UFUNCTION(BlueprintGetter)
	FORCEINLINE UMeshComponent* GetProceduralAnimMesh() const { return ProceduralAnimMesh; }
	template< typename T >
	FORCEINLINE T* GetProceduralAnimMesh() const { return Cast<T>(ProceduralAnimMesh); }

	// This fires only on the calling client when the ability to aim is toggled with this component
	UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
	FOnCanAimChanged OnCanAimChanged;
	// This fires only on the calling client when the point aim state changes
	UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
	FOnPointAimStateChanged OnPointAimStateChanged;
	// This fires only on the calling client when the aim socket is changed
	UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
	FOnAimSocketCycled OnAimSocketCycled;
	// This fires for everyone including the calling client when the aim socket is replicated
	UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
	FOnAimSocketCycledReplicated OnAimSocketCycledReplicated;
	UPROPERTY(BlueprintAssignable, Category = "SKGProceduralAnimComponent|Events")
	FOnProceduralDataAssetsChanged OnProceduralDataAssetsChanged;
	
	// if true, the component will auto initialize itself upon creation
	UPROPERTY(EditDefaultsOnly, Category = "SKGProceduralAnim|Initialize")
	bool bAutoInitialize {true};
	// if true, this assumes you will set the ProceduralMeshName in code before initialization
	UPROPERTY(EditDefaultsOnly, Category = "SKGProceduralAnim|Initialize")
	bool bOverrideComponentNames {false};
	UPROPERTY(EditDefaultsOnly, Category = "SKGProceduralAnim|Initialize")
	TObjectPtr<USKGPDAProceduralAnimInitialize> InitializationSettingsDataAsset;

	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void SetAimingSettingsSettings(USKGPDAAimingSettings* DataAsset);
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void RevertAimingSettings();
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void SetProceduralOffsets(USKGPDAProceduralOffsets* DataAsset);
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void RevertProceduralOffsetsSettings();
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void SetCycleAimingPointSettings(USKGPDACycleAimingPointSettings* DataAsset);
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void RevertCycleAimingPointSettings();
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void SetMovementSwaySettings(USKGPDAMovementSwaySettings* DataAsset);
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void RevertMovementSwaySettings();
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void SetMovementLagSettings(USKGPDAMovementLagSettings* DataAsset);
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void RevertMovementLagSettings();
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void SetRotationLagSettings(USKGPDARotationLagSettings* DataAsset);
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void RevertRotationLagSettings();
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void SetDeadzoneSettings(USKGPDADeadzoneSettings* DataAsset);
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void RevertDeadzoneSettings();
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void SetRecoilSettings(USKGPDARecoilSettings* DataAsset);
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void RevertRecoilSettings();
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void SetProceduralPoseSettings(TArray<USKGPDAProceduralPoseSettings*> NewPoses);
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void RevertProceduralPoseSettings();
	UFUNCTION(BlueprintCallable, Category = "SKGProceduralAnimComponent|DataAssets")
	void RevertAllSettingsToDefault();
	
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_SettingsDataAssetChanged, Category = "SKGProceduralAnim|Procedural")
	TObjectPtr<USKGPDAAimingSettings> AimingSettingsDataAsset;
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_SettingsDataAssetChanged, Category = "SKGProceduralAnim|Procedural")
	TObjectPtr<USKGPDAProceduralOffsets> ProceduralOffsetsDataAsset;
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_SettingsDataAssetChanged, Category = "SKGProceduralAnim|Procedural")
	TObjectPtr<USKGPDACycleAimingPointSettings> CycleAimingPointSettingsDataAsset;
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_SettingsDataAssetChanged, Category = "SKGProceduralAnim|Procedural")
	TObjectPtr<USKGPDAMovementSwaySettings> MovementSwaySettingsDataAsset;
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_SettingsDataAssetChanged, Category = "SKGProceduralAnim|Procedural")
	TObjectPtr<USKGPDAMovementLagSettings> MovementLagSettingsDataAsset;
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_SettingsDataAssetChanged, Category = "SKGProceduralAnim|Procedural")
	TObjectPtr<USKGPDARotationLagSettings> RotationLagSettingsDataAsset;
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_SettingsDataAssetChanged, Category = "SKGProceduralAnim|Procedural")
	TObjectPtr<USKGPDADeadzoneSettings> DeadzoneSettingsDataAsset;
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_SettingsDataAssetChanged, Category = "SKGProceduralAnim|Procedural")
	TObjectPtr<USKGPDARecoilSettings> RecoilSettingsDataAsset;
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_SettingsDataAssetChanged, Category = "SKGProceduralAnim|Procedural")
	TArray<TObjectPtr<USKGPDAProceduralPoseSettings>> PoseSettingsDataAssets;

	UFUNCTION()
	void OnRep_SettingsDataAssetChanged();
	
	// The mesh name to be found for the to be used for an optic, laser, firearm, etc. to aim with
	FName ProceduralMeshName {"StaticMesh"};
	// The sockets on the ProceduralMeshName that can be used for aiming
	TArray<FName> ProceduralAimSocketNames {"S_Aim"};
	/** The index to use if you decide to point aim. Primarily useful for the
	 * procedural anim component on a firearm. -1 = no point aim socket
	 */
	int32 RightHandDominatePointAimAimSocketIndex {INDEX_NONE};
	/** The index to use if you decide to point aim. Primarily useful for the
	 * procedural anim component on a firearm. -1 = no point aim socket
	 */
	int32 LeftHandDominatePointAimAimSocketIndex {INDEX_NONE};
	// If this is true, this allows you to enter left hand mode if you have the anims setup.
	bool bCanUseLeftHandDominate {false};
	// Used to tell what you are holding (such as a rifle, pistol, optic, etc)
	FGameplayTag ProceduralAnimGameplayTag;
	FGameplayTagContainer GameplayTags;

protected:
	FSKGProceduralAimingSettings AimingSettings;
	// The held offset position of the held actor. Separate for first and third person
	FSKGFirstAndThirdPersonBasePoseOffset BasePoseOffset;
	// Offset applied when aiming for third person view with left hand holding firearm
	FTransform ThirdPersonRightHandedAimingOffset;
	// Offset applied when aiming for third person view with right hand holding firearm
	FTransform ThirdPersonLeftHandedAimingOffset;
	FSKGCycleAimingPointSettings CycleAimingPointSettings;
	FSKGMovementSwaySettings MovementSwaySettings;
	FSKGMovementLagSettings MovementLagSettings;
	FSKGRotationLagSettings RotationLagSettings;
	FSKGDeadzoneSettings DeadzoneSettings;
	FSKGRecoilSettings RecoilSettings;
	TArray<FSKGToFromCurveSettings> PoseSettings;
	
	UPROPERTY(BlueprintGetter = GetProceduralAnimMesh, Category = "SKGProceduralAnim|Mesh")
	TObjectPtr<UMeshComponent> ProceduralAnimMesh;
	bool bUsedForAiming {false};
	
	void SetupComponents();

	int32 PreviousAimSocketIndex {0};
	UPROPERTY(ReplicatedUsing = OnRep_AimSocketIndex)
	uint8 AimSocketIndex {0};
	UPROPERTY(BlueprintGetter = GetAimOffset, Category = "SKGProceduralAnim")
	FTransform CurrentAimSocketTransform;
	UPROPERTY()
	TObjectPtr<UMeshComponent> LastRelativeToOffset;

	UFUNCTION()
	void OnRep_AimSocketIndex() const;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetAimSocketIndex(uint8 Index);
};
