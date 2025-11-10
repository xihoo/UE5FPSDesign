// Copyright 2025, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "AnimPose.h"
#include "Copy_ik_hand_gun_BonesToSocket.generated.h"

/** This modifier will copy ik_hand_gun to a socket of your choice,
 * then correct ik_hand_r and ik_hand_l to their corresponding hand_r and hand_l bones
 */
UCLASS()
class SKGSHOOTERFRAMEWORKEDITOR_API UCopy_ik_hand_gun_BonesToSocket : public UAnimationModifier
{
	GENERATED_BODY()
private:
	void CopyBone(IAnimationDataController& Controller, FAnimPose& AnimPose, const int32 AnimKey, bool bSourceIsSocket, const FName& Source, const FName& Target);
	
public:
	// Source socket for ik_hand_gun to be copied to
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ik_hand_gun")
	FName SocketName {NAME_None};
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ik_hand_gun")
	FName ik_hand_gunName {"ik_hand_gun"};
	
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ik_hand_l")
	FName ik_hand_lName {"ik_hand_l"};
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ik_hand_l")
	FName hand_lName {"hand_l"};
	
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ik_hand_r")
	FName ik_hand_rName {"ik_hand_r"};
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ik_hand_r")
	FName hand_rName {"hand_r"};
	
	/** Space to convert SourceBone transforms into before copying them to TargetBone */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	EAnimPoseSpaces BonePoseSpace {EAnimPoseSpaces::World};
	
	virtual void OnApply_Implementation(UAnimSequence* AnimationSequence) override;
};
