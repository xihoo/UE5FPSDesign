// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagAssetInterface.h"
#include "SKGFirearmAttachmentStatComponent.generated.h"


class USKGPDAFirearmAttachmentStats;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKGSTATS_API USKGFirearmAttachmentStatComponent : public UActorComponent, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USKGFirearmAttachmentStatComponent();
	UFUNCTION(BlueprintPure, Category = "SKGShooterFrameworkStatics|Getters")
	static USKGFirearmAttachmentStatComponent* GetFirearmAttachmentStatComponent(const AActor* Actor);
	virtual void BeginPlay() override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }
	FORCEINLINE bool HasAuthority() const { return GetOwnerRole() == ROLE_Authority; }
	UFUNCTION(BlueprintGetter)
	USKGPDAFirearmAttachmentStats* GetAttachmentStats() const { return AttachmentStatsDataAsset; }
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetAttachmentStats, Category = "SKGFirearmAttachmentStatComponent|Initialize")
	TObjectPtr<USKGPDAFirearmAttachmentStats> AttachmentStatsDataAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGFirearmAttachmentStatComponent|Initialize")
	FGameplayTagContainer GameplayTags;
};
