// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SKGPDAFirearmCollisionSettings.generated.h"

class UCurveVector;

USTRUCT(BlueprintType)
struct FSKGFirearmCollisionSettings
{
	GENERATED_BODY()
	// Whether or not to use firearm collision
	UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent")
	bool bUseFirearmCollision {true};
	// The diameter to be used for the firearm collision trace (runs down the barrel)
	UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent", meta = (EditCondition = "bUseFirearmCollision"))
	float TraceDiameter {2.0f};
	// How far you can go into firearm collision before it stops/prevents aiming
	UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent", meta = (EditCondition = "bUseFirearmCollision"))
	float StopAimingDistance {0.2f};
	// The curve in which the firearm moves to as it moves through firearm collision
	UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent", meta = (EditCondition = "bUseFirearmCollision"))
	TObjectPtr<UCurveVector> PoseLocationCurve;
	// The curve in which the firearm rotates to as it moves through firearm collision
	UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent", meta = (EditCondition = "bUseFirearmCollision"))
	TObjectPtr<UCurveVector> PoseRotationCurve;
	// Higher value = further iteration through the curve for each unit of distance pushed from collision
	UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent", meta = (EditCondition = "bUseFirearmCollision"))
	float PoseScale {2.0f};
	// The speed in which the firearm collision interpolates to a new value in the curve
	UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent", meta = (EditCondition = "bUseFirearmCollision"))
	float PoseLocationInterpSpeed {10.0f};
	// The speed in which the firearm collision interpolates to a new value in the curve
	UPROPERTY(EditDefaultsOnly, Category = "SKGFirearmComponent", meta = (EditCondition = "bUseFirearmCollision"))
	float PoseRotationInterpSpeed {10.0f};
};

UCLASS()
class SKGSHOOTERFRAMEWORK_API USKGPDAFirearmCollisionSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGProcedural")
	FSKGFirearmCollisionSettings FirearmCollisionSettings;
};
