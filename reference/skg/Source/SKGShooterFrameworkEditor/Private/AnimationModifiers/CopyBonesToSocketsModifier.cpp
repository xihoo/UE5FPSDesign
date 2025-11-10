// Copyright 2025, Dakota Dawe, All rights reserved


#include "AnimationModifiers/CopyBonesToSocketsModifier.h"

#define LOCTEXT_NAMESPACE "CopyBonesToSocketsModifier"

void UCopyBonesToSocketsModifier::CopyBone(IAnimationDataController& Controller, FAnimPose& AnimPose, const int32 AnimKey, bool bSourceIsSocket, const FName& Source, const FName& Target)
{
	FTransform SourceTransform = bSourceIsSocket ? UAnimPoseExtensions::GetSocketPose(AnimPose, Source, BonePoseSpace) : UAnimPoseExtensions::GetBonePose(AnimPose, Source, BonePoseSpace);

	// UAnimDataController::UpdateBoneTrackKeys expects local transforms so we need to convert the source transforms to target bone local transforms first. 
	UAnimPoseExtensions::SetBonePose(AnimPose, SourceTransform, Target, BonePoseSpace);
	FTransform BonePoseTargetLocal = UAnimPoseExtensions::GetBonePose(AnimPose, Target, EAnimPoseSpaces::Local);

	const FInt32Range KeyRangeToSet(AnimKey, AnimKey + 1);
	Controller.UpdateBoneTrackKeys(Target, KeyRangeToSet, { BonePoseTargetLocal.GetLocation() }, { BonePoseTargetLocal.GetRotation() }, { BonePoseTargetLocal.GetScale3D() });
}

void UCopyBonesToSocketsModifier::OnApply_Implementation(UAnimSequence* AnimationSequence)
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
		UE_LOG(LogAnimation, Error, TEXT("CopyBonesToSocketsModifier failed. Reason: Invalid Data Model. Animation: %s"), *GetNameSafe(AnimationSequence));
		return;
	}
	
	// Temporally set ForceRootLock to true so we get the correct transforms regardless of the root motion configuration in the animation
	TGuardValue<bool> ForceRootLockGuard(AnimationSequence->bForceRootLock, true);

	// Start editing animation data
	constexpr bool bShouldTransact = false;
	Controller.OpenBracket(LOCTEXT("CopyBonesToSocketsModifier_Bracket", "Updating bones"), bShouldTransact);

	// Get the transform of all the source bones in the desired space
	const int32 NumKeys = Model->GetNumberOfKeys();
	for (int32 AnimKey = 0; AnimKey < NumKeys; AnimKey++)
	{
		for (const FBoneSocketReferencePair& BoneSocketPair : BoneSocketPairs)
		{
			const FName SocketName = BoneSocketPair.SourceSocket.SocketName;
			const FName BoneName = BoneSocketPair.TargetBone.BoneName;
			FAnimPose AnimPose;
			UAnimPoseExtensions::GetAnimPoseAtFrame(AnimationSequence, AnimKey, FAnimPoseEvaluationOptions(), AnimPose);
			CopyBone(Controller, AnimPose, AnimKey, true, SocketName, BoneName);
		}
	}

	// Done editing animation data
	Controller.CloseBracket(bShouldTransact);
}

#undef LOCTEXT_NAMESPACE