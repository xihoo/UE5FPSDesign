// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Engine/DataAsset.h"
#include "SKGPDAMuzzleSettings.generated.h"

namespace SKGGAMEPLAYTAGS
{
	SKGMUZZLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MuzzleComponentBarrel);
	SKGMUZZLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MuzzleComponentMuzzleDevice);
	SKGMUZZLE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(MuzzleComponentSuppressor);
}

UCLASS()
class SKGMUZZLE_API USKGPDAMuzzleSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	/**
	 * The tag for the muzzle. Firearms with barrels = MuzzleComponentType.Barrel, Barrels = MuzzleComponentType.Barrel, Muzzle Devices
	 * like muzzle brakes = MuzzleComponentType.MuzzleDevice, Suppressors ontop of muzzle devices = MuzzleComponentType.Suppressor
	 */
	UPROPERTY(EditDefaultsOnly, Category = "SKGMuzzle")
	FGameplayTag MuzzleTag {SKGGAMEPLAYTAGS::MuzzleComponentBarrel};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMuzzle")
	FGameplayTagContainer GameplayTags;
	// If true, the temperature system will be used (temperature accumulates with each shot and slowly cools)
	UPROPERTY(EditDefaultsOnly, Category = "SKGMuzzle|MuzzleTemperature")
	bool bUseMuzzleTemperatureSystem {false};
	// The max temperature the muzzle can get
	UPROPERTY(EditDefaultsOnly, Category = "SKGMuzzle|MuzzleTemperature", meta = (EditCondition = "bUseMuzzleTemperatureSystem"))
	float MaxMuzzleTemperatureFahrenheit {1200.0f};
	// The temperature at which the muzzle with start to glow in the material
	UPROPERTY(EditDefaultsOnly, Category = "SKGMuzzle|MuzzleTemperature", meta = (EditCondition = "bUseMuzzleTemperatureSystem"))
	float MuzzleStartGlowTemperature {600.0f};
	// The amount the muzzle temperature increases per shot
	UPROPERTY(EditDefaultsOnly, Category = "SKGMuzzle|MuzzleTemperature", meta = (EditCondition = "bUseMuzzleTemperatureSystem"))
	float IncreaseMuzzleTemperatureAmountFahrenheit {10.0f};
	// The amount the muzzle temperature decreases per tick
	UPROPERTY(EditDefaultsOnly, Category = "SKGMuzzle|MuzzleTemperature", meta = (EditCondition = "bUseMuzzleTemperatureSystem"))
	float DecreaseMuzzleTemperatureAmountPerTick {20.0f};
};
