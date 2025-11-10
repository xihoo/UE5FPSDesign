// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "Engine/NetSerialization.h"
#include "DataAssets/SKGPDAMuzzleSettings.h"
#include "Components/ActorComponent.h"
#include "SKGMuzzleComponent.generated.h"

class USKGPDAMuzzleInitialize;
class USKGMuzzleWorldSubsystem;
class UMeshComponent;

// Data type used for replication as it's much smaller than sending a RPC with a FTransform
USTRUCT(BlueprintType)
struct FSKGMuzzleTransform
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SKGMuzzle")
	FVector_NetQuantize Location = FVector_NetQuantize::ZeroVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SKGMuzzle")
	FVector_NetQuantizeNormal Direction = FVector_NetQuantizeNormal::ZeroVector;

	FSKGMuzzleTransform() {}
	FSKGMuzzleTransform(const FVector& INLocation, const FRotator& INRotation)
	{
		Location = FVector_NetQuantize10(INLocation);
		Direction = INRotation.Vector();
	}
	FSKGMuzzleTransform(const FTransform& INTransform)
	{
		Location = INTransform.GetLocation();
		Direction = INTransform.GetRotation().GetForwardVector();
	}
	
	FTransform ConvertToTransform() const
	{
		return FTransform(Direction.ToOrientationRotator(), Location, FVector::OneVector);
	}

	bool operator == (const FSKGMuzzleTransform& RHS) const
	{
		return Location == RHS.Location && Direction == RHS.Direction;
	}

	bool operator != (const FSKGMuzzleTransform& RHS) const
	{
		return Location != RHS.Location || Direction != RHS.Direction;
	}

	operator FTransform() const { return ConvertToTransform(); }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMuzzleTemperatureChanged, float, Temperature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMuzzleCooled);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKGMUZZLE_API USKGMuzzleComponent : public UActorComponent, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	friend USKGMuzzleWorldSubsystem;
public:
	// Sets default values for this component's properties
	USKGMuzzleComponent();
	UFUNCTION(BlueprintPure, Category = "SKGShooterFrameworkStatics|Getters")
	static USKGMuzzleComponent* GetMuzzleComponent(const AActor* Actor);
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }
	void InitializeMuzzleComponent();

	void InitializeComponentFromData();
	FORCEINLINE bool HasAuthority() const { return GetOwnerRole() == ROLE_Authority; }
	// Should only be used when manually setting the value for construction
	void SetMuzzleMeshComponentName(const FName& Name) { MuzzleMeshComponentName = Name; }
	// Should only be used when manually setting the value for construction
	void SetMuzzleSocketName(const FName& Name) { MuzzleSocketName = Name; }
	// Should only be used when manually setting the value for construction
	void SetMuzzleTag(const FGameplayTag& Tag) { MuzzleTag = Tag; }
	
	UFUNCTION(BlueprintPure, Category = "SKGMuzzle|Tag")
	FGameplayTag GetMuzzleTag() const { return MuzzleTag; }
	
	// To be called when you fire your firearm to work with muzzle temperature
	UFUNCTION(BlueprintCallable, Category = "SKGMuzzle|MuzzleTemperature")
	void ShotPerformed();
	UFUNCTION(BlueprintPure, Category = "SKGMuzzle|MuzzleTemperature")
	float GetMuzzleTemperature() const { return CurrentMuzzleTemperature; }
	UFUNCTION(BlueprintPure, Category = "SKGMuzzle|MuzzleTemperature")
	float GetMuzzleTemperatureNormalized() const;
	UFUNCTION(BlueprintPure, Category = "SKGMuzzle|MuzzleTemperature")
	float GetMuzzleTemperatureNormalizedWithMinimumGlowTemperature() const;
	UFUNCTION(BlueprintPure, Category = "SKGMuzzle|MuzzleTemperature")
	bool IsUsingMuzzleTemperatureSystem() const { return bUseMuzzleTemperatureSystem; }
	
	// Returns the transform of the muzzle socket
	UFUNCTION(BlueprintPure, Category = "SKGMuzzle|Transform")
	FTransform GetMuzzleTransform() const;
	/**
	 * Useful as a way to get a transform for launching projectiles with an estimated zero distance
	 * @param ZeroDistanceMeters Range to zero to (eg 100 meters)
	 * @param MOA Accuracy (1 = 1 inch of spread at 100 yards/91.44 meters
	 * @param AimTransform Transform of the sight socket (current aiming point)
	 */
	UFUNCTION(BlueprintPure, Category = "SKGMuzzle|Transform")
	FSKGMuzzleTransform GetMuzzleProjectileTransformCompensated(float ZeroDistanceMeters, float MOA, const FTransform& AimTransform) const;
	/**
	 * Useful as a way to get a transform for launching projectiles straight from the muzzle (no compensation)
	 * @param MOA Accuracy (1 = 1 inch of spread at 100 yards/91.44 meters
	 */
	UFUNCTION(BlueprintPure, Category = "SKGMuzzle|Transform")
	FSKGMuzzleTransform GetMuzzleProjectileTransform(float MOA) const;
	
	// Used for procedural system with firearm collision
	FTransform GetMuzzleTransformRelative(const UPrimitiveComponent* ComponentRelativeTo) const;

	// This fires client side each time the muzzle temperature changes
	UPROPERTY(BlueprintAssignable, Category = "SKGMuzzle|Events")
	FOnMuzzleTemperatureChanged OnMuzzleTemperatureChanged;
	// This fires client side whenever the muzzle is completely cooled.
	UPROPERTY(BlueprintAssignable, Category = "SKGMuzzle|Events")
	FOnMuzzleCooled OnMuzzleCooled;
	
	// if true, the component will auto initialize itself upon creation
	UPROPERTY(EditDefaultsOnly, Category = "SKGMuzzle|Initialize")
	bool bAutoInitialize {true};
	// if true, this assumes you will set the MuzzleMeshComponentName in code before initialization
	UPROPERTY(EditDefaultsOnly, Category = "SKGMuzzle|Initialize")
	bool bOverrideComponentNames {false};
	UPROPERTY(EditDefaultsOnly, Category = "SKGMuzzle|Initialize")
	TObjectPtr<USKGPDAMuzzleInitialize> InitializationSettingsDataAsset;
	UPROPERTY(EditDefaultsOnly, Category = "SKGMuzzle|Settings")
	TObjectPtr<USKGPDAMuzzleSettings> MuzzleSettingsDataAsset;
	
	// The name of the mesh to be used for the muzzle (to gather the muzzle transform from)
	FName MuzzleMeshComponentName {"StaticMesh"};
	// The name of the socket on the MuzzleMeshComponentName mesh to be used to gather the muzzle transform from
	FName MuzzleSocketName {"S_Muzzle"};

protected:
	/**
	 * The tag for the muzzle. Firearms with barrels = MuzzleComponentType.Barrel, Barrels = MuzzleComponentType.Barrel, Muzzle Devices
	 * like muzzle brakes = MuzzleComponentType.MuzzleDevice, Suppressors ontop of muzzle devices = MuzzleComponentType.Suppressor
	 */
	FGameplayTag MuzzleTag {SKGGAMEPLAYTAGS::MuzzleComponentBarrel};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMuzzle|Settings|Initialize")
	FGameplayTagContainer GameplayTags;
	// If true, the temperature system will be used (temperature accumulates with each shot and slowly cools)
	bool bUseMuzzleTemperatureSystem {false};
	// The max temperature the muzzle can get
	float MaxMuzzleTemperatureFahrenheit {1200.0f};
	// The normalize rate (used in the normalize calculation) to get a normalized temperature between 0 and 1 (changes scaling)
	float MuzzleStartGlowTemperature {600.0f};
	// The amount the muzzle temperature increases per shot
	float IncreaseMuzzleTemperatureAmountFahrenheit {10.0f};
	// The amount the muzzle temperature decreases per tick
	float DecreaseMuzzleTemperatureAmountPerTick {20.0f};

	float CurrentMuzzleTemperature {0.0f};
	bool bRegisteredToSubsystem {false};
	
	UPROPERTY()
	TObjectPtr<UMeshComponent> MuzzleMesh;
	
	void SetupComponents();
	void CoolMuzzle(float DeltaTime);
};
