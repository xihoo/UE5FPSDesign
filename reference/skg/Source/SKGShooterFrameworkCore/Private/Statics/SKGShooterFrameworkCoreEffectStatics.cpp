// Copyright 2023, Dakota Dawe, All rights reserved


#include "Statics/SKGShooterFrameworkCoreEffectStatics.h"

#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

constexpr float SpeedOfSound = 343.0f;

float USKGShooterFrameworkCoreEffectStatics::GetSpeedOfSoundDelay(const FVector& LocationA, const FVector& LocationB)
{
	const float Distance = FVector::Distance(LocationA, LocationB) * 0.01f;
	return Distance / SpeedOfSound;
}

void USKGShooterFrameworkCoreEffectStatics::PlaySoundEffectSpeedOfSound(const UWorld* World, const float Delay,
	const FVector Location, USoundBase* Sound, const float VolumeMultiplier, float PitchMultiplier)
{
	if (World && Sound)
	{
		FTimerHandle FSoundDelay;
		const FTimerDelegate SoundDelayDelegate = FTimerDelegate::CreateStatic(&USKGShooterFrameworkCoreEffectStatics::PlaySoundEffectSpeedOfSound_Implementation, World, Location, Sound, VolumeMultiplier, PitchMultiplier);
		World->GetTimerManager().SetTimer(FSoundDelay, SoundDelayDelegate, Delay, false);
	}
}

void USKGShooterFrameworkCoreEffectStatics::PlaySoundEffectSpeedOfSound_Implementation(const UWorld* World, const FVector Location, USoundBase* Sound, const float VolumeMultiplier, float PitchMultiplier)
{
	UGameplayStatics::SpawnSoundAtLocation(World, Sound, Location, FRotator::ZeroRotator, VolumeMultiplier, PitchMultiplier, 0.0f, Sound->AttenuationSettings);
}

void USKGShooterFrameworkCoreEffectStatics::PlaySoundEffect(const UObject* WorldContextObject, const FVector& SpawnLocation, USoundBase* Sound, const float VolumeMultiplier, float PitchMultiplier, const bool bUseSpeedOfSound, const float SpeedOfSoundTolerance)
{
	if (WorldContextObject && WorldContextObject->GetWorld())
	{
		const UWorld* World = WorldContextObject->GetWorld();
		float SoundDelay = 0.0f;
		if (bUseSpeedOfSound && UGameplayStatics::GetPlayerPawn(World, 0))
		{
			SoundDelay = GetSpeedOfSoundDelay(SpawnLocation, UGameplayStatics::GetPlayerPawn(World, 0)->GetActorLocation());
		}
		if (!bUseSpeedOfSound || SoundDelay < SpeedOfSoundTolerance)
		{
			UGameplayStatics::SpawnSoundAtLocation(World, Sound, SpawnLocation, FRotator::ZeroRotator, VolumeMultiplier, PitchMultiplier, 0.0f, Sound->AttenuationSettings);
		}
		else
		{
			PlaySoundEffectSpeedOfSound(World, SoundDelay, SpawnLocation, Sound, VolumeMultiplier, PitchMultiplier);
		}
	}
}