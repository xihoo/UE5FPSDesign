// Copyright 2023, Dakota Dawe, All rights reserved


#include "Components/SKGFirearmAttachmentStatComponent.h"
#include "DataAssets/SKGPDAFirearmAttachmentStats.h"
#include "GameFramework/Actor.h"

USKGFirearmAttachmentStatComponent::USKGFirearmAttachmentStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

USKGFirearmAttachmentStatComponent* USKGFirearmAttachmentStatComponent::GetFirearmAttachmentStatComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<USKGFirearmAttachmentStatComponent>() : nullptr;
}

void USKGFirearmAttachmentStatComponent::BeginPlay()
{
	Super::BeginPlay();

}
