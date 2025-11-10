// Copyright 2023, Dakota Dawe, All rights reserved


#include "Components/SKGMuzzleComponent.h"
#include "Subsystems/SKGMuzzleWorldSubsystem.h"
#include "DataAssets/Initialize/SKGPDAMuzzleInitialize.h"

#include "Components/MeshComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

constexpr double MRADAdjustment {0.0572958};
constexpr double MOAAdjustment {0.01666667};


USKGMuzzleComponent::USKGMuzzleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

USKGMuzzleComponent* USKGMuzzleComponent::GetMuzzleComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<USKGMuzzleComponent>() : nullptr;
}

void USKGMuzzleComponent::InitializeMuzzleComponent()
{
	InitializeComponentFromData();
	if (!MuzzleSettingsDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("SKGMuzzleComponent on Actor: %s is NOT using the MuzzleSettingsDataAsset for configuration. Please switch to the new system before updating as it will be mandatory in a future version"), *GetOwner()->GetName());
	}
	SetupComponents();
}

void USKGMuzzleComponent::BeginPlay()
{
	Super::BeginPlay();

}

void USKGMuzzleComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	if (bAutoInitialize)
	{
		InitializeMuzzleComponent();
	}
}

void USKGMuzzleComponent::InitializeComponentFromData()
{
	if (InitializationSettingsDataAsset)
	{
		if (!bOverrideComponentNames)
		{
			MuzzleMeshComponentName = InitializationSettingsDataAsset->MuzzleMeshComponentName;
			MuzzleSocketName = InitializationSettingsDataAsset->MuzzleSocketName;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InitializationSettingsDataAsset NOT set on Actor: %s on Component: %s Please utilize this data asset as it will be mandatory moving forwards"), *GetOwner()->GetName(), *GetName());
	}
	
	if (MuzzleSettingsDataAsset)
	{
		MuzzleTag = MuzzleSettingsDataAsset->MuzzleTag;
		GameplayTags = MuzzleSettingsDataAsset->GameplayTags;
		bUseMuzzleTemperatureSystem = MuzzleSettingsDataAsset->bUseMuzzleTemperatureSystem;
		MaxMuzzleTemperatureFahrenheit = MuzzleSettingsDataAsset->MaxMuzzleTemperatureFahrenheit;
		MuzzleStartGlowTemperature = MuzzleSettingsDataAsset->MuzzleStartGlowTemperature;
		IncreaseMuzzleTemperatureAmountFahrenheit = MuzzleSettingsDataAsset->IncreaseMuzzleTemperatureAmountFahrenheit;
		DecreaseMuzzleTemperatureAmountPerTick = MuzzleSettingsDataAsset->DecreaseMuzzleTemperatureAmountPerTick;
	}
}

void USKGMuzzleComponent::SetupComponents()
{
	ensureAlwaysMsgf(MuzzleMeshComponentName != NAME_None, TEXT("Muzzle Mesh Component Name must be valid on Actor: %s on the SKGMuzzleComponent: %s"), *GetOwner()->GetName(), *GetName());
	ensureAlwaysMsgf(MuzzleSocketName != NAME_None, TEXT("Muzzle Socket Name must be valid on Actor: %s on the SKGMuzzleComponent: %s"), *GetOwner()->GetName(), *GetName());
	ensureAlwaysMsgf(MuzzleTag.IsValid(), TEXT("Muzzle Tag must be valid on Actor: %s on the SKGMuzzleComponent: %s"), *GetOwner()->GetName(), *GetName());

	for (UActorComponent* Component : GetOwner()->GetComponents())
	{
		if (Component && Component->GetFName().IsEqual(MuzzleMeshComponentName))
		{
			MuzzleMesh = Cast<UMeshComponent>(Component);;
		}
	}

	ensureAlwaysMsgf(MuzzleMesh, TEXT("MuzzleMesh NOT found on Actor: %s on the SKGMuzzleComponent: %s"), *GetOwner()->GetName(), *GetName());
	if (MuzzleMesh)
	{
		ensureAlwaysMsgf(MuzzleMesh->DoesSocketExist(MuzzleSocketName), TEXT("Socket: %s does NOT exist on Actor: %s on the SKGMuzzleComponent: %s"), *MuzzleSocketName.ToString(), *GetOwner()->GetName(), *GetName());
	}
}

void USKGMuzzleComponent::CoolMuzzle(float DeltaTime)
{
	CurrentMuzzleTemperature -= DecreaseMuzzleTemperatureAmountPerTick * DeltaTime;
	CurrentMuzzleTemperature = FMath::Clamp(CurrentMuzzleTemperature, 0.0f, MaxMuzzleTemperatureFahrenheit);
	OnMuzzleTemperatureChanged.Broadcast(CurrentMuzzleTemperature);
	if (CurrentMuzzleTemperature == 0.0f)
	{
		OnMuzzleCooled.Broadcast();
		GetWorld()->GetSubsystem<USKGMuzzleWorldSubsystem>()->UnregisterComponent(this);
		bRegisteredToSubsystem = false;
	}
}

void USKGMuzzleComponent::ShotPerformed()
{
	if (bUseMuzzleTemperatureSystem)
	{
		CurrentMuzzleTemperature += IncreaseMuzzleTemperatureAmountFahrenheit;
		OnMuzzleTemperatureChanged.Broadcast(CurrentMuzzleTemperature);
		if (!bRegisteredToSubsystem)
		{
			GetWorld()->GetSubsystem<USKGMuzzleWorldSubsystem>()->RegisterComponent(this);
			bRegisteredToSubsystem = true;
		}
	}
}

float USKGMuzzleComponent::GetMuzzleTemperatureNormalized() const
{
	return FMath::Clamp(CurrentMuzzleTemperature / MaxMuzzleTemperatureFahrenheit, 0.0f, 1.0f);
}

float USKGMuzzleComponent::GetMuzzleTemperatureNormalizedWithMinimumGlowTemperature() const
{
	return FMath::Clamp((CurrentMuzzleTemperature - MuzzleStartGlowTemperature) / (MaxMuzzleTemperatureFahrenheit - MuzzleStartGlowTemperature), 0.0f, 1.0f);
}

FTransform USKGMuzzleComponent::GetMuzzleTransform() const
{
	if (MuzzleMesh)
	{
		return MuzzleMesh->GetSocketTransform(MuzzleSocketName);
	}
	return FTransform();
}

FTransform USKGMuzzleComponent::GetMuzzleTransformRelative(const UPrimitiveComponent* ComponentRelativeTo) const
{
	if (ComponentRelativeTo)
	{
		return GetMuzzleTransform().GetRelativeTransform(ComponentRelativeTo->GetComponentTransform());
	}
	return FTransform();
}

FSKGMuzzleTransform USKGMuzzleComponent::GetMuzzleProjectileTransformCompensated(float ZeroDistanceMeters, float MOA, const FTransform& AimTransform) const
{
	ZeroDistanceMeters = FMath::Clamp(ZeroDistanceMeters * 100.0f, 0.0f, 10000000.0f);
	MOA *= MOAAdjustment;
	float AdjustedMOA = MOA;
	
	const FVector SightZeroDistanceLocation = AimTransform.GetTranslation() + AimTransform.GetRotation().Vector() * ZeroDistanceMeters;
	FRotator MuzzleLookAtSightRotation = UKismetMathLibrary::FindLookAtRotation(GetMuzzleTransform().GetTranslation(), SightZeroDistanceLocation);
	
	AdjustedMOA = FMath::RandRange(-MOA, MOA) * 0.5f;
	MuzzleLookAtSightRotation.Pitch += AdjustedMOA * 0.6f; // Multiply by 0.6 for more realistic consistency
	AdjustedMOA = FMath::RandRange(-MOA, MOA) * 0.5f;
	MuzzleLookAtSightRotation.Yaw += AdjustedMOA;

	FTransform MuzzleTransform = GetMuzzleTransform();
	MuzzleTransform.SetRotation(MuzzleLookAtSightRotation.Quaternion());
	return MuzzleTransform;
}

FSKGMuzzleTransform USKGMuzzleComponent::GetMuzzleProjectileTransform(float MOA) const
{
	MOA *= MOAAdjustment;
	float AdjustedMOA = MOA;
	FTransform MuzzleTransform = GetMuzzleTransform();
	FRotator MuzzleRotation = MuzzleTransform.Rotator();
	
	AdjustedMOA = FMath::RandRange(-MOA, MOA) * 0.5f;
	MuzzleRotation.Pitch += AdjustedMOA * 0.6f; // Multiply by 0.6 for more realistic consistency
	AdjustedMOA = FMath::RandRange(-MOA, MOA) * 0.5f;
	MuzzleRotation.Yaw += AdjustedMOA;

	MuzzleTransform.SetRotation(MuzzleRotation.Quaternion());
	return MuzzleTransform;
}