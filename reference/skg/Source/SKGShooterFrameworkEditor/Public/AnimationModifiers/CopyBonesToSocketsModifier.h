// Copyright 2025, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "AnimPose.h"
#include "CopyBonesToSocketsModifier.generated.h"

USTRUCT(BlueprintType)
struct FBoneSocketReferencePair
{
	GENERATED_BODY()

	/** Socket to get transform from  */
	UPROPERTY(EditAnywhere, Category = "Default")
	FSocketReference SourceSocket;
	/** Bone to update with the transform copied from SourceBone */
	UPROPERTY(EditAnywhere, Category = "Default")
	FBoneReference TargetBone;
};

/** Animation Modifier to copy the transform of 'SourceSocket(s)' to 'TargetBone(s)' */
UCLASS()
class SKGSHOOTERFRAMEWORKEDITOR_API UCopyBonesToSocketsModifier : public UAnimationModifier
{
	GENERATED_BODY()
private:
	void CopyBone(IAnimationDataController& Controller, FAnimPose& AnimPose, int32 AnimKey, bool bSourceIsSocket, const FName& Source, const FName& Target);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TArray<FBoneSocketReferencePair> BoneSocketPairs;
	/** Space to convert SourceBone transforms into before copying them to TargetBone */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	EAnimPoseSpaces BonePoseSpace {EAnimPoseSpaces::World};
	
	virtual void OnApply_Implementation(UAnimSequence* AnimationSequence) override;
};
