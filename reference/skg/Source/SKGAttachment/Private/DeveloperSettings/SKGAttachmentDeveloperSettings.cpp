// Copyright 2024, Dakota Dawe, All rights reserved


#include "DeveloperSettings/SKGAttachmentDeveloperSettings.h"

USKGAttachmentDeveloperSettings::USKGAttachmentDeveloperSettings(const FObjectInitializer& ObjectInitializer)
{
}

FSKGAttachmentSlotCompatbility USKGAttachmentDeveloperSettings::GetSlotCompatibility(const FGameplayTag& SlotTag)
{
	USKGAttachmentDeveloperSettings* AttachmentDeveloperSettings = NewObject<USKGAttachmentDeveloperSettings>();
	for (FSoftObjectPath SoftTablePath : AttachmentDeveloperSettings->SlotCompatibilityTableList)
	{
		if (UDataTable* DataTable = LoadObject<UDataTable>(nullptr, *SoftTablePath.ToString(), nullptr, LOAD_None, nullptr))
		{
			TArray<FSKGAttachmentSlotCompatbility*> SlotCompatibilities;
			DataTable->GetAllRows(FString("SlotCompatibility"), SlotCompatibilities);
			for (FSKGAttachmentSlotCompatbility* SlotCompatibility : SlotCompatibilities)
			{
				if (SlotCompatibility->SlotTag == SlotTag)
				{
					return *SlotCompatibility;
				}
			}
		}
	}
	return FSKGAttachmentSlotCompatbility();
}

TArray<FPrimaryAssetType>& USKGAttachmentDeveloperSettings::GetPrimarAssetTypes()
{
	USKGAttachmentDeveloperSettings* AttachmentDeveloperSettings = NewObject<USKGAttachmentDeveloperSettings>();
	return AttachmentDeveloperSettings->AssetTypes;
}
