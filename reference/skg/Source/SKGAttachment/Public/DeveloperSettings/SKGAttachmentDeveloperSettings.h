// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "Engine/DeveloperSettings.h"
#include "SKGAttachmentDeveloperSettings.generated.h"

USTRUCT(BlueprintType)
struct FSKGAttachmentSlotCompatbility : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment")
	FGameplayTag SlotTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGAttachment")
	FGameplayTagQuery Query;

	FSKGAttachmentSlotCompatbility() {}
	FSKGAttachmentSlotCompatbility(const FGameplayTag& GameplayTag): SlotTag(GameplayTag) {}

	bool operator==(const FSKGAttachmentSlotCompatbility& RHS) const
	{
		return SlotTag == RHS.SlotTag;
	}
};

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "SKG Shooter Framework Attachment System"))
class SKGATTACHMENT_API USKGAttachmentDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	USKGAttachmentDeveloperSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual FName GetCategoryName() const override { return FName("Plugins"); }

	static FSKGAttachmentSlotCompatbility GetSlotCompatibility(const FGameplayTag& SlotTag);
	static TArray<FPrimaryAssetType>& GetPrimarAssetTypes();

	/*
	 * These are the primary asset types that the system will look for by default to return what is compatible
	 * with X attachment component defined by the settings below
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "SKGAttachment")
	TArray<FPrimaryAssetType> AssetTypes;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "SKGAttachment", meta = (AllowedClasses = "/Script/Engine.DataTable"))
	TArray<FSoftObjectPath> SlotCompatibilityTableList; 
};
