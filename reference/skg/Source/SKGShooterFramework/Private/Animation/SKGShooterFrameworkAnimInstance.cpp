// Copyright 2023, Dakota Dawe, All rights reserved

#include "Animation/SKGShooterFrameworkAnimInstance.h"
#include "Components/SKGShooterPawnComponent.h"

#include "KismetTraceUtils.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Pawn.h"
#include "Curves/CurveVector.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Runtime/Launch/Resources/Version.h"

DECLARE_CYCLE_STAT(TEXT("NativeUpdateAnimation"), STAT_SKGNativeUpdate, STATGROUP_SKGShooterFrameworkAnimInstance);
DECLARE_CYCLE_STAT(TEXT("SetupInitialData"), STAT_SKGSetupInitialData, STATGROUP_SKGShooterFrameworkAnimInstance);
DECLARE_CYCLE_STAT(TEXT("SetFirearmCollisionData"), STAT_SKGSetFirearmCollisionData, STATGROUP_SKGShooterFrameworkAnimInstance);
DECLARE_CYCLE_STAT(TEXT("OnFirearmTraceComplete"), STAT_SKGOnFirearmTraceComplete, STATGROUP_SKGShooterFrameworkAnimInstance);
DECLARE_CYCLE_STAT(TEXT("NativeThreadSafeUpdateAnimation"), STAT_SKGNativeThreadSafeUpdate, STATGROUP_SKGShooterFrameworkAnimInstance);

void USKGShooterFrameworkAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	SetupShooterPawnComponent();
	if (bUsePelvisForVelocity)
	{
		PelvisPosition = GetSkelMeshComponent()->GetSocketLocation(PelvisName);
	}
}

void USKGShooterFrameworkAnimInstance::SetupShooterPawnComponent()
{
	ShooterPawnComponent = USKGShooterPawnComponent::GetShooterPawnComponent(GetOwningActor());
	if (ensureAlwaysMsgf(ShooterPawnComponent, TEXT("IF YOU SEE THIS MESSAGE, ENSURE YOU HAVE THE SHOOTER PAWN COMPONENT ON THE ACTOR YOU ARE USING THIS ANIM INSTANCE ON")))
	{
		ShooterPawnComponent->OnHeldActorSet.AddUniqueDynamic(this, &USKGShooterFrameworkAnimInstance::OnHeldActorSet);
		if (ShooterPawnComponent->GetHeldActor())
		{
			OnHeldActorSet(ShooterPawnComponent->GetHeldActor(), nullptr);
		}
	}
	
	FirearmTraceDelegate.BindUObject(this, &USKGShooterFrameworkAnimInstance::OnFirearmTraceComplete);
}

void USKGShooterFrameworkAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	SCOPE_CYCLE_COUNTER(STAT_SKGNativeUpdate);
	SCOPED_NAMED_EVENT(SKGNativeUpdateAnimation, FColor::Blue);
	
	if (!ShooterPawnComponent)
	{
		return;
	}
	
	ProceduralAnimData = ShooterPawnComponent->GetProceduralData();
	FreeLookRecoilRotation = ProceduralAnimData.FreeLookStartRotation;
	bOffhandIKIsLeftHand = ProceduralAnimData.bOffhandIKIsLeftHand;
	
	SetupMandatoryData();
	SetupInitialData();
	
	if (ProceduralAnimData)
	{
		bUseFirstPerson = bIsLocallyControlled && ShooterPawnComponent->IsUsingFirstPersonProceduralsAsLocal();
		LengthOfPull = ProceduralAnimData.LengthOfPull;
		
		SetMovementSwayData(DeltaSeconds);
		SetRecoilData();
		SetPoseData(DeltaSeconds);
		SetFirearmCollisionData();
	}
	SetAimingData();
	SetCustomCurveData(DeltaSeconds);
	ShooterPawnComponent->AnimInstanceTicked(DeltaSeconds);
}

void USKGShooterFrameworkAnimInstance::SetupMandatoryData()
{
	bIsLocallyControlled = ShooterPawnComponent->IsLocallyControlled();
	IsLocallyControlledAlpha = static_cast<float>(bIsLocallyControlled);
	ControlRotation = bIsLocallyControlled ? ShooterPawnComponent->GetOwningPawn()->GetControlRotation() : ShooterPawnComponent->GetOwningPawn()->GetBaseAimRotation();
	TargetSpineLeanLeftRightAngle = ShooterPawnComponent->GetTargetLeanAngle();
	bIsTryingToAim = ShooterPawnComponent->IsAiming();
}

void USKGShooterFrameworkAnimInstance::SetupInitialData()
{
	SCOPE_CYCLE_COUNTER(STAT_SKGSetupInitialData);
	if (bIsLocallyControlled && ShooterPawnComponent->NeedsToContinuallyUpdateCameraOffset())
	{
		ShooterPawnComponent->SetCameraOffset();
	}
	SetCameraOffset(ShooterPawnComponent->GetCameraOffset());
	OffhandIKLocation = ProceduralAnimData.OffhandIKOffset.GetTranslation();
	OffhandIKRotation = ProceduralAnimData.OffhandIKOffset.Rotator();
	OffhandIKPose = ProceduralAnimData.OffhandIKPose;
}

void USKGShooterFrameworkAnimInstance::SetCameraOffset(const FTransform& Offset)
{
	CameraOffsetLocation = Offset.GetTranslation();
	CameraOffsetRotation = Offset.Rotator();
}

void USKGShooterFrameworkAnimInstance::SetAimingData()
{
	if (bIsAiming != ShooterPawnComponent->IsAiming())
	{
		if (!ProceduralAnimData.FirearmCollisionSettings.bUseFirearmCollision || FirearmCollisionPushAmount < ProceduralAnimData.FirearmCollisionSettings.CollisionStopAimingDistance)
		{
			bIsAiming = bIsTryingToAim;
			TargetAimAlpha = bIsAiming ? 1.0f : 0.0f;
			bInterpAimAlpha = true;
		}
	}
}

void USKGShooterFrameworkAnimInstance::SetMovementSwayData(float DeltaSeconds)
{
	if (ProceduralAnimData.MovementSwaySettings.bUseMovementSway)
	{
		const UCurveVector* LocationCurve = ProceduralAnimData.MovementSwaySettings.LocationSettings.Curve;
		const UCurveVector* RotationCurve = ProceduralAnimData.MovementSwaySettings.RotationSettings.Curve;
		
		MovementSwayVelocityMultiplier = UKismetMathLibrary::NormalizeToRange(PawnHorizontalVelocity, 0.0f, 250.0f);
        MovementSwayVelocityMultiplier = FMath::Clamp(MovementSwayVelocityMultiplier, 0.25f, 1.0f);
    
        MovementSwayAccumulator += DeltaSeconds * MovementSwayVelocityMultiplier;
		if (LocationCurve)
		{
			MovementSwayCurveLocation = LocationCurve->GetVectorValue(MovementSwayAccumulator);
		}
		if (RotationCurve)
		{
			MovementSwayCurveRotation = RotationCurve->GetVectorValue(MovementSwayAccumulator);
		}
	}
	else
	{
		MovementSwayCurveLocation = FVector::ZeroVector;
		MovementSwayCurveRotation = FVector::ZeroVector;
	}
}

void USKGShooterFrameworkAnimInstance::SetRecoilData()
{
	if (bPerformingRecoil)
	{
		const float ElapsedTimeSinceStart = GetWorld()->GetTimeSeconds() - RecoilStartTimeStamp;
		if (const UCurveVector* ControlRotationCurve = ProceduralAnimData.RecoilSettings.ControlRotationCurve)
		{
			ControlRotationRecoilTarget = ControlRotationCurve->GetVectorValue(ElapsedTimeSinceStart) * RecoilControlRotationMultiplier * ProceduralAnimData.ProceduralStats.ControlRotationRecoilMultipliers;
		}
		if (const UCurveVector* LocationCurve = ProceduralAnimData.RecoilSettings.LocationCurve)
		{
			RecoilLocationTarget = LocationCurve->GetVectorValue(ElapsedTimeSinceStart) * RecoilLocationMultiplier * ProceduralAnimData.ProceduralStats.RecoilLocationMultipliers;
		}
		if (const UCurveVector* RotationCurve = ProceduralAnimData.RecoilSettings.RotationCurve)
		{
			RecoilRotationTarget = RotationCurve->GetVectorValue(ElapsedTimeSinceStart) * RecoilRotationMultiplier * ProceduralAnimData.ProceduralStats.RecoilRotationMultipliers;
		}
	}
}

void USKGShooterFrameworkAnimInstance::SetPoseData(float DeltaSeconds)
{
	if (!bReachedEndOfPoseCurve)
	{
		if (CurrentPose)
		{
			PoseCurveTime = (GetWorld()->GetTimeSeconds() - PoseStartTime) * CurrentPose.PlayRate;
			if (!CurrentPose.GetVectorValues(PoseCurveTime, TargetPoseLocation, TargetPoseRotation, bOffhandIKIsLeftHand))
			{
				TargetPoseLocation = FVector::ZeroVector;
				TargetPoseRotation = FRotator::ZeroRotator;
				bReachedEndOfPoseCurve = true;
			}
			
			if (PoseCurveTime > CurrentPose.CurveEndTime)
			{
				bReachedEndOfPoseCurve = true;
				ShooterPawnComponent->PoseComplete();
			}
		}
		else
		{
			TargetPoseLocation = FVector::ZeroVector;
			TargetPoseRotation = FRotator::ZeroRotator;
			bReachedEndOfPoseCurve = true;
		}
	}
}

void USKGShooterFrameworkAnimInstance::SetFirearmCollisionData()
{
	if (ProceduralAnimData.FirearmCollisionSettings.bUseFirearmCollision)
	{
		SCOPE_CYCLE_COUNTER(STAT_SKGSetFirearmCollisionData);

		if (bFirearmCollisionAllowTrace)
		{
			bFirearmCollisionAllowTrace = false;
			FCollisionQueryParams FirearmCollisionParams;
			FirearmCollisionParams.bTraceComplex = false;
#if ENGINE_MINOR_VERSION >= 5
			FirearmCollisionParams.ClearIgnoredSourceObjects();
#else
			FirearmCollisionParams.ClearIgnoredActors();
#endif
			FirearmCollisionParams.AddIgnoredActors(ProceduralAnimData.FirearmCollisionSettings.ActorsToIgnoreForTrace);
			FirearmCollisionParams.AddIgnoredActor(GetOwningActor());

			GetWorld()->AsyncSweepByChannel(EAsyncTraceType::Single, FirearmCollisionTraceStart, FirearmCollisionTraceEnd, FQuat::Identity, ProceduralAnimData.FirearmCollisionSettings.CollisionChannel, FCollisionShape::MakeSphere(ProceduralAnimData.FirearmCollisionSettings.TraceDiameter), FirearmCollisionParams, FCollisionResponseParams(), &FirearmTraceDelegate);
		}
	}
	else if (!bSettingTestPose)
	{
		FirearmCollisionPushAmount = 0.0f;
		FirearmCollisionLocation = FVector::ZeroVector;
		FirearmCollisionRotation = FRotator::ZeroRotator;
	}
}

void USKGShooterFrameworkAnimInstance::OnFirearmTraceComplete(const FTraceHandle& TraceHandle, FTraceDatum& TraceData)
{
	SCOPE_CYCLE_COUNTER(STAT_SKGOnFirearmTraceComplete);
	bFirearmCollisionAllowTrace = true;
	const bool bOldCollisionHit = bFirearmCollisionHit;
	FHitResult HitResult;
	if (TraceData.OutHits.Num())
	{
		HitResult = TraceData.OutHits[0];
		bFirearmCollisionHit = true;
	}
	else
	{
		bFirearmCollisionHit = false;
	}
	
	if (bFirearmCollisionHit)
	{
		FirearmCollisionHitLocation = HitResult.Location;
	}
	else
	{	// Interpolate back to 0 from no hit
		FirearmCollisionHitLocation = FirearmCollisionTraceEnd;
	}

#if WITH_EDITOR
	if (ShooterPawnComponent)
	{
		if (ShooterPawnComponent->bPrintHit && bFirearmCollisionHit)
		{
			if (HitResult.GetComponent())
			{
				UE_LOG(LogTemp, Warning, TEXT("Hitting Component: %s of Actor: %s"), *HitResult.GetComponent()->GetName(), *HitResult.GetActor()->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Hitting Actor: %s"), *HitResult.GetActor()->GetName());
			}
		}
		if (ShooterPawnComponent->bDrawDebugTrace)
		{
			DrawDebugSweptSphere(GetWorld(), FirearmCollisionTraceStart, FirearmCollisionTraceEnd, ProceduralAnimData.FirearmCollisionSettings.TraceDiameter, FColor::Red, false, -1, 0);
		}
	}
#endif
	const float PushBackDistance = FVector::Dist(FirearmCollisionTraceEnd, FirearmCollisionHitLocation) * ProceduralAnimData.FirearmCollisionSettings.PoseScale;

	FirearmCollisionPushAmount = ((FirearmCollisionHitLocation - FirearmCollisionTraceEnd) / (FirearmCollisionTraceStart - FirearmCollisionTraceEnd)).Size();
	if (FMath::IsNaN(FirearmCollisionPushAmount))
	{
		FirearmCollisionPushAmount = 0.0f;
	}
	if (bIsAiming && FirearmCollisionPushAmount > ProceduralAnimData.FirearmCollisionSettings.CollisionStopAimingDistance)
	{
		bIsAiming = false;
		bInterpAimAlpha = true;
		TargetAimAlpha = 0.0f;
	}
	
	if (ProceduralAnimData.FirearmCollisionSettings.PoseLocation)
	{
		FirearmCollisionTargetLocation = ProceduralAnimData.FirearmCollisionSettings.PoseLocation->GetVectorValue(PushBackDistance);
		FirearmCollisionTargetLocation.X *= bOffhandIKIsLeftHand ? 1.0 : -1.0;
	}
	if (ProceduralAnimData.FirearmCollisionSettings.PoseRotation)
	{
		FirearmCollisionTargetRotation = ProceduralAnimData.FirearmCollisionSettings.PoseRotation->GetVectorValue(PushBackDistance);
		FirearmCollisionTargetRotation.X *= bOffhandIKIsLeftHand ? 1.0 : -1.0;
		FirearmCollisionTargetRotation.Y *= bOffhandIKIsLeftHand ? 1.0 : -1.0;
	}

	if (bFirearmCollisionHit != bOldCollisionHit)
	{
		const bool bExecuted = OnFirearmCollisionChanged.ExecuteIfBound(bFirearmCollisionHit);
	}
}

void USKGShooterFrameworkAnimInstance::SetCustomCurveData(float DeltaSeconds)
{
	if (bPlayCustomCurve)
	{
		const float CustomCurveTime = (GetWorld()->GetTimeSeconds() - CustomCurveStartTime) * CurrentCustomCurve.PlayRate;
		
		if (!CurrentCustomCurve.GetVectorValues(CustomCurveTime, CustomCurveLocation, CustomCurveRotation, bOffhandIKIsLeftHand) || CustomCurveTime > CurrentCustomCurve.CurveEndTime)
		{
			bPlayCustomCurve = false;
		}
	}
}

void USKGShooterFrameworkAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	SCOPE_CYCLE_COUNTER(STAT_SKGNativeThreadSafeUpdate);
	SCOPED_NAMED_EVENT(SKGNativeThreadSafeUpdateAnimation, FColor::Blue);

	HandleProceduralSpine(DeltaSeconds);

	AimCurveAlpha = GetCurveValue(AimCurveName);
	OffhandIKAlpha = 1.0f - GetCurveValue(OffhandIKCurveName);

	HandleAiming(DeltaSeconds);
	if (ProceduralAnimData)
	{
		HandlePawnVelocity();
		HandleOffsets(DeltaSeconds);
		HandleMovementSway(DeltaSeconds);
		HandleMovementLag(DeltaSeconds);
		HandleRotationLag(DeltaSeconds);
		HandleRecoil(DeltaSeconds);
		HandlePose(DeltaSeconds);
		HandleFirearmCollision(DeltaSeconds);

		if (bIsLocallyControlled)
		{
			if (ProceduralAnimData.DeadzoneSettings.bUseDeadzone)
			{
				// Mouse input only handled locally, no need to waste resources running logic on other machines
				HandleDeadzone(DeltaSeconds);
				if (ProceduralAnimData.DeadzoneSettings.bAlwaysInterpolateBackToCenter)
				{
					InterpDeadzoneBackToCenter(DeltaSeconds);
				}
			}
		}
		
		ComponentSpaceFinalLocation = RotationLagLocation;
		ComponentSpaceFinalRotation = DeadzoneHandRotation + MovementLagRotation + RotationLagRotation;
	}

	TestHitRotation = FMath::RInterpTo(TestHitRotation, TestHitTargetRotation, DeltaSeconds, 5.0f);
	TestHitTargetRotation = FMath::RInterpTo(TestHitTargetRotation, FRotator::ZeroRotator, DeltaSeconds, 4.0f);
}

void USKGShooterFrameworkAnimInstance::HandlePawnVelocity()
{
	FVector Velocity;
	if (bUsePelvisForVelocity)
	{
		const FVector CurrentLoc = GetSkelMeshComponent()->GetSocketLocation(PelvisName);
		Velocity = (CurrentLoc - PelvisPosition) / GetDeltaSeconds();
		PelvisPosition = CurrentLoc;
	}
	else
	{
		Velocity = GetOwningActor()->GetVelocity();
	}
	PawnVelocity = Velocity;
	Velocity.Z = 0.0f;
	PawnHorizontalVelocity = Velocity.Size();
}

void USKGShooterFrameworkAnimInstance::HandleOffsets(float DeltaSeconds)
{
	const FTransform BasePoseOffset = bIsLocallyControlled && bUseFirstPerson? ProceduralAnimData.BasePoseOffset.FirstPersonOffset : ProceduralAnimData.BasePoseOffset.ThirdPersonOffset;
	BasePoseOffsetLocation = BasePoseOffset.GetTranslation();
	BasePoseOffsetRotation = BasePoseOffset.Rotator();

	if (!bIsLocallyControlled || !bUseFirstPerson)
	{
		ThirdPersonAimingOffsetLocation = FMath::VInterpTo(ThirdPersonAimingOffsetLocation, ProceduralAnimData.ThirdPersonAimingOffset.GetTranslation(), DeltaSeconds, 10.0f);
		ThirdPersonAimingOffsetRotation = FMath::RInterpTo(ThirdPersonAimingOffsetRotation, ProceduralAnimData.ThirdPersonAimingOffset.Rotator(), DeltaSeconds, 10.0f);
	}
	else
	{
		ThirdPersonAimingOffsetLocation = FVector::ZeroVector;
		ThirdPersonAimingOffsetRotation = FRotator::ZeroRotator;
	}
}

void USKGShooterFrameworkAnimInstance::HandleProceduralSpine(float DeltaSeconds)
{
	if (bUseProceduralSpineUpDown)
	{
		if (ProceduralAnimData.bInFreeLook && bPerformingRecoil)
		{
			OldSpineControlRotation = FreeLookRecoilRotation;
			OldSpineControlRotation -= AccumulatedControlRotationRecoil * 2.5f;
			FreeLookRecoilRotation = OldSpineControlRotation;
			SpineRotationToInterpTo = UKismetMathLibrary::NormalizedDeltaRotator(OldSpineControlRotation, GetOwningActor()->GetActorRotation());
		
			SpineRotationToInterpTo.Roll = (SpineRotationToInterpTo.Pitch * -1.0f);
			SpineRotationToInterpTo.Pitch = 0.0f;
			SpineRotationToInterpTo.Yaw = 0.0f;
		
			if (bIsLocallyControlled)
			{
				SpineRotationUpDown = SpineRotationToInterpTo;
			}
			else
			{
				SpineRotationUpDown = UKismetMathLibrary::RInterpTo(SpineRotationUpDown, SpineRotationToInterpTo, DeltaSeconds, 10.0f);
			}
		}
		else if (!ProceduralAnimData.bInFreeLook && (OldSpineControlRotation != ControlRotation || SpineRotationUpDown != SpineRotationToInterpTo))
		{
			AccumulatedControlRotationRecoil = FRotator::ZeroRotator;
			OldSpineControlRotation = ControlRotation;
			SpineRotationToInterpTo = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, GetOwningActor()->GetActorRotation());
		
			SpineRotationToInterpTo.Roll = (SpineRotationToInterpTo.Pitch * -1.0f);
			SpineRotationToInterpTo.Pitch = 0.0f;
			SpineRotationToInterpTo.Yaw = 0.0f;
		
			if (bIsLocallyControlled)
			{
				SpineRotationUpDown = SpineRotationToInterpTo;
			}
			else
			{
				SpineRotationUpDown = UKismetMathLibrary::RInterpTo(SpineRotationUpDown, SpineRotationToInterpTo, DeltaSeconds, 10.0f);
			}
		}
	}
	
	if (bUseProceduralSpineLeftRight && SpineLeanLeftRight.Pitch != TargetSpineLeanLeftRightAngle)
	{
		if (ProceduralAnimData.LeanLeftRightSettings.bUseSpringInterp)
		{
			SpineLeanLeftRight.Pitch = UKismetMathLibrary::FloatSpringInterp(SpineLeanLeftRight.Pitch, TargetSpineLeanLeftRightAngle, LeanLeftRightSpringState, ProceduralAnimData.LeanLeftRightSettings.Stiffness, ProceduralAnimData.LeanLeftRightSettings.CriticalDampingFactor, DeltaSeconds, ProceduralAnimData.LeanLeftRightSettings.Mass, ProceduralAnimData.LeanLeftRightSettings.TargetVelocityAmount);
		}
		else
		{
			SpineLeanLeftRight.Pitch = UKismetMathLibrary::FInterpTo(SpineLeanLeftRight.Pitch, TargetSpineLeanLeftRightAngle, DeltaSeconds, ProceduralAnimData.LeanLeftRightSettings.InterpSpeed);
		}
	}
}

void USKGShooterFrameworkAnimInstance::HandleAiming(float DeltaSeconds)
{
	if (!SightOffsetLocation.Equals(ProceduralAnimData.AimOffset.GetTranslation()) || !SightOffsetRotation.Equals(ProceduralAnimData.AimOffset.Rotator()))
	{
		if (SightOffsetLocation.Equals(FVector::ZeroVector))
		{ // Temp fix before rewrite so the first ADS should not have any weird path
			SightOffsetLocation = ProceduralAnimData.AimOffset.GetTranslation();
			SightOffsetRotation = ProceduralAnimData.AimOffset.Rotator();
		}
		
		if (ProceduralAnimData.CycleAimingPointSettings.bUseSpringInterpForCyclingAimingPoint)
		{
			const FSKGCycleAimingPointSpringInterpSettings SpringSettings = ProceduralAnimData.ProceduralStats.CycleAimingPointSpringInterpSettings;
			SightOffsetLocation = UKismetMathLibrary::VectorSpringInterp(SightOffsetLocation, ProceduralAnimData.AimOffset.GetTranslation(), AimingLocationSpringState, SpringSettings.LocationStiffness, SpringSettings.LocationCriticalDampingFactor, DeltaSeconds, SpringSettings.LocationMass, SpringSettings.LocationTargetVelocityAmount);
			
			SightOffsetRotation = UKismetMathLibrary::QuaternionSpringInterp(SightOffsetRotation.Quaternion(), ProceduralAnimData.AimOffset.GetRotation(), AimingRotationSpringState, SpringSettings.RotationStiffness, SpringSettings.RotationCriticalDampingFactor, DeltaSeconds, SpringSettings.RotationMass, SpringSettings.RotationTargetVelocityAmount).Rotator();
		}
		else
		{
			SightOffsetLocation = FMath::VInterpTo(SightOffsetLocation, ProceduralAnimData.AimOffset.GetTranslation(), DeltaSeconds, ProceduralAnimData.AimOffsetInterpSpeed);
			SightOffsetRotation = FMath::RInterpTo(SightOffsetRotation, ProceduralAnimData.AimOffset.Rotator(), DeltaSeconds, ProceduralAnimData.AimOffsetInterpSpeed);
		}
	}

	if (bInterpAimAlpha)
	{
		AimAlpha = FMath::FInterpTo(AimAlpha, TargetAimAlpha, DeltaSeconds, ProceduralAnimData.ProceduralStats.AimInterpolationRate);
		if (AimAlpha == TargetAimAlpha)
		{
			bInterpAimAlpha = false;
		}
	}
}

void USKGShooterFrameworkAnimInstance::HandleMovementSway(float DeltaSeconds)
{
	if (ProceduralAnimData.MovementSwaySettings.bUseMovementSway)
	{
		MovementSwayLocationMultiplier = FMath::FInterpTo(MovementSwayLocationMultiplier, ProceduralAnimData.MovementSwaySettings.LocationSettings.Multiplier, DeltaSeconds, ProceduralAnimData.MovementSwaySettings.LocationSettings.MultiplierInterpolationRate);
		MovementSwayRotationMultiplier = FMath::FInterpTo(MovementSwayRotationMultiplier, ProceduralAnimData.MovementSwaySettings.RotationSettings.Multiplier, DeltaSeconds, ProceduralAnimData.MovementSwaySettings.RotationSettings.MultiplierInterpolationRate);
		
		MovementSwayCurveLocation *= MovementSwayLocationMultiplier * ProceduralAnimData.ProceduralStats.MovementSwayMultiplier;
		MovementSwayCurveRotation *= MovementSwayRotationMultiplier * ProceduralAnimData.ProceduralStats.MovementSwayMultiplier;
		const FRotator TargetRotation = FRotator(MovementSwayCurveRotation.Y, MovementSwayCurveRotation.X, MovementSwayCurveRotation.Z);

		MovementSwayLocation = UKismetMathLibrary::VInterpTo(MovementSwayLocation, MovementSwayCurveLocation, DeltaSeconds, ProceduralAnimData.MovementSwaySettings.LocationSettings.InterpSpeed);
		MovementSwayRotation = UKismetMathLibrary::RInterpTo(MovementSwayRotation, TargetRotation, DeltaSeconds, ProceduralAnimData.MovementSwaySettings.RotationSettings.InterpSpeed);
	}
	else
	{
		MovementSwayLocation = FVector::ZeroVector;
		MovementSwayRotation = FRotator::ZeroRotator;
	}
}

void USKGShooterFrameworkAnimInstance::HandleMovementLag(float DeltaSeconds)
{
	if (ProceduralAnimData.MovementLagSettings.bUseMovementLag && (!MovementLagRotation.Equals(FRotator::ZeroRotator) || PawnVelocity.Size()))
	{
		float VerticalVelocity = PawnVelocity.Z;
		VerticalVelocity = UKismetMathLibrary::NormalizeToRange(VerticalVelocity, 0.0f, ProceduralAnimData.MovementLagSettings.UpDownNormalizedMax);
		
		float RightVelocity = FVector::DotProduct(PawnVelocity, GetOwningActor()->GetActorRightVector());
		RightVelocity = UKismetMathLibrary::NormalizeToRange(RightVelocity, 0.0f, ProceduralAnimData.MovementLagSettings.LeftRightNormalizedMax);

		float ForwardVelocity = FVector::DotProduct(PawnVelocity, GetOwningActor()->GetActorForwardVector());
		ForwardVelocity = UKismetMathLibrary::NormalizeToRange(ForwardVelocity, 0.0f, ProceduralAnimData.MovementLagSettings.ForwardBackwardNormalizedMax);
		if (ProceduralAnimData.MovementLagSettings.bDisableForwardMovementLagWhileAiming)
		{
			ForwardVelocity *= 1.0f - AimAlpha;
		}
		float PitchAmount = (ForwardVelocity * ProceduralAnimData.MovementLagSettings.ForwardBackwardMultiplier) + VerticalVelocity;
		
		const FRotator MovementLagMultiplier = ProceduralAnimData.MovementLagSettings.Multiplier;

		const FRotator MovementLagTargetRotation = FRotator(RightVelocity * MovementLagMultiplier.Roll, RightVelocity * MovementLagMultiplier.Yaw, PitchAmount * MovementLagMultiplier.Pitch);

		if (ProceduralAnimData.MovementLagSettings.bUseSpringInterpolation)
		{
			const FSKGMovementLagSpringInterpSettings SpringInterpSettings = ProceduralAnimData.ProceduralStats.MovementLagSpringInterpSettings;

			MovementLagRotation = UKismetMathLibrary::QuaternionSpringInterp(MovementLagRotation.Quaternion(), MovementLagTargetRotation.Quaternion(), MovementLagSpringState, SpringInterpSettings.SpringStiffness, SpringInterpSettings.SpringCriticalDampingFactor, DeltaSeconds, SpringInterpSettings.SpringMass, SpringInterpSettings.SpringTargetVelocityAmount).Rotator();
		}
		else
		{
			MovementLagRotation = FMath::RInterpTo(MovementLagRotation, MovementLagTargetRotation, DeltaSeconds, ProceduralAnimData.MovementLagSettings.InterpSpeed);
		}
	}
	else
	{
		MovementLagRotation = FRotator::ZeroRotator;
	}
}

void USKGShooterFrameworkAnimInstance::HandleRotationLag(float DeltaSeconds)
{
	if (ProceduralAnimData.RotationLagSettings.bUseRotationLag && (!ControlRotation.Equals(OldControlRotation) || (!RotationLagRotation.Equals(FRotator::ZeroRotator) || !RotationLagLocation.Equals(FVector::ZeroVector))) && !ProceduralAnimData.bInFreeLook)
	{
		const float Delta = 1.0f - DeltaSeconds;
		const FQuat RotationDifference = (ControlRotation - OldControlRotation).Quaternion() * Delta;
		OldControlRotation = ControlRotation;
		
		FRotator TargetRotationLagRotation = RotationDifference.Rotator();
		// Left/Right, Forward = Right | Backward = Left, Up/Down
		FVector TargetRotationLagLocation = FVector(TargetRotationLagRotation.Yaw, TargetRotationLagRotation.Pitch, TargetRotationLagRotation.Roll);
		const FRotator RotationLagMultiplier = ProceduralAnimData.RotationLagSettings.RotationMultiplier;
		// Roll, Yaw, Pitch
		TargetRotationLagRotation = FRotator(TargetRotationLagRotation.Yaw * RotationLagMultiplier.Roll, TargetRotationLagRotation.Yaw * RotationLagMultiplier.Yaw, TargetRotationLagRotation.Pitch * RotationLagMultiplier.Pitch);

		const float RollMax = ProceduralAnimData.RotationLagSettings.MaxRotation.Roll;
		const float YawMax = ProceduralAnimData.RotationLagSettings.MaxRotation.Yaw;
		const float PitchMax = ProceduralAnimData.RotationLagSettings.MaxRotation.Pitch;
		TargetRotationLagRotation.Roll = FMath::Clamp(TargetRotationLagRotation.Roll, -RollMax, RollMax);
		TargetRotationLagRotation.Pitch = FMath::Clamp(TargetRotationLagRotation.Pitch, -PitchMax, PitchMax);
		TargetRotationLagRotation.Yaw = FMath::Clamp(TargetRotationLagRotation.Yaw, -YawMax, YawMax);
		
		TargetRotationLagLocation *= ProceduralAnimData.RotationLagSettings.LocationMultiplier;
		const float XMax = ProceduralAnimData.RotationLagSettings.MaxLocation.X;
		const float YMax = ProceduralAnimData.RotationLagSettings.MaxLocation.Y;
		const float ZMax = ProceduralAnimData.RotationLagSettings.MaxLocation.Z;
		TargetRotationLagLocation.X = FMath::Clamp(TargetRotationLagLocation.X, -XMax, XMax);
		TargetRotationLagLocation.Y = FMath::Clamp(TargetRotationLagLocation.Y, -YMax, YMax);
		TargetRotationLagLocation.Z = FMath::Clamp(TargetRotationLagLocation.Z, -ZMax, ZMax);
		
		if (bIsLocallyControlled && ProceduralAnimData.RotationLagSettings.bUseSpringInterpolation)
		{
			const FSKGRotationLagSpringInterpSettings SpringInterpSettings = ProceduralAnimData.ProceduralStats.RotationLagSpringInterpSettings;
			
			RotationLagRotation = UKismetMathLibrary::QuaternionSpringInterp(UnalteredRotationLagRotation.Quaternion(), TargetRotationLagRotation.Quaternion(), RotationLagRotationSpringState, SpringInterpSettings.RotationSpringStiffness, SpringInterpSettings.RotationSpringCriticalDampingFactor, DeltaSeconds, SpringInterpSettings.RotationSpringMass, SpringInterpSettings.RotationSpringTargetVelocityAmount).Rotator();
			
			RotationLagLocation = UKismetMathLibrary::VectorSpringInterp(RotationLagLocation, TargetRotationLagLocation, RotationLagLocationSpringState, SpringInterpSettings.LocationSpringStiffness, SpringInterpSettings.LocationSpringCriticalDampingFactor, DeltaSeconds, SpringInterpSettings.LocationSpringMass, SpringInterpSettings.LocationSpringTargetVelocityAmount);
		}
		else
		{
			RotationLagRotation = FMath::RInterpTo(UnalteredRotationLagRotation, TargetRotationLagRotation, DeltaSeconds, ProceduralAnimData.ProceduralStats.RotationLagInterpSettings.RotationInterpSpeed);
			
			RotationLagLocation = FMath::VInterpTo(RotationLagLocation, TargetRotationLagLocation, DeltaSeconds, ProceduralAnimData.ProceduralStats.RotationLagInterpSettings.LocationInterpSpeed);
		}
		UnalteredRotationLagRotation = RotationLagRotation;
	}
	else
	{
		RotationLagLocation = FVector::ZeroVector;
		RotationLagRotation = FRotator::ZeroRotator;
	}
}

void USKGShooterFrameworkAnimInstance::HandlePose(float DeltaSeconds)
{
	if (bInterpolatePose)
	{
		if (bInterpolatePoseTransform)
		{
			PoseLocation = FMath::VInterpConstantTo(PoseLocation, TargetPoseLocation, DeltaSeconds, PoseInterpolationSpeed);
			PoseRotation = FMath::RInterpConstantTo(PoseRotation, TargetPoseRotation, DeltaSeconds, PoseInterpolationSpeed);
			const float PoseInterpolationTime = (GetWorld()->GetTimeSeconds() - PostInterpolationTimeStart) * PoseInterpolationSpeed;
			PoseInterpolationSpeed += (PoseInterpolationTime * DefaultPoseInterpolationSpeedMultiplier) * 0.5f;
			PoseInterpolationSpeed = FMath::Clamp(PoseInterpolationSpeed, 1.0f, 1000.0f);
		}
		else
		{
			PoseLocation = TargetPoseLocation;
			PoseRotation = TargetPoseRotation;
		}
		
		if (bReachedEndOfPoseCurve && PoseLocation.Equals(TargetPoseLocation) && PoseRotation.Equals(TargetPoseRotation))
		{
			bInterpolatePose = false;
		}
	}
}

void USKGShooterFrameworkAnimInstance::HandleFirearmCollision(float DeltaSeconds)
{
	if (ProceduralAnimData.FirearmCollisionSettings.bUseFirearmCollision)
	{
		const FTransform MuzzleRelativeTransform = ProceduralAnimData.FirearmCollisionSettings.MuzzleRelativeTransform;
		const float MuzzleToRootDistance = MuzzleRelativeTransform.GetTranslation().Size();
		const FTransform EndTransform = MuzzleRelativeTransform * GetOwningComponent()->GetSocketTransform(FirearmCollisionBoneName, RTS_World);
		FirearmCollisionTraceEnd = EndTransform.GetTranslation();
		FirearmCollisionTraceStart = FirearmCollisionTraceEnd + (EndTransform.Rotator().Vector() * -1.0f) * MuzzleToRootDistance;
		
		FirearmCollisionLocation = FMath::VInterpTo(FirearmCollisionLocation, FirearmCollisionTargetLocation, DeltaSeconds, ProceduralAnimData.FirearmCollisionSettings.PoseLocationInterpSpeed);
		const FRotator TargetRot = FRotator(FirearmCollisionTargetRotation.X, FirearmCollisionTargetRotation.Y, FirearmCollisionTargetRotation.Z);
		FirearmCollisionRotation = FMath::RInterpTo(FirearmCollisionRotation, TargetRot, DeltaSeconds, ProceduralAnimData.FirearmCollisionSettings.PoseRotationInterpSpeed);
	}
}

void USKGShooterFrameworkAnimInstance::HandleDeadzone(float DeltaSeconds)
{
	const bool bInterpBackToZero = ProceduralAnimData.DeadzoneSettings.bDisableDeadzoneWhenAiming && bIsAiming;
	FRotator TargetRotation = FRotator::ZeroRotator;
	float TargetInterpSpeed = ProceduralAnimData.DeadzoneSettings.DeadzoneAimingDisableInterpolationSpeed;
	if (!bInterpBackToZero)
	{
		DeadzoneYaw += ProceduralAnimData.MouseInput.X * ProceduralAnimData.DeadzoneSettings.YawRate;
		DeadzonePitch += ProceduralAnimData.MouseInput.Y * ProceduralAnimData.DeadzoneSettings.PitchRate;
	
		DeadzoneYaw = FMath::Clamp(DeadzoneYaw, -ProceduralAnimData.DeadzoneSettings.YawLimit, ProceduralAnimData.DeadzoneSettings.YawLimit);
		DeadzonePitch = FMath::Clamp(DeadzonePitch, -ProceduralAnimData.DeadzoneSettings.PitchLimit, ProceduralAnimData.DeadzoneSettings.PitchLimit);

		TargetRotation = FRotator(0.0f, DeadzoneYaw, DeadzonePitch);
		TargetInterpSpeed = ProceduralAnimData.DeadzoneSettings.InterpolationSpeed;
	}
	
	DeadzoneHandRotation = UKismetMathLibrary::RInterpTo(DeadzoneHandRotation, TargetRotation, DeltaSeconds, TargetInterpSpeed);
}

void USKGShooterFrameworkAnimInstance::InterpDeadzoneBackToCenter(float DeltaSeconds)
{
	DeadzoneHandRotation = UKismetMathLibrary::RInterpTo(DeadzoneHandRotation, FRotator::ZeroRotator, DeltaSeconds, ProceduralAnimData.DeadzoneSettings.InterpolateBackToCenterSpeed);
	DeadzoneYaw = FMath::FInterpTo(DeadzoneYaw, 0.0f, DeltaSeconds, ProceduralAnimData.DeadzoneSettings.InterpolateBackToCenterSpeed);
	DeadzonePitch = FMath::FInterpTo(DeadzonePitch, 0.0f, DeltaSeconds, ProceduralAnimData.DeadzoneSettings.InterpolateBackToCenterSpeed);
}

void USKGShooterFrameworkAnimInstance::InterpRecoilToNone(float DeltaSeconds)
{
	RecoilLocation = FMath::VInterpTo(RecoilLocation, FVector::ZeroVector, DeltaSeconds, ProceduralAnimData.RecoilSettings.LocationInterpToNoneSpeed);
	RecoilRotation = FMath::RInterpTo(RecoilRotation, FRotator::ZeroRotator, DeltaSeconds, ProceduralAnimData.RecoilSettings.RotationInterpToNoneSpeed);
	float ControllerRoll = 0.0f;
	if (bIsLocallyControlled)
	{
		float CurrentControllerRoll = 0.0f;
		if (APawn* Pawn = TryGetPawnOwner())
		{
			CurrentControllerRoll = Pawn->GetControlRotation().Roll;
		}
		ControllerRoll = FMath::FInterpTo(CurrentControllerRoll, 0.0f, DeltaSeconds, ProceduralAnimData.RecoilSettings.RotationInterpToNoneSpeed);
		ControlRotationRecoilTarget.Z = ControllerRoll;
	}
	
	if (RecoilLocation.Equals(FVector::ZeroVector) && RecoilRotation.Equals(FRotator::ZeroRotator) && ControllerRoll == 0.0f)
	{
		bPerformingRecoil = false;
	}
}

void USKGShooterFrameworkAnimInstance::HandleRecoil(float DeltaSeconds)
{
	if (bPerformingRecoil)
	{
		const float Delta = 100.0f * DeltaSeconds;
		
		RecoilLocationTarget *= RandomRecoilLocation;
		RecoilRotationTarget *= RandomRecoilRotation;

		if (bIsLocallyControlled && !ControlRotationRecoilTarget.Equals(FVector::ZeroVector))
		{
			if (APawn* Pawn = TryGetPawnOwner())
			{
				ControlRotationRecoilTarget *= RandomControlRotationRecoil;
				ControlRotationRecoilTargetRot = FRotator(ControlRotationRecoilTarget.X, ControlRotationRecoilTarget.Y, ControlRotationRecoilTarget.Z) * Delta;
				AccumulatedControlRotationRecoil += ControlRotationRecoilTargetRot;
				Pawn->AddControllerPitchInput(ControlRotationRecoilTargetRot.Pitch);
				Pawn->AddControllerYawInput(ControlRotationRecoilTargetRot.Yaw);
				if (AController* PawnController = Pawn->GetController())
				{
					const FRotator CurrentControllerRotation = PawnController->GetControlRotation();
					PawnController->SetControlRotation(FRotator(CurrentControllerRotation.Pitch, CurrentControllerRotation.Yaw, ControlRotationRecoilTargetRot.Roll));
				}
			}
		}
		
		RecoilLocation += RecoilLocationTarget * Delta;
		RecoilRotation += FRotator(RecoilRotationTarget.X, RecoilRotationTarget.Y, RecoilRotationTarget.Z) * Delta;
		InterpRecoilToNone(DeltaSeconds);
	}
}

void USKGShooterFrameworkAnimInstance::PerformRecoil(const FRotator& ControlRotationMultiplier, const FVector& LocationMultiplier, const FRotator& RotationMultiplier)
{
	RecoilStartTimeStamp = GetWorld()->GetTimeSeconds();
	RecoilLocationMultiplier = LocationMultiplier;
	//XYZ = Roll, Pitch, Yaw
	RecoilRotationMultiplier = FVector(RotationMultiplier.Pitch, RotationMultiplier.Yaw, RotationMultiplier.Roll);
	// XYZ = Pitch, Yaw, Roll
	RecoilControlRotationMultiplier = FVector(ControlRotationMultiplier.Pitch, ControlRotationMultiplier.Yaw, ControlRotationMultiplier.Roll);

	// X = up/down, Y = forward/backward, Z = left/right
	RandomRecoilLocation = FVector(ProceduralAnimData.RecoilSettings.XRandom.GetRandom(), ProceduralAnimData.RecoilSettings.YRandom.GetRandom(), ProceduralAnimData.RecoilSettings.ZRandom.GetRandom());
	RandomRecoilRotation = FVector(ProceduralAnimData.RecoilSettings.RollRandom.GetRandom(), ProceduralAnimData.RecoilSettings.YawRandom.GetRandom(), ProceduralAnimData.RecoilSettings.PitchRandom.GetRandom());
	
	if (bIsLocallyControlled)
	{
		RandomControlRotationRecoil = FVector(ProceduralAnimData.RecoilSettings.ControlPitchRandom.GetRandom(), ProceduralAnimData.RecoilSettings.ControlYawRandom.GetRandom(), ProceduralAnimData.RecoilSettings.ControlRollRandom.GetRandom());
	}
	bPerformingRecoil = true;
}

void USKGShooterFrameworkAnimInstance::TryPerformPose(const FSKGToFromCurveSettings& PoseData, const bool ExitPose)
{
	CurrentPoseData = PoseData;
	const FSKGFirstAndThirdPersonCurveSettings SelectedPose = ExitPose ? CurrentPoseData.FromCurve : CurrentPoseData.ToCurve;
	if (bIsLocallyControlled)
	{
		CurrentPose = bUseFirstPerson ? SelectedPose.FirstPersonCurve : SelectedPose.ThirdPersonCurve;
	}
	else
	{
		CurrentPose = SelectedPose.ThirdPersonCurve;
	}
	
	if (bReachedEndOfPoseCurve)
	{
		PoseStartTime = GetWorld()->GetTimeSeconds();
	}
	else if (CurrentPoseData == PoseData)
	{
		const float TimeFromEndOfCurve = (CurrentPose.CurveEndTime - PoseCurveTime) / CurrentPoseData.InterpolationSpeedMultiplier;
		PoseStartTime = GetWorld()->GetTimeSeconds() - TimeFromEndOfCurve;
	}
	
	bReachedEndOfPoseCurve = false;
	bInterpolatePose = true;

	bInterpolatePoseTransform = CurrentPoseData.bInterpolate;
	PoseInterpolationSpeed = CurrentPoseData.InterpolationSpeed;
	PostInterpolationTimeStart = GetWorld()->GetTimeSeconds();
}

void USKGShooterFrameworkAnimInstance::PerformCustomCurve(const FSKGFirstAndThirdPersonCurveSettings& CurveData)
{
	if (CurveData)
	{
		bPlayCustomCurve = true;
		CustomCurveStartTime = GetWorld()->GetTimeSeconds();
		if (bIsLocallyControlled)
		{
			CurrentCustomCurve = bUseFirstPerson? CurveData.FirstPersonCurve : CurveData.ThirdPersonCurve;
		}
		else
		{
			CurrentCustomCurve = CurveData.ThirdPersonCurve;
		}
	}
	else
	{
		bPlayCustomCurve = false;
	}
}

void USKGShooterFrameworkAnimInstance::SetTestPose(const FVector& Location, const FVector& Rotation, bool bFirearmCollision)
{
	if (bFirearmCollision)
	{
		FirearmCollisionLocation = Location;
		FirearmCollisionRotation = FRotator(Rotation.X, Rotation.Y, Rotation.Z);
		PoseLocation = FVector::ZeroVector;
		PoseRotation = FRotator::ZeroRotator;
		bSettingTestPose = true;
	}
	else
	{
		PoseLocation = Location;
		PoseRotation = FRotator(Rotation.X, Rotation.Y, Rotation.Z);
		FirearmCollisionLocation = FVector::ZeroVector;
		FirearmCollisionRotation = FRotator::ZeroRotator;
		bSettingTestPose = false;
	}
}

void USKGShooterFrameworkAnimInstance::GetCurrentPoseValues(FVector& Location, FVector& Rotation, bool bFirearmCollision)
{
	if (bFirearmCollision)
	{
		Location = FirearmCollisionLocation;
		Rotation = FVector(FirearmCollisionRotation.Pitch, FirearmCollisionRotation.Yaw, FirearmCollisionRotation.Roll);
	}
	else
	{
		Location = PoseLocation;
		Rotation = FVector(PoseRotation.Pitch, PoseRotation.Yaw, PoseRotation.Roll);
	}
}

void USKGShooterFrameworkAnimInstance::TestHit(const FHitResult& HitResult)
{
	FVector Normal = HitResult.ImpactNormal * 15;
	TestHitTargetRotation = FRotator(Normal.X, 0.0f, Normal.Y);

	UE_LOG(LogTemp, Warning, TEXT("Normal: %s"), *Normal.ToString());
}