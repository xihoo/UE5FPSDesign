// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SKGPDAOpticZeroSettings.generated.h"

UENUM(BlueprintType)
enum class ESKGOpticAdjustmentType : uint8
{
	MOA		UMETA(DisplayName = "MOA"),
	MRAD	UMETA(DisplayName = "MRAD")
};

UENUM(BlueprintType)
enum class ESKGMRADAdjustment : uint8
{
	PointOne	UMETA(DisplayName = ".1")
};

UENUM(BlueprintType)
enum class ESKGMOAAdjustment : uint8
{
	OneEighth	UMETA(DisplayName = "1/8"),
	OneQuarter	UMETA(DisplayName = "1/4"),
	OneHalf		UMETA(DisplayName = "1/2"),
	One			UMETA(DisplayName = "1")
};

USTRUCT(BlueprintType)
struct FSKGOpticStartingZeroSettings
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent")
	bool bStartWithDefaultZero {false};
	UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent", meta = (EditCondition = "bStartWithDefaultZero"))
	int32 DefaultElevationClicks {25};
	UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent", meta = (EditCondition = "bStartWithDefaultZero"))
	int32 DefaultWindageClicks {25};
	UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent", meta = (EditCondition = "!bStartWithDefaultZero"))
	bool bStartWithRandomZero {false};
	UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent", meta = (EditCondition = "bStartWithRandomZero && !bStartWithDefaultZero"))
	int32 RandomMaxElevationClicks {25};
	UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent", meta = (EditCondition = "bStartWithRandomZero && !bStartWithDefaultZero"))
	int32 RandomMaxWindageClicks {25};
};

USTRUCT(BlueprintType)
struct FSKGOpticZeroSettings
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent")
	ESKGOpticAdjustmentType AdjustmentType {ESKGOpticAdjustmentType::MRAD};
	UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent", meta = (EditCondition = "AdjustmentType == ESKGOpticAdjustmentType::MOA", EditConditionHides))
	ESKGMOAAdjustment MOAAdjustmentAmount = ESKGMOAAdjustment::OneQuarter;
	UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent", meta = (EditCondition = "AdjustmentType == ESKGOpticAdjustmentType::MRAD", EditConditionHides))
	ESKGMRADAdjustment MRADAdjustmentAmount = ESKGMRADAdjustment::PointOne;
	UPROPERTY(EditDefaultsOnly, Category = "SKGOpticComponent")
	FSKGOpticStartingZeroSettings StartingZeroSettings;
	
private:
	double MRADAdjustment {0.0572958};
	double MOAAdjustment {0.01666667};
	double AdjustmentAmount {0.0};
	
public:
	// Needs to be called by BeginPlay to initialize the given settings
	void Initialize()
	{
		if (AdjustmentType == ESKGOpticAdjustmentType::MRAD)
		{
			switch (MRADAdjustmentAmount)
			{
			case ESKGMRADAdjustment::PointOne :
				{
					AdjustmentAmount = MRADAdjustment * 0.1f;
					break;
				}
			}
		}
		else
		{
			switch (MOAAdjustmentAmount)
			{
			case ESKGMOAAdjustment::OneQuarter :
				{
					AdjustmentAmount = MOAAdjustment * 0.25;
					break;
				}
			case ESKGMOAAdjustment::OneHalf :
				{
					AdjustmentAmount = MOAAdjustment * 0.5;
					break;
				}
			case ESKGMOAAdjustment::One :
				{
					AdjustmentAmount = MOAAdjustment;
					break;
				}
			case ESKGMOAAdjustment::OneEighth :
				{
					AdjustmentAmount = MOAAdjustment * 0.125;
					break;
				}
			}
		}
	}

	double GetAdjustmentAmount() const
	{
		return AdjustmentAmount;
	}
};

UCLASS()
class SKGOPTIC_API USKGPDAOpticZeroSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "SKGOptic")
	FSKGOpticZeroSettings OpticZeroSettings;
};
