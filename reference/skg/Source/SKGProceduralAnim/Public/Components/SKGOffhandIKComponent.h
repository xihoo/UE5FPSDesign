// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "NativeGameplayTags.h"
#include "Components/ActorComponent.h"
#include "SKGOffhandIKComponent.generated.h"

class USKGPDAOffhandIKInitialize;
class USKGPDAOffhandIKSettings;
class UMeshComponent;
class UAnimSequence;

namespace SKGGAMEPLAYTAGS
{
	SKGPROCEDURALANIM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(OffhandIKComponentFirearm);
	SKGPROCEDURALANIM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(OffhandIKComponentHandguard);
	SKGPROCEDURALANIM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(OffhandIKComponentForwardGrip);
}

/**
 * Used for off hand ik. For example with a right handed shooter holding a rifle, the
 * offhand would be the left hand, for a left handed shooter the offhand would be the right hand
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKGPROCEDURALANIM_API USKGOffhandIKComponent : public UActorComponent, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USKGOffhandIKComponent();
	UFUNCTION(BlueprintPure, Category = "SKGShooterFrameworkStatics|Getters")
	static USKGOffhandIKComponent* GetOffhandIKComponent(const AActor* Actor);
	
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }
	void InitializeOffhandIKComponent();

	void InitializeComponentFromData();
	FORCEINLINE bool HasAuthority() const { return GetOwnerRole() == ROLE_Authority; }
	// Should only be used when manually setting the value for construction
	void SetOffhandIKMeshName(const FName& Name) { OffhandIKMeshName = Name; }
	// Should only be used when manually setting the value for construction
	void SetLeftHandIKSocketName(const FName& Name) { LeftHandIKSocketName = Name; }
	// Should only be used when manually setting the value for construction
	void SetRightHandIKSocketName(const FName& Name) { RightHandIKSocketName = Name; }
	
	UFUNCTION(BlueprintCallable, Category = "SKGOffhandIK")
	void UpdateOffhandIK(UPrimitiveComponent* ComponentRelativeTo, bool bLeftHand);
	UFUNCTION(BlueprintGetter)
	FORCEINLINE FTransform GetOffhandIKOffset() const { return OffhandIKOffset; }
	UFUNCTION(BlueprintGetter)
	FORCEINLINE UAnimSequence* GetOffhandIKPose() const { return OffhandIKPose; }
	UFUNCTION(BlueprintGetter)
	FORCEINLINE UMeshComponent* GetOffhandIKMesh() const { return OffhandIKMesh; }
	template< typename T >
	FORCEINLINE T* GetOffhandIKMesh() const { return Cast<T>(OffhandIKMesh); }

	// if true, the component will auto initialize itself upon creation
	UPROPERTY(EditDefaultsOnly, Category = "SKGOffhandIK|Initialize")
	bool bAutoInitialize {true};
	// if true, this assumes you will set the ProceduralMeshName in code before initialization
	UPROPERTY(EditDefaultsOnly, Category = "SKGOffhandIK|Initialize")
	bool bOverrideComponentNames {false};
	UPROPERTY(EditDefaultsOnly, Category = "SKGOffhandIK|Initialize")
	TObjectPtr<USKGPDAOffhandIKInitialize> InitializationSettingsDataAsset;
	
	UPROPERTY(EditDefaultsOnly, Category = "SKGOffhandIK|Settings")
	TObjectPtr<USKGPDAOffhandIKSettings> OffhandIKSettingsDataAsset;
	
	// The mesh used to gather data for the Left Hand IK (think forward grip/handguard)
	FName OffhandIKMeshName {"StaticMesh"};
	// Socket used for the Left Hand IK transform (S_OffhandIK in example project)
	FName LeftHandIKSocketName {"S_LeftHandIK"};
	// Socket used for the Right Hand IK transform (S_OffhandIK in example project)
	FName RightHandIKSocketName {"S_RightHandIK"};
	
	// Animation to be played on the left hand for the left hand IK
	UPROPERTY()
	TObjectPtr<UAnimSequence> LeftHandIKPose;
	// Animation to be played on the Right hand for the Right hand IK
	UPROPERTY()
	TObjectPtr<UAnimSequence> RightHandIKPose;
	/**
	 * The tag to be used to determine which offhand ik component should be used. If complete firearm (no forward
	 * grip) = OffhandIKComponentType.Firearm, Handguard = OffhandIKComponentType.Handguard, ForwardGrip = OffhandIKComponentType.ForwardGrip
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGOffhandIK|Initialize")
	FGameplayTagContainer GameplayTags;

protected:
	UPROPERTY(BlueprintGetter = GetOffhandIKMesh, Category = "SKGOffhandIK|Mesh")
	TObjectPtr<UMeshComponent> OffhandIKMesh;
	
	void SetupComponents();
	FTransform GetOffhandIKWorldTransform(bool bLeftHand) const;

	UPROPERTY(BlueprintGetter = GetOffhandIKOffset, Category = "SKGOffhandIK")
	FTransform OffhandIKOffset;
	UPROPERTY(BlueprintGetter = GetOffhandIKPose, Category = "SKGOffhandIK")
	TObjectPtr<UAnimSequence> OffhandIKPose;

	FORCEINLINE const FName& GetHandSocketName(bool bLeftHand) const;
};
