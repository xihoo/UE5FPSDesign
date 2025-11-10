// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SKGPDARangeFinderSettings.generated.h"

USTRUCT(BlueprintType)
struct FSKGMeasurementSymbols
{
	GENERATED_BODY()
	// The symbol that gets appended to the end of the optional text render component
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SKGRangeFinder|Settings")
	FString MeterSymbol {"m"};
	// The symbol that gets appended to the end of the optional text render component
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SKGRangeFinder|Settings")
	FString YardSymbol {"yd"};
	// The symbol that gets appended to the end of the optional text render component
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SKGRangeFinder|Settings")
	FString InchSymbol {"in"};
	// The symbol that gets appended to the end of the optional text render component
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SKGRangeFinder|Settings")
	FString CentimeterSymbol {"cm"};
};

UCLASS()
class SKGRANGEFINDER_API USKGPDARangeFinderSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGRangeFinder")
	FGameplayTagContainer GameplayTags;
	UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder")
	float MinTraceDistance {0.0f};
	UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder")
	float MaxTraceDistance {100000.0f};
	// The symbols that gets appended to the end of the optional text render component and GetRangeAsString
	UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder")
	FSKGMeasurementSymbols MeasurementSymbols;
	// If false, will use feet and centimeter
	UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder")
	bool bUseInYardMeter {true};
	// If false it will use imperial units
	UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder")
	bool bUseMetric {true};
	// If true the range will be gotten automatically at the given interval
	UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder")
	bool bAutoRange {false};
	// Time between ranging
	UPROPERTY(EditDefaultsOnly, Category = "SKGRangeFinder", meta = (EditCondition = "bAutoRange"))
	float AutoRangeInterval {1.0f};
};
