// Copyright 2023, Dakota Dawe, All rights reserved


#include "Statics/SKGAttachmentHelpers.h"
#include "Components/SKGAttachmentComponent.h"
#include "Components/SKGAttachmentManagerComponent.h"
#include "DataTypes/SKGAttachmentDataTypes.h"

#include "JsonObjectConverter.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "HAL/FileManagerGeneric.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

DECLARE_CYCLE_STAT(TEXT("SKGGetAttachmentComponents"), STAT_SKGGetAttachmentComponents, STATGROUP_SKGAttachmentHelpers);
DECLARE_CYCLE_STAT(TEXT("SKGConstructActorFromAttachmentManagerData"), STAT_SKGConstructActorFromAttachmentManagerData, STATGROUP_SKGAttachmentHelpers);
DECLARE_CYCLE_STAT(TEXT("SKGConstructAttachmentManagerData"), STAT_SKGConstructAttachmentManagerData, STATGROUP_SKGAttachmentHelpers);

USKGAttachmentManagerComponent* USKGAttachmentHelpers::GetAttachmentManagerComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<USKGAttachmentManagerComponent>() : nullptr;
}

TArray<USKGAttachmentComponent*> USKGAttachmentHelpers::GetAttachmentComponents(const AActor* Actor)
{
	if (Actor)
	{
		TArray<USKGAttachmentComponent*> Components;
		Actor->GetComponents<USKGAttachmentComponent>(Components);
		return Components;
	}
	return TArray<USKGAttachmentComponent*>();
}

TArray<AActor*> USKGAttachmentHelpers::GetAllAttachedAttachments(const AActor* Actor)
{
	TArray<AActor*> Attachments;
	if (Actor)
	{
		Actor->GetAttachedActors(Attachments, true, true);
	}
	return Attachments;
}

TArray<USKGAttachmentComponent*> USKGAttachmentHelpers::GetAllAttachedAttachmentComponents(AActor* Actor)
{
	TArray<USKGAttachmentComponent*> Components = USKGAttachmentHelpers::GetAttachmentComponents(Actor);
	Components.Reserve(Components.Num() + 10);
	if (Components.Num())
	{
		TArray<AActor*> Attached = GetAllAttachedAttachments(Actor);
		for (const AActor* AttachedActor : Attached)
		{
			Components.Append(USKGAttachmentHelpers::GetAttachmentComponents(AttachedActor));
		}
	}
	Components.Shrink();
	return Components;
}

FString USKGAttachmentHelpers::GetStringFromAttachmentManagerData(const FSKGAttachmentActor& Data)
{
	FString DataString;
	if (FJsonObjectConverter::UStructToJsonObjectString(Data, DataString))
	{
		return DataString;
	}
	return FString("CONVERSION FAILED");
}

bool USKGAttachmentHelpers::ConstructAttachmentManagerData(const USKGAttachmentManagerComponent* AttachmentManagerComponent, FSKGAttachmentActor& Data)
{
	if (AttachmentManagerComponent && AttachmentManagerComponent->GetOwner())
	{
		SCOPE_CYCLE_COUNTER(STAT_SKGConstructAttachmentManagerData);
		Data.ActorClass = AttachmentManagerComponent->GetOwner()->GetClass();
		
		const TArray<FSKGAttachmentComponentItem>& AttachmentComponentItems = AttachmentManagerComponent->GetAttachmentComponents();
		Data.Attachments.Reserve(AttachmentComponentItems.Num());
		for (const FSKGAttachmentComponentItem& Item : AttachmentComponentItems)
		{
			if (const USKGAttachmentComponent* AttachmentComponent = Item.AttachmentComponent)
			{
				if (const AActor* Attachment = AttachmentComponent->GetAttachment())
				{
					FSKGActorsAttachment AttachmentData;
					AttachmentData.ActorClass = Attachment->GetClass();
					AttachmentData.AttachmentComponentName = AttachmentComponent->GetFName();
					AttachmentData.AttachmentOffset = AttachmentComponent->GetAttachmentOffset();
					Data.Attachments.Add(AttachmentData);
				}
			}
		}
		Data.Attachments.Shrink();
		return true;
	}
	return false;
}

bool USKGAttachmentHelpers::SerializeAttachmentManagerToJson(const USKGAttachmentManagerComponent* AttachmentManagerComponent, FString& SerializedString)
{
	FSKGAttachmentActor AttachmentData;
	if (ConstructAttachmentManagerData(AttachmentManagerComponent, AttachmentData))
	{
		return FJsonObjectConverter::UStructToJsonObjectString(AttachmentData, SerializedString);
	}
	return false;
}

bool USKGAttachmentHelpers::DeserializeAttachmentManagerJson(const FString& JsonString, FSKGAttachmentActor& Data)
{
	return !JsonString.IsEmpty() && FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &Data);
}

void USKGAttachmentHelpers::DestroyAttachments(USKGAttachmentManagerComponent* AttachmentManagerComponent)
{
	TArray<FSKGAttachmentComponentItem> AttachmentComponentItems = AttachmentManagerComponent->GetAttachmentComponents();
	for (const FSKGAttachmentComponentItem& Item : AttachmentComponentItems)
	{
		if (USKGAttachmentComponent* AttachmentComponent = Item.AttachmentComponent)
		{
			AttachmentComponent->DestroyAttachment();
		}
	}
}

void USKGAttachmentHelpers::SetupAttachments(USKGAttachmentManagerComponent* AttachmentManagerComponent,
	const FSKGAttachmentActor& Data)
{
	for (const FSKGActorsAttachment& Attachment : Data.Attachments)
	{
		if (Attachment)
		{
			TArray<FSKGAttachmentComponentItem> AttachmentComponentItems = AttachmentManagerComponent->GetAttachmentComponents();
			for (const FSKGAttachmentComponentItem& Item : AttachmentComponentItems)
			{
				if (USKGAttachmentComponent* AttachmentComponent = Item.AttachmentComponent)
				{
					if (!AttachmentComponent->HasAttachment() && AttachmentComponent->GetFName().IsEqual(Attachment.AttachmentComponentName) && AttachmentComponent->IsAttachmentClassCompatible(Attachment.ActorClass))
					{
						AttachmentComponent->SetAttachmentOffset(Attachment.AttachmentOffset);
						AttachmentComponent->TrySetupAttachment(Attachment.ActorClass);
						break;
					}
				}
			}
		}
	}
}

AActor* USKGAttachmentHelpers::ConstructActorFromAttachmentManagerData(const UObject* WorldContextObject, const FSKGAttachmentActor& Data)
{
	if (WorldContextObject && WorldContextObject->GetWorld() && Data)
	{
		if (AActor* Actor = WorldContextObject->GetWorld()->SpawnActor<AActor>(Data.ActorClass, FTransform()))
		{
			if (USKGAttachmentManagerComponent* AttachmentManagerComponent = GetAttachmentManagerComponent(Actor))
			{
				SCOPE_CYCLE_COUNTER(STAT_SKGConstructActorFromAttachmentManagerData);
				AttachmentManagerComponent->SetCanSpawnDefaultAttachments(false);
				
				DestroyAttachments(AttachmentManagerComponent);
				SetupAttachments(AttachmentManagerComponent, Data);

				AttachmentManagerComponent->SetCanSpawnDefaultAttachments(true);
			}
			else
			{
				Actor->Destroy();
			}
			return Actor;
		}
	}
	return nullptr;
}

void USKGAttachmentHelpers::ConstructExistingActorFromAttachmentManagerData(const AActor* Actor, const FSKGAttachmentActor& Data)
{
	if (Actor)
	{
		if (USKGAttachmentManagerComponent* AttachmentManagerComponent = GetAttachmentManagerComponent(Actor))
		{
			SCOPE_CYCLE_COUNTER(STAT_SKGConstructActorFromAttachmentManagerData);
			AttachmentManagerComponent->SetCanSpawnDefaultAttachments(false);
				
			DestroyAttachments(AttachmentManagerComponent);
			SetupAttachments(AttachmentManagerComponent, Data);

			AttachmentManagerComponent->SetCanSpawnDefaultAttachments(true);
		}
	}
}

bool USKGAttachmentHelpers::SaveStringToFile(const FString& Path, const FString& FileContent)
{
	return FFileHelper::SaveStringToFile(FileContent, *Path);
}

bool USKGAttachmentHelpers::LoadFileToString(const FString& Path, FString& OutString)
{
	return FFileHelper::LoadFileToString(OutString, *Path);
}

bool USKGAttachmentHelpers::GetAllFiles(FString Path, TArray<FString>& OutFiles)
{
	bool ValidFiles = false;
	if (FPaths::DirectoryExists(Path))
	{
		ValidFiles = true;
		Path.Append("*");
		FFileManagerGeneric::Get().FindFiles(OutFiles, *Path, true, false);
	}

	return ValidFiles;
}

FSKGAttachmentActorTEST USKGAttachmentHelpers::ConstructSKGAttachmentActorData(const FString& Data)
{
	FSKGAttachmentActorTEST Test;
	FJsonObjectConverter::JsonObjectStringToUStruct(Data, &Test);
	return Test;
}

AActor* USKGAttachmentHelpers::ConstructSKGAttachmentTEST(const UObject* WorldContextObject, const FSKGAttachmentActorTEST& AttachmentActorTEST)
{
	AActor* Actor = WorldContextObject->GetWorld()->SpawnActor<AActor>(AttachmentActorTEST.ActorClass, FTransform());
	for (const FSKGAttachmentTEST& AttachmentTest : AttachmentActorTEST.Attachments)
	{
		USKGAttachmentComponent* AttachmentComponent = NewObject<USKGAttachmentComponent>(Actor, AttachmentTest.AttachmentComponentClass);
		AttachmentComponent->SetAttachToMeshName(AttachmentTest.AttachToMeshName);
		AttachmentComponent->SetAttachToSocketName(AttachmentTest.AttachToSocket);
		AttachmentComponent->ComponentName = "CUSTOM TEST";
		AttachmentComponent->bConstructDefaults = false;
		AttachmentComponent->RegisterComponent();

		if (AttachmentTest.ActorClass)
		{
			AttachmentComponent->TrySetupAttachment(AttachmentTest.ActorClass);
		}
	}
	
	return Actor;
}

FSKGAttachmentActorTEST USKGAttachmentHelpers::GetSKGAttachmentActorTEST(AActor* Actor, FString& SerializedString)
{
	FSKGAttachmentActorTEST Data;
	Data.ActorClass = Actor->GetClass();
	USKGAttachmentManagerComponent* AttachmentManager = Actor->FindComponentByClass<USKGAttachmentManagerComponent>();
	for (const FSKGAttachmentComponentItem& AttachmentComponentItem : AttachmentManager->GetAttachmentComponents())
	{
		USKGAttachmentComponent* AttachmentComponent = AttachmentComponentItem.AttachmentComponent;
		
		FSKGAttachmentTEST AttachmentData;
		AttachmentData.AttachmentComponentClass = AttachmentComponent->GetClass();
		AttachmentData.AttachmentOffset = AttachmentComponent->GetAttachmentOffset();
		AttachmentData.AttachToMeshName = AttachmentComponent->AttachToMeshName;
		AttachmentData.AttachToSocket = AttachmentComponent->GetAttachToSocket();
		//if (!AttachmentComponent->GetDefaultAttachment())
		{
			if (AActor* Attachment = AttachmentComponent->GetAttachment())
			{
				AttachmentData.ActorClass = Attachment->GetClass();
			}
		}
		Data.Attachments.Emplace(AttachmentData);
	}
	FJsonObjectConverter::UStructToJsonObjectString(Data, SerializedString);
	return Data;
}
