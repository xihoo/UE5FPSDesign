// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagAssetInterface.h"
#include "Animation/SKGShooterFrameworkAnimInstance.h"
#include "SKGShooterPawnComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("SKGShooterPawnComponent"), STATGROUP_SKGShooterPawnComponent, STATCAT_Advanced);

class USKGLightLaserComponent;
class USKGOpticComponent;
class UCurveVector;
class USKGShooterFrameworkAnimInstance;
class ISKGInfraredInterface;
class USKGFirearmComponent;
class USKGProceduralAnimComponent;
class UCameraComponent;
// @TODO Uncomment for 5.4+
class UGameplayCameraComponent;
class USkeletalMeshComponent;
class UMaterialParameterCollection;
class APawn;

USTRUCT(BlueprintType)
struct FSKGFreeLookSettings
{
	GENERATED_BODY()
	// If true, it will auto set bUseControllerRotationYaw to true and false
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFreeLookSettings")
	bool bAutoSetUseControllerRotationYaw {true};
	// The max you can look up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFreeLookSettings")
	float MaxPitchUp {40};
	// The max you can look down
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFreeLookSettings")
	float MaxPitchDown {30};
	// The max you can look left
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFreeLookSettings")
	float MaxYawLeft {35};
	// The max you can look right
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGFreeLookSettings")
	float MaxYawRight {35};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPoseComplete, const FSKGProceduralPoseReplicationData&, CurrentPoseData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHeldActorSet, AActor*, NewHeldActor, AActor*, OldHeldActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnimInstanceTicked, float, DeltaTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAimStateChanged, bool, bAiming);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFreeLookStateChanged, bool, bInFreeLook);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOffhandIKIsLeftHandChanged, bool, bOffhandIsLeftHand);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFirearmCollisionChanged, bool, bIsColliding);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKGSHOOTERFRAMEWORK_API USKGShooterPawnComponent : public UActorComponent, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	friend USKGShooterFrameworkAnimInstance;
	
public:	
	// Sets default values for this component's properties
	USKGShooterPawnComponent();
	UFUNCTION(BlueprintPure, Category = "SKGShooterFrameworkStatics|Getters")
	static USKGShooterPawnComponent* GetShooterPawnComponent(const AActor* Actor);
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }

	FORCEINLINE bool HasAuthority() const { return GetOwnerRole() == ROLE_Authority; }
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|HeldActor")
	void SetHeldActor(AActor* Actor);
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|HeldActor")
	void ClearHeldActor();
	
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|AnimationLayer")
	USKGShooterFrameworkAnimInstance* LinkAnimLayerClass(TSubclassOf<USKGShooterFrameworkAnimInstance> Class);
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|AnimationLayer")
	USKGShooterFrameworkAnimInstance* LinkAnimLayerClassByInstance(UAnimInstance* AnimInstance, TSubclassOf<USKGShooterFrameworkAnimInstance> Class);
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|AnimationLayer")
	void UnlinkAnimLayerClass();
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|AnimationLayer")
	void UnlinkAnimLayerClassByInstance(UAnimInstance* AnimInstance);
	// This function gets called by the anim instance each time the NativeUpdateAnimation runs at the end
	virtual void AnimInstanceTicked(float DeltaSeconds);
	
	// Used for freelook. If false, it means control rotation goes to far
	UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|FreeLook")
	bool CanAddYaw() const;
	// Used for freelook. If false, it means control rotation goes to far
	UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|FreeLook")
	bool CanAddPitch() const;
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|FreeLook")
	void StartFreeLook();
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|FreeLook")
	void StopFreeLook();
	UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|FreeLook")
	bool IsFreeLooking() const { return bInFreeLook; }

	// Used for deadzone
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|MouseInput")
	void SetMouseInput(float X, float Y);

	bool NeedsToContinuallyUpdateCameraOffset() const { return bConstantlyUpdateCameraOffset; }
	void SetCameraOffset();
	FORCEINLINE FTransform GetCameraOffset() const { return CameraOffset; }
	FSKGProceduralAnimInstanceData GetProceduralData();

	UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Sensitivity")
	virtual float GetMagnificationSensitivityMultiplier() const;
	// Outputs new sensivity based on whether or not free looking, zoomed with magnification, utilizes base turn rate
	UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Sensitivity")
	virtual void GetSensitivityMultiplier(const float X, const float XBaseTurnRate, const float Y, const float YBaseTurnRate, float& NewX, bool& AddYaw, float& NewY, bool& AddPitch) const;
	
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|Leaning")
	void LeanLeft(float TargetAngle);
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|Leaning")
	void LeanRight(float TargetAngle);
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|Leaning")
	void StopLeaningLeft();
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|Leaning")
	void StopLeaningRight();
	UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Leaning")
	FORCEINLINE float GetTargetLeanAngle() const { return TargetLeanAngle; }
	
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	void StartAiming();
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	void StartAimingAI();
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	void StopAiming();
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	void StopAimingAI();
	UFUNCTION(BlueprintGetter, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	FORCEINLINE bool IsAiming() const { return bIsAiming; }
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	void SetUseFirstPersonProceduralsAsLocal();
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	void SetUseThirdPersonProceduralsAsLocal();
	UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	bool IsUsingFirstPersonProceduralsAsLocal() const { return bUseFirstPersonProceduralsAsLocal; }
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	void SetSwayMultiplier(const float Multiplier);
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	void ResetSwayMultiplier();
	UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	bool GetUsingCustomSwayMultiplier() const { return bUsingCustomSwayMultiplier; }
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	void PerformProceduralRecoil(const FRotator ControlRotationMultiplier = FRotator(1.0, 1.0, 1.0), const FVector LocationMultiplier = FVector(1.0), const FRotator RotationMultiplier = FRotator(1.0, 1.0, 1.0));
	/**
		 * @param Tag used to search for the given pose
		 * @param bExitPose whether or not to use the return curve (return to normal) or the to curve (go from normal to pose)
		 */
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	void PerformProceduralPose(const FGameplayTag Tag, bool bExitPose);
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	void PerformCustomCurve(const FSKGCurveSettings& CurveData);
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	void SetOffhandIKToLeftHand();
	UFUNCTION(BlueprintCallable, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	void SetOffhandIKToRightHand();
	UFUNCTION(BlueprintGetter, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	const FSKGProceduralPoseReplicationData& GetCurrentProceduralPoseReplicationData() const { return CurrentProceduralPoseData; }
	UFUNCTION(BlueprintGetter, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	bool GetOffhandIKIsLeftHand() const { return bOffhandIKIsLeftHand; }
	UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	FSKGToFromCurveSettings GetProceduralPoseData(const FGameplayTag& Tag) const;
	UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|ProceduralAnimation")
	float GetFirearmCollisionPushAmount() const;
	
	UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Components")
	USKGFirearmComponent* GetCurrentFirearmComponent() const { return CurrentFirearmComponent; }
	UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Components")
	USKGProceduralAnimComponent* GetCurrentProceduralAnimComponent() const;
	UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Components")
	USKGOpticComponent* GetCurrentOpticComponent() const;
	UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Components")
	TArray<USKGLightLaserComponent*> GetCurrentLightLaserComponents() const;
	
	UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Misc")
	bool IsLocallyControlled() const;
	/* Returns control rotation if local controlled, returns replicated
	 * rotator (remote yaw and remote pitch) if remote client
	 */
	UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Misc")
	FRotator GetControlRotation() const;

	// Returns First Person Mesh if local, Third Person Mesh if remote
	UFUNCTION(BlueprintPure, Category = "SKGShooterPawnComponent|Mesh")
	USkeletalMeshComponent* GetPawnMesh() const;
	UFUNCTION(BlueprintGetter)
	USkeletalMeshComponent* GetFirstPersonMesh() const { return MeshFP; }
	UFUNCTION(BlueprintGetter)
	USkeletalMeshComponent* GetThirdPersonMesh() const { return MeshTP; }
	
	UFUNCTION(BlueprintGetter)
	const FSKGProceduralShooterPawnData& GetProceduralShooterPawnData() const { return ProceduralShooterPawnData; }
	UFUNCTION(BlueprintGetter)
	UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	// @TODO Uncomment for 5.4+ since #if ENGINE_MINOR_VERSION >= 4 doesnt work with UE header tool.
	UFUNCTION(BlueprintGetter)
	UGameplayCameraComponent* GetGameplayCameraComponent() const { return GameplayCameraComponent; }
	UFUNCTION(BlueprintGetter)
	APawn* GetOwningPawn() const { return OwningPawn; }
	template< typename T >
	T* GetOwningPawn() const { return Cast<T>(GetOwningPawn()); }
	UFUNCTION(BlueprintGetter)
	USKGShooterFrameworkAnimInstance* GetShooterFrameworkAnimInstance() const { return ShooterFrameworkAnimInstance; }
	template< typename T >
	T* GetShooterFrameworkAnimInstance() const { return Cast<T>(GetShooterFrameworkAnimInstance()); }
	
	UFUNCTION(BlueprintGetter)
	AActor* GetHeldActor() const { return HeldActor; }
	template< typename T >
	T* GetHeldActor() const { return Cast<T>(GetHeldActor()); }

	// Blueprint specific overrideable functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGShooterPawnComponent|Sensitivity")
	float BP_GetMagnificationSensitivityMultiplier() const;
	virtual float BP_GetMagnificationSensitivityMultiplier_Implementation() const;
	// Outputs new sensivity based on whether or not free looking, zoomed with magnification, utilizes base turn rate
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SKGShooterPawnComponent|Sensitivity")
	void BP_GetSensitivityMultiplier(const float X, const float XBaseTurnRate, const float Y, const float YBaseTurnRate, float& NewX, bool& AddYaw, float& NewY, bool& AddPitch) const;
	virtual void BP_GetSensitivityMultiplier_Implementation(const float X, const float XBaseTurnRate, const float Y, const float YBaseTurnRate, float& NewX, bool& AddYaw, float& NewY, bool& AddPitch) const;
	
	UPROPERTY(BlueprintAssignable, Category = "SKGShooterPawnComponent|Events")
	FOnPoseComplete OnPoseComplete;
	UPROPERTY(BlueprintAssignable, Category = "SKGShooterPawnComponent|Events")
	FOnHeldActorSet OnHeldActorSet;
	// This fires at the end of the Anim Instance tick
	UPROPERTY(BlueprintAssignable, Category = "SKGShooterPawnComponent|Events")
	FOnAnimInstanceTicked OnAnimInstanceTicked;
	UPROPERTY(BlueprintAssignable, Category = "SKGShooterPawnComponent|Events")
	FOnAimStateChanged OnAimStateChanged;
	UPROPERTY(BlueprintAssignable, Category = "SKGShooterPawnComponent|Events")
	FOnFreeLookStateChanged OnFreeLookStateChanged;
	UPROPERTY(BlueprintAssignable, Category = "SKGShooterPawnComponent|Events")
	FOnOffhandIKIsLeftHandChanged OnOffhandIKIsLeftHandChanged;
	// This will fire when the firearm starts colliding and stops colliding.
	UPROPERTY(BlueprintAssignable, Category = "SKGShooterPawnComponent|Events")
	FOnFirearmCollisionChanged OnFirearmCollisionChanged;

#if WITH_EDITORONLY_DATA
	// If true, firearm collision will be printed
	UPROPERTY(EditDefaultsOnly, Category = "SKGShooterFramework|Debug")
	bool bPrintHit {false};
	UPROPERTY(EditDefaultsOnly, Category = "SKGShooterFramework|Debug")
	bool bDrawDebugTrace {false};
#endif

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
	bool bUsingLegacyInputScales {true};
	/** The linked anim layer class to be used instead of the actual anim instance of the mesh
	 * so you do not need to reparent your animbp. Refer to example project for usage
	 */
	UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
	TSubclassOf<USKGShooterFrameworkAnimInstance> LinkedAnimLayerClass;
	/**
	 * If true, it will auto link the LinkedAnimLayerClass to your mesh anim instance for the procedurals.
	 * If LinkedAnimLayerClass is invalid, it will automatically try to get the anim instance from your pawns mesh rather than
	 * link a AnimLayer.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
	bool bAutoSetupLinkedAnimLayer {true};
	// If true, it will use the first found match as the first and third person mesh
	UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
	bool bUseSingleMesh {true};
	// The name of the Pawns Third Person Mesh Component
	UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
	FName ThirdPersonMeshComponentName {"CharacterMesh0"};
	// The name of the Pawns First Person Mesh Component
	UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
	FName FirstPersonMeshComponentName {"MeshFP"};
	// The name of the Pawns Camera Component
	UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
	FName CameraComponentName {"CameraComponent"};
	// If the CameraComponent is invalid/not found, this socket will be used as the CameraOffset for aiming and such
	UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
	FName CameraAttachedSocket {"S_Camera"};
	// If true, the camera offset will be continually updated (such as if your camera is moving from the head)
	UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
	bool bConstantlyUpdateCameraOffset {false};
	// Used for free look, turn in place, etc
	UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize")
	bool bReplicateRemoteYaw {true};
	// Replication rate for the Yaw. 0 = every frame
	UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Initialize", meta = (EditCondition = "bReplicateRemoteYaw"))
	double RemoteYawReplicationRate {0.1};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGShooterPawnComponent|Initialize")
	FGameplayTagContainer GameplayTags;
	
	UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|FreeLook")
	FSKGFreeLookSettings FreeLookSettings;
	UPROPERTY(EditDefaultsOnly, Category = "SKGShooterPawnComponent|Leaning")
	FSKGLeanLeftRightSettings LeanLeftRightSettings;

	UPROPERTY(BlueprintGetter = GetCameraComponent, Category = "SKGShooterPawnComponent")
	TObjectPtr<UCameraComponent> CameraComponent;
	// @TODO Uncomment for 5.4+ since #if ENGINE_MINOR_VERSION >= 4 doesnt work for with UE header tool.
	UPROPERTY(BlueprintGetter = GetGameplayCameraComponent, Category = "SKGShooterPawnComponent")
	TObjectPtr<UGameplayCameraComponent> GameplayCameraComponent;
	UPROPERTY(BlueprintGetter = GetFirstPersonMesh, Category = "SKGShooterPawnComponent")
	TObjectPtr<USkeletalMeshComponent> MeshFP;
	UPROPERTY(BlueprintGetter = GetThirdPersonMesh, Category = "SKGShooterPawnComponent")
	TObjectPtr<USkeletalMeshComponent> MeshTP;
	UPROPERTY(BlueprintGetter = GetShooterFrameworkAnimInstance, Category = "SKGShooterPawnComponent")
	TObjectPtr<USKGShooterFrameworkAnimInstance> ShooterFrameworkAnimInstance;
	UPROPERTY(BlueprintGetter = GetOwningPawn, Category = "SKGShooterPawnComponent")
	TObjectPtr<APawn> OwningPawn;
	float CameraStartingFOV {-1.0f};

	UPROPERTY(BlueprintGetter = GetProceduralShooterPawnData, Category = "SKGShooterPawnComponent")
	FSKGProceduralShooterPawnData ProceduralShooterPawnData;

	UPROPERTY(ReplicatedUsing = OnRep_TargetLeanAngleCompressed)
	uint8 TargetLeanAngleCompressed {0};
	float TargetLeanAngle {0.0f};
	float TargetLeanLeftAngle {0.0f};
	float TargetLeanRightAngle {0.0f};
	bool bLeaningLeft {false};
	bool bLeaningRight {false};
	UFUNCTION()
	void OnRep_TargetLeanAngleCompressed();
	
	UPROPERTY(ReplicatedUsing = OnRep_HeldActor, BlueprintGetter = GetHeldActor, Category = "SKGShooterPawnComponent")
	TObjectPtr<AActor> HeldActor;
	UPROPERTY(ReplicatedUsing = OnRep_IsAiming, BlueprintGetter = IsAiming, Category = "SKGShooterPawnComponent")
	bool bIsAiming {false};
	UPROPERTY(ReplicatedUsing = OnRep_CurrentProceduralPoseData, BlueprintGetter = GetCurrentProceduralPoseReplicationData, Category = "SKGShooterPawnComponent")
	FSKGProceduralPoseReplicationData CurrentProceduralPoseData;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_OffhandIKIsLeftHand, BlueprintGetter = GetOffhandIKIsLeftHand, Category = "SKGShooterPawnComponent")
	bool bOffhandIKIsLeftHand {true};

	UPROPERTY(ReplicatedUsing = OnRep_InFreeLook)
	bool bInFreeLook {false};
	UFUNCTION()
	void OnRep_InFreeLook();
	FRotator FreeLookStartRotation {FRotator::ZeroRotator};
	bool bCanAddYaw {true};
	bool bCanAddPitch {true};
	
	UPROPERTY(Replicated)
	uint8 RemoteViewYaw {0};

	UPROPERTY()
	TObjectPtr<USKGFirearmComponent> CurrentFirearmComponent;
	UPROPERTY()
	TObjectPtr<USKGProceduralAnimComponent> CurrentProceduralAnimComponent;
	// If there is no firearm component on the held actor, we atleast have a pointer to a possible existing OffhandIKComponent to use
	UPROPERTY()
	TObjectPtr<USKGOffhandIKComponent> CurrentOffhandIKComponent;
	UPROPERTY()
	TObjectPtr<USKGProceduralAnimComponent> CharactersProceduralAnimComponent;

	FTransform CameraOffset;
	bool bUseFirstPersonProceduralsAsLocal {true};
	bool bUsingCustomSwayMultiplier {false};
	float SwayMultiplier {1.0f};
	ECollisionChannel FirearmCollisionChannel {ECC_Visibility};
	
	UFUNCTION()
	virtual void OnRep_HeldActor(AActor* OldActor);
	UFUNCTION()
	void OnRep_IsAiming();
	UFUNCTION()
	void OnRep_CurrentProceduralPoseData() const;
	UFUNCTION()
	void OnRep_OffhandIKIsLeftHand();

	FVector2D MouseInput {FVector2D::ZeroVector};
	
	void SetupComponents();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetAiming(bool bAim);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Lean(const uint8 TargetAngle);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PerformProceduralPose(const FGameplayTag& Tag, bool bExitPose);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetOffhandIKHand(bool bLeftHand);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetFreeLook(bool bFreeLook);
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_PerformCustomCurveUnreliable(const FSKGFirstAndThirdPersonCurveSettings& CurveData);
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetRemoteYaw(uint8 Yaw);
	
	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void Multi_PlayCustomCurveUnreliable(const FSKGFirstAndThirdPersonCurveSettings& CurveData);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PerformCustomCurve(const FSKGFirstAndThirdPersonCurveSettings& CurveData);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multi_PlayCustomCurve(const FSKGFirstAndThirdPersonCurveSettings& CurveData);

	void ReplicateYaw(bool bForce);
	
	// Called from only the anim instance so a delegate can be fired
	void PoseComplete() const;
	void OnFirearmRequestedCycleAimingPoint() const;
	void AnimLayerLinked();
	void FirearmCollisionChanged(const bool bIsColliding) const;
};
