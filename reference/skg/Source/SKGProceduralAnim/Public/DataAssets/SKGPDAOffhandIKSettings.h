// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SKGPDAOffhandIKSettings.generated.h"


class UAnimSequence;

UCLASS()
class SKGPROCEDURALANIM_API USKGPDAOffhandIKSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// Animation to be played on the left hand for the left hand IK
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGOffhandIK")
	TObjectPtr<UAnimSequence> LeftHandIKPose;
	// Animation to be played on the Right hand for the Right hand IK
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGOffhandIK")
	TObjectPtr<UAnimSequence> RightHandIKPose;
};
