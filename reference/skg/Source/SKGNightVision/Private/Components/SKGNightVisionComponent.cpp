// Copyright 2025, Dakota Dawe, All rights reserved


#include "Components/SKGNightVisionComponent.h"
#include "DataAssets/SKGPDANightVisionSettings.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MaterialTypes.h"
#include "Kismet/KismetMaterialLibrary.h"


USKGNightVisionComponent::USKGNightVisionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void USKGNightVisionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

float USKGNightVisionComponent::GetGainValue() const
{
	FMaterialParameterInfo GainParameterInfo(GainParameterName);
	float Gain;
	NightVisionMaterialPair.NightVisionMaterialInstance->GetScalarParameterValue(GainParameterInfo, Gain);
	return Gain;
}

void USKGNightVisionComponent::SetGainValues(const float Gain) const
{
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(GainParameterName, Gain);
	NightVisionMaterialPair.NightVisionScintillationMaterialInstance->SetScalarParameterValue(GainParameterName, Gain);
}

FNightVisionMaterialPair& USKGNightVisionComponent::CreateAndSetupDynamicNightVisionMaterialInstance(USKGPDANightVisionTubeSettings* Tube, USKGPDANightVisionHousingSettings* Housing)
{
	TubeSettings = Tube;
	HousingSettings = Housing;
	
	NightVisionMaterialPair.NightVisionMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), TubeSettings->NightVisionMaterial);
	NightVisionMaterialPair.NightVisionScintillationMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), TubeSettings->NightVisionScintillationMaterial);

	CurrentGainLerpValue = TubeSettings->Gain / TubeSettings->MaxGain;
	
	NightVisionMaterialPair.NightVisionMaterialInstance->SetVectorParameterValue(NightVisionColorParameterName, TubeSettings->Color);
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(GainParameterName, TubeSettings->Gain);
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(NormalizedGainParameterName, CurrentGainLerpValue);
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(ContrastParameterName, TubeSettings->Contrast);
	
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(NoiseFactorParameterName, TubeSettings->NoiseFactor);
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(GrainMultiplierParameterName, TubeSettings->GrainMultiplier);
	
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(DistortionRadiusParameterName, HousingSettings->DistortionRadius);
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(DistortionHardnessParameterName, HousingSettings->DistortionHardness);
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(UseRadialDistortionParameterName, HousingSettings->bUseRadialDistortion);

	NightVisionMaterialPair.NightVisionScintillationMaterialInstance->SetScalarParameterValue(GainParameterName, TubeSettings->Gain);
	NightVisionMaterialPair.NightVisionScintillationMaterialInstance->SetScalarParameterValue(NormalizedGainParameterName, CurrentGainLerpValue);
	return NightVisionMaterialPair;
}

void USKGNightVisionComponent::SetGain(const float Gain)
{
	if (HousingSettings->bManualGain)
	{
		TargetGainLerpValue = FMath::Clamp(Gain, MinimumGainValue, 1.0f);
		if (HousingSettings->bSmoothAdjustGain)
		{
			PreviousSmoothGainAdjustTimestamp = GetWorld()->GetTimeSeconds();
			GetWorld()->GetTimerManager().SetTimer(TSmoothGainAdjustHandle, [this]()
			{
				CurrentGainLerpValue = FMath::FInterpConstantTo(CurrentGainLerpValue, TargetGainLerpValue, GetWorld()->GetTimeSeconds() - PreviousSmoothGainAdjustTimestamp, HousingSettings->SmoothAdjustSpeed);
				PreviousSmoothGainAdjustTimestamp = GetWorld()->GetTimeSeconds();
				const float NewGain = FMath::Lerp(0.0f, TubeSettings->MaxGain, CurrentGainLerpValue);
				if (!FMath::IsNearlyEqual(NewGain, GetGainValue()))
				{
					NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(GainParameterName, NewGain);
					NightVisionMaterialPair.NightVisionScintillationMaterialInstance->SetScalarParameterValue(GainParameterName, NewGain);
				}
				else
				{
					GetWorld()->GetTimerManager().ClearTimer(TSmoothGainAdjustHandle);
				}
			}, 1.0f / 60.0f, true);
		}
		else
		{
			CurrentGainLerpValue = TargetGainLerpValue;
			const float NewGain = FMath::Lerp(0.0f, TubeSettings->MaxGain, CurrentGainLerpValue);
			if (!FMath::IsNearlyEqual(NewGain, GetGainValue()))
			{
				NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(GainParameterName, NewGain);
				NightVisionMaterialPair.NightVisionScintillationMaterialInstance->SetScalarParameterValue(GainParameterName, NewGain);
			}
		}
	}
}

void USKGNightVisionComponent::IncreaseGain()
{
	SetGain(TargetGainLerpValue + HousingSettings->GainAdjustmentStepAmount);
}

void USKGNightVisionComponent::DecreaseGain()
{
	SetGain(TargetGainLerpValue - HousingSettings->GainAdjustmentStepAmount);
}
