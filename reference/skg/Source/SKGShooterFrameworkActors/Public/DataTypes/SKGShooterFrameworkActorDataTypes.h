// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Launch/Resources/Version.h"
#if ENGINE_MINOR_VERSION >= 5
#include "StructUtils/InstancedStruct.h"
#else
#include "InstancedStruct.h"
#endif
#include "Components/SKGAttachmentComponent.h"
#include "SKGShooterFrameworkActorDataTypes.generated.h"

class USKGAttachmentComponent;
class AActor;
class UDataAsset;

USTRUCT(BlueprintType)
struct SKGSHOOTERFRAMEWORKACTORS_API FSKGAttachmentComponentData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGData")
	TSubclassOf<USKGAttachmentComponent> AttachmentComponentClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGData")
	FName AttachmentComponentName {NAME_None};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGData")
	TSubclassOf<AActor> ActorClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGData")
	TObjectPtr<UDataAsset> ActorClassDefinition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKGData")
	FName SocketToAttachTo {NAME_None};
	// If this SlotTag is empty, it is assumed you are intending to use the SlotCompatbilityOverride
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment|Attachment|Compatibility|GameplayTag")
	FGameplayTag SlotTag;
	UPROPERTY(EditDefaultsOnly, Category = "SKGAttachment|Attachment|Compatibility|GameplayTag")
	FSKGAttachmentSlotCompatbility SlotCompatbilityOverride;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGData", meta = (BaseStruct = "/Script/SKGShooterFrameworkActors.SKGAttachmentComponentData"))
	TArray<FInstancedStruct> Children;

	FSKGAttachmentComponentData();
};