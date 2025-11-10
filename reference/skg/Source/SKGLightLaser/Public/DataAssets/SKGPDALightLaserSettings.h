// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Engine/DataAsset.h"
#include "SKGPDALightLaserSettings.generated.h"

class UMaterialInterface;

UENUM(BlueprintType)
enum class ESKGLaserMode : uint8
{
	Off		UMETA(DisplayName = "Off"),
	On		UMETA(DisplayName = "On")
};

UENUM(BlueprintType)
enum class ESKGLightMode : uint8
{
	Off		UMETA(DisplayName = "Off"),
	On		UMETA(DisplayName = "On"),
	Strobe	UMETA(DisplayName = "Strobe")
};

USTRUCT(BlueprintType)
struct FSKGLaserMaterial
{
	GENERATED_BODY()
	// Material to be used for the laser mesh
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser")
	TObjectPtr<UMaterialInterface> Laser;
	// Material to be used for the laser dot
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser")
	TObjectPtr<UMaterialInterface> LaserDot;

	FSKGLaserMaterial() {};
	FSKGLaserMaterial(UMaterialInterface* LaserMaterial, UMaterialInterface* DecalMaterial)
	{
		Laser = LaserMaterial;
		LaserDot = DecalMaterial;
	}
	
	operator bool() const
	{
		return Laser && LaserDot;
	}

	bool operator ==(const FSKGLaserMaterial& RHS) const
	{
		return Laser == RHS.Laser && LaserDot == RHS.LaserDot;
	}

	bool operator !=(const FSKGLaserMaterial& RHS) const
	{
		return Laser != RHS.Laser || LaserDot != RHS.LaserDot;
	}
};

USTRUCT(BlueprintType)
struct FSKGLaserState
{
	GENERATED_BODY()
	UPROPERTY()
	ESKGLaserMode LaserMode {ESKGLaserMode::Off};

	bool bCanScaleLaser {false};
	FSKGLaserMaterial CurrentLaserMaterial;
};

USTRUCT(BlueprintType)
struct FSKGLaserSettings
{
	GENERATED_BODY()
	ECollisionChannel LaserCollisionChannel {ECC_Visibility};
	// Max distance the laser will go
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser")
	float MaxLaserDistance {20000.0f};
	// Based on length of your laser mesh, the provided example is 2000 units long so the scale factor is 20
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser")
	float LaserScaleFactor {20.0f};

	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser")
	FSKGLaserMaterial DefaultLaserMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser")
	FSKGLaserMaterial InfraredLaserMaterial;

public:
	FSKGLaserMaterial GetMaterial(bool bInfrared) const
	{
		return bInfrared && InfraredLaserMaterial ? InfraredLaserMaterial : DefaultLaserMaterial;
	}

	void SetupDefaultMaterial(const FSKGLaserMaterial& LaserMaterial)
	{
		DefaultLaserMaterial.Laser = LaserMaterial.Laser;
		DefaultLaserMaterial.LaserDot = LaserMaterial.LaserDot;
	}
};

USTRUCT(BlueprintType)
struct FSKGLightState
{
	GENERATED_BODY()
	UPROPERTY()
	ESKGLightMode LightMode {ESKGLightMode::Off};
	UPROPERTY()
	int8 LightIntensityIndex {0};

	float OnIntensity {0.0f};
	bool bCanStrobe {false};
};

USTRUCT(BlueprintType)
struct FSKGLightSettings
{
	GENERATED_BODY()
	// Time between the light turning on/off for the strobing
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser")
	double LightStrobeInterval {0.08f};

protected:
	// Light brightness settings
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser")
	TArray<float> LightIntensities {50.0f};
	// Infrared (night vision) brightness settings
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser")
	TArray<float> LightIntensitiesInfrared {10.0f};

public:
	float GetLightIntensity(bool bInfrared, int32 Index) const
	{
		const TArray<float>* Intensities = bInfrared ? &LightIntensitiesInfrared : &LightIntensities;
		Index = FMath::Clamp(Index, 0, Intensities->Num());
		return (*Intensities)[Index];
	}

	bool IsValidIndex(bool bInfrared, int32 Index) const
	{
		return (bInfrared ? &LightIntensitiesInfrared : &LightIntensities)->IsValidIndex(Index);
	}

	int32 GetNextLightIntensityIndex(bool bInfrared, int32 Index) const
	{
		const int32 ArrayLength = (bInfrared ? LightIntensitiesInfrared : LightIntensities).Num();
		return ++Index >= ArrayLength ? 0 : Index;
	}
};

USTRUCT(BlueprintType)
struct FSKGLightLaserCycleMode
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser")
	ESKGLaserMode LaserMode {ESKGLaserMode::Off};
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser")
	ESKGLightMode LightMode {ESKGLightMode::Off};
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser")
	bool bInfrared {false};

	bool operator == (const FSKGLightLaserCycleMode& RHS) const
	{
		return RHS.LaserMode == LaserMode && RHS.LightMode == LightMode && RHS.bInfrared == bInfrared;
	}
};

USTRUCT(BlueprintType)
struct FSKGLightLaserCycleModes
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser")
	TArray<FSKGLightLaserCycleMode> LightLaserModes;

private:
	int32 Index {0};

public:
	int32 GetIndex() const { return Index; }
	
	const FSKGLightLaserCycleMode& GetNextLightLaserMode()
	{
		if (++Index >= LightLaserModes.Num())
		{
			Index = 0;
		}
		return LightLaserModes[Index];
	}

	bool GetLightLaserModeAtIndex(const int32 NewIndex, FSKGLightLaserCycleMode& NewLightLaserMode)
	{
		if (NewIndex < LightLaserModes.Num())
		{
			Index = NewIndex;
			NewLightLaserMode = LightLaserModes[NewIndex];
			return true;
		}
		return false;
	}
};

UCLASS()
class SKGLIGHTLASER_API USKGPDALightLaserSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser")
	FSKGLaserSettings LaserSettings;
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser")
	FSKGLightSettings LightSettings;
	UPROPERTY(EditDefaultsOnly, Category = "SKGLightLaser")
	FSKGLightLaserCycleModes LightLaserCycleModes;
};
