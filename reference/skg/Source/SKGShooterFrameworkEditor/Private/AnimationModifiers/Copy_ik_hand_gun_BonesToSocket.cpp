// Copyright 2025, Dakota Dawe, All rights reserved


#include "AnimationModifiers/Copy_ik_hand_gun_BonesToSocket.h"

#define LOCTEXT_NAMESPACE "Copy_ik_hand_gun_BonesToSocketModifier"

void UCopy_ik_hand_gun_BonesToSocket::CopyBone(IAnimationDataController& Controller, FAnimPose& AnimPose, const int32 AnimKey, bool bSourceIsSocket, const FName& Source, const FName& Target)
{
	FTransform SourceTransform = bSourceIsSocket ? UAnimPoseExtensions::GetSocketPose(AnimPose, Source, BonePoseSpace) : UAnimPoseExtensions::GetBonePose(AnimPose, Source, BonePoseSpace);

	// UAnimDataController::UpdateBoneTrackKeys expects local transforms so we need to convert the source transforms to target bone local transforms first. 
	UAnimPoseExtensions::SetBonePose(AnimPose, SourceTransform, Target, BonePoseSpace);
	FTransform BonePoseTargetLocal = UAnimPoseExtensions::GetBonePose(AnimPose, Target, EAnimPoseSpaces::Local);

	const FInt32Range KeyRangeToSet(AnimKey, AnimKey + 1);
	Controller.UpdateBoneTrackKeys(Target, KeyRangeToSet, { BonePoseTargetLocal.GetLocation() }, { BonePoseTargetLocal.GetRotation() }, { BonePoseTargetLocal.GetScale3D() });
}

void UCopy_ik_hand_gun_BonesToSocket::OnApply_Implementation(UAnimSequence* AnimationSequence)
{
	if (AnimationSequence == nullptr)
	{
		UE_LOG(LogAnimation, Error, TEXT("CopyBonesModifier failed. Reason: Invalid AnimationSequence"));
		return;
	}

	IAnimationDataController& Controller = AnimationSequence->GetController();
	const IAnimationDataModel* Model = AnimationSequence->GetDataModel();

	if (Model == nullptr)
	{
		UE_LOG(LogAnimation, Error, TEXT("Copy_ik_hand_gun_BonesToSocket failed. Reason: Invalid Data Model. Animation: %s"), *GetNameSafe(AnimationSequence));
		return;
	}

	// Temporally set ForceRootLock to true so we get the correct transforms regardless of the root motion configuration in the animation
	TGuardValue<bool> ForceRootLockGuard(AnimationSequence->bForceRootLock, true);

	// Start editing animation data
	constexpr bool bShouldTransact = false;
	Controller.OpenBracket(LOCTEXT("Copy_ik_hand_gun_BonesToSocket_Bracket", "Updating bones"), bShouldTransact);

	// Get the transform of all the source bones in the desired space
	const int32 NumKeys = Model->GetNumberOfKeys();
	for (int32 AnimKey = 0; AnimKey < NumKeys; AnimKey++)
	{
		FAnimPose AnimPose;
		UAnimPoseExtensions::GetAnimPoseAtFrame(AnimationSequence, AnimKey, FAnimPoseEvaluationOptions(), AnimPose);
		CopyBone(Controller, AnimPose, AnimKey, true, SocketName, ik_hand_gunName);
		CopyBone(Controller, AnimPose, AnimKey, false, hand_rName, ik_hand_rName);
		CopyBone(Controller, AnimPose, AnimKey, false, hand_lName, ik_hand_lName);
	}

	// Done editing animation data
	Controller.CloseBracket(bShouldTransact);
}

#undef LOCTEXT_NAMESPACE