// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SKGShooterFrameworkCoreEffectStatics.generated.h"

class USoundBase;

UCLASS()
class SKGSHOOTERFRAMEWORKCORE_API USKGShooterFrameworkCoreEffectStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
private:
	static void PlaySoundEffectSpeedOfSound_Implementation(const UWorld* World, const FVector Location, USoundBase* Sound, const float VolumeMultiplier, float PitchMultiplier);
public:
	static float GetSpeedOfSoundDelay(const FVector& LocationA, const FVector& LocationB);
	static void PlaySoundEffectSpeedOfSound(const UWorld* World, const float Delay, const FVector Location, USoundBase* Sound, const float VolumeMultiplier, float PitchMultiplier);

	UFUNCTION(BlueprintCallable, Category = "SKGShooterFramework|Core|Sounds", meta = (WorldContext = "WorldContextObject"))
	static void PlaySoundEffect(const UObject* WorldContextObject, const FVector& SpawnLocation, USoundBase* Sound, const float VolumeMultiplier = 1.0f, float PitchMultiplier = 1.0f, const bool bUseSpeedOfSound = false, const float SpeedOfSoundTolerance = 0.2f);
};
