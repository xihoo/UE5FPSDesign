// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "DataAssets/SKGPDARangeFinderSettings.h"
#include "Components/ActorComponent.h"
#include "SKGRangeFinderComponent.generated.h"

class UTextRenderComponent;
class UMeshComponent;
struct FHitResult;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRanged, float, Distance);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKGRANGEFINDER_API USKGRangeFinderComponent : public UActorComponent, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USKGRangeFinderComponent();
	UFUNCTION(BlueprintPure, Category = "SKGShooterFrameworkStatics|Getters")
	static USKGRangeFinderComponent* GetRangeFinderComponent(const AActor* Actor);
	// The mesh used for the range finder
	UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder|Initialize")
	FName RangeFinderMeshName {"StaticMesh"};
	// The socket to be used for performing the trace from
	UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder|Initialize")
	FName RangeFinderLaserSocketName {"S_Laser"};
	// Optional, but if set the distance will auto be applied as text to the found component
	UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder|Initialize")
	FName TextRenderComponentName {"TextRenderComponent"};
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGRangeFinder|Initialize")
	FGameplayTagContainer GameplayTags;
	UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder|Settings")
	TObjectPtr<USKGPDARangeFinderSettings> RangeFinderSettingsDataAsset;

protected:
	float MinTraceDistance {0.0f};
	// Deprecated, Utilize Range Finder Settings instead
	float MaxTraceDistance {100000.0f};
	// The symbols that gets appended to the end of the optional text render component and GetRangeAsString. // Deprecated, Utilize Range Finder Settings instead
	FSKGMeasurementSymbols MeasurementSymbols;
	// If false, will use feet and centimeter. // Deprecated, Utilize Range Finder Settings instead
	bool bUseInYardMeter {true};
	// If false it will use imperial units. // Deprecated, Utilize Range Finder Settings instead
	bool bUseMetric {true};
	// If true the range will be gotten automatically at the given interval. // Deprecated, Utilize Range Finder Settings instead
	bool bAutoRange {false};
	// Time between ranging. // Deprecated, Utilize Range Finder Settings instead
	float AutoRangeInterval {1.0f};

	UPROPERTY(BlueprintGetter = GetRangeFinderMesh, Category = "SKGRangeFinder|Mesh")
	TObjectPtr<UMeshComponent> RangeFinderMesh;
	UPROPERTY(BlueprintGetter = GetTextRenderComponent, Category = "SKGRangeFinder|Mesh")
	TObjectPtr<UTextRenderComponent> TextRenderComponent;
	
	ECollisionChannel RangeFinderCollisionChannel {ECC_Visibility};
	
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTags; }
	bool PerformTrace(FHitResult& HitResult) const;
	float ConvertDistance(float Distance) const;
	FString ConstructMeasurementString(const float Distance) const;
	
public:
	void InitializeComponentFromData();
	FORCEINLINE bool HasAuthority() const { return GetOwnerRole() == ROLE_Authority; }
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "SKGRangeFinder|Range")
	float GetRange() const;
	UFUNCTION(BlueprintCallable, Category = "SKGRangeFinder|Range")
	FString GetRangeAsString() const;
	
	UFUNCTION(BlueprintGetter)
	UMeshComponent* GetRangeFinderMesh() const { return RangeFinderMesh; }
	UFUNCTION(BlueprintGetter)
	UTextRenderComponent* GetTextRenderComponent() const { return TextRenderComponent; }

	UPROPERTY(BlueprintAssignable, Category = "SKGRangeFinder|Events")
	FOnRanged OnRanged;
};
