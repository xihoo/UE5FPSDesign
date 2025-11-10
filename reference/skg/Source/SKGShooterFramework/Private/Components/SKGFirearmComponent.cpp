// Copyright 2023, Dakota Dawe, All rights reserved


#include "Components/SKGFirearmComponent.h"
#include "Components/SKGAttachmentManagerComponent.h"
#include "Components/SKGAttachmentComponent.h"
#include "Components/SKGOffhandIKComponent.h"
#include "Components/SKGLightLaserComponent.h"
#include "Components/SKGProceduralAnimComponent.h"
#include "Components/SKGMuzzleComponent.h"
#include "Components/SKGOpticComponent.h"
#include "Components/SKGFirearmAttachmentStatComponent.h"
#include "Components/SKGStockComponent.h"
#include "Subsystems/SKGProjectileWorldSubsystem.h"

#include "GameFramework/Actor.h"
#include "Components/MeshComponent.h"
#include "Engine/World.h"
#include "GameplayTagAssetInterface.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Statics/SKGAttachmentHelpers.h"

DECLARE_CYCLE_STAT(TEXT("GetProceduralData"), STAT_SKGGetProceduralData, STATGROUP_SKGShooterFrameworkFirearmComponent);
DECLARE_CYCLE_STAT(TEXT("GetProceduralDataCurrentProceduralComponent"), STAT_SKGGetProceduralDataCurrentProceduralComponent, STATGROUP_SKGShooterFrameworkFirearmComponent);

USKGFirearmComponent::USKGFirearmComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

USKGFirearmComponent* USKGFirearmComponent::GetFirearmComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<USKGFirearmComponent>() : nullptr;
}

USKGFirearmComponent* USKGFirearmComponent::GetParentFirearmComponent(const AActor* Actor)
{
	if (Actor)
	{
		for (const AActor* ParentActor = Actor->GetOwner(); ParentActor; ParentActor = ParentActor->GetOwner())
		{
			if (USKGFirearmComponent* FirearmComponent = GetFirearmComponent(ParentActor))
			{
				return FirearmComponent;
			}
		}
	}
	return nullptr;
}

AActor* USKGFirearmComponent::GetParentWithFirearmComponent(const AActor* Actor)
{
	const USKGFirearmComponent* ParentFirearmComponent = GetParentFirearmComponent(Actor);
	return ParentFirearmComponent ? ParentFirearmComponent->GetOwner() : nullptr;
}

void USKGFirearmComponent::InitializeFirearmComponent()
{
	InitializeComponentFromData();
	
	if (FirearmStatsDataAsset && FirearmStats == FSKGFirearmStats())
	{
		FirearmStats.SetFirearmDefaultStats(FirearmStatsDataAsset);
	}
	
	SetupComponents();

	if (!FirearmCollisionSettingsDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("USKGFirearmComponent on Actor: %s is NOT using the FirearmCollisionSettingsDataAsset for configuration. Please switch to the new system before updating as it will be mandatory in a future version"), *GetOwner()->GetName());
	}
	
	SetInitialProceduralData();
	bIsInitialized = true;
}

void USKGFirearmComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoInitialize)
	{
		InitializeFirearmComponent();
	}
}

void USKGFirearmComponent::SetInitialProceduralData()
{	
	if (FirearmProceduralAnimComponent)
	{
		// This is to recache procedural settings whenever the procedural anim component receives an update/change
		FirearmProceduralAnimComponent->OnProceduralDataAssetsChanged.AddUniqueDynamic(this, &USKGFirearmComponent::SetInitialProceduralData);
		ProceduralAnimData.BasePoseOffset = FirearmProceduralAnimComponent->GetBasePoseOffset();
		ProceduralAnimData.ThirdPersonAimingOffset = FirearmProceduralAnimComponent->GetThirdPersonAimingOffset(true);
		ProceduralAnimData.CycleAimingPointSettings = FirearmProceduralAnimComponent->GetCycleAimingPointSettings();
		ProceduralAnimData.MovementSwaySettings = FirearmProceduralAnimComponent->GetMovementSwaySettings();
		ProceduralAnimData.MovementLagSettings = FirearmProceduralAnimComponent->GetMovementLagSettings();
		ProceduralAnimData.RotationLagSettings = FirearmProceduralAnimComponent->GetRotationLagSettings();
		ProceduralAnimData.DeadzoneSettings = FirearmProceduralAnimComponent->GetDeadzoneSettings();
		ProceduralAnimData.RecoilSettings = FirearmProceduralAnimComponent->GetRecoilSettings();

		FSKGProceduralStats ProceduralStats;
		ProceduralStats.AimInterpolationRate = FirearmProceduralAnimComponent->GetProceduralAimingSettings().DefaultAimingSpeed;
		ProceduralStats.CycleAimingPointSpringInterpSettings = FirearmProceduralAnimComponent->GetCycleAimingPointSettings().SpringInterpSettings;
		ProceduralStats.MovementLagSpringInterpSettings = FirearmProceduralAnimComponent->GetMovementLagSettings().SpringInterpSettings;
		ProceduralStats.MovementLagInterpSetting = FirearmProceduralAnimComponent->GetMovementLagSettings().InterpSpeed;
		ProceduralStats.RotationLagSpringInterpSettings = FirearmProceduralAnimComponent->GetRotationLagSettings().SpringInterpSettings;
		ProceduralStats.RotationLagInterpSettings = FirearmProceduralAnimComponent->GetRotationLagSettings().InterpSettings;
		ProceduralStats.ControlRotationRecoilMultipliers = FVector::OneVector;
		ProceduralStats.RecoilLocationMultipliers = FVector::OneVector;
		ProceduralStats.RecoilRotationMultipliers = FVector::OneVector;
		ProceduralAnimData.ProceduralStats = ProceduralStats;
	}

	ProceduralAnimData.FirearmCollisionSettings.bUseFirearmCollision = FirearmCollisionSettings.bUseFirearmCollision;
	ProceduralAnimData.FirearmCollisionSettings.CollisionStopAimingDistance = FirearmCollisionSettings.StopAimingDistance;
	ProceduralAnimData.FirearmCollisionSettings.PoseLocation = FirearmCollisionSettings.PoseLocationCurve;
	ProceduralAnimData.FirearmCollisionSettings.PoseRotation = FirearmCollisionSettings.PoseRotationCurve;
	ProceduralAnimData.FirearmCollisionSettings.PoseScale = FirearmCollisionSettings.PoseScale;
	ProceduralAnimData.FirearmCollisionSettings.PoseLocationInterpSpeed = FirearmCollisionSettings.PoseLocationInterpSpeed;
	ProceduralAnimData.FirearmCollisionSettings.PoseRotationInterpSpeed = FirearmCollisionSettings.PoseRotationInterpSpeed;
	ProceduralAnimData.FirearmCollisionSettings.TraceDiameter = FirearmCollisionSettings.TraceDiameter;
}

void USKGFirearmComponent::InitializeComponentFromData()
{
	if (FirearmCollisionSettingsDataAsset)
	{
		FirearmCollisionSettings = FirearmCollisionSettingsDataAsset->FirearmCollisionSettings;
	}
}

void USKGFirearmComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	
	FDoRepLifetimeParams ParamsOwnerOnly;
	ParamsOwnerOnly.bIsPushBased = true;
	ParamsOwnerOnly.Condition = COND_OwnerOnly;

	DOREPLIFETIME_WITH_PARAMS_FAST(USKGFirearmComponent, CurrentProceduralAnimComponent, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGFirearmComponent, CurrentOffhandIKComponent, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGFirearmComponent, CurrentMuzzleComponent, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGFirearmComponent, CurrentOpticComponent, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGFirearmComponent, CurrentStockComponent, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGFirearmComponent, FirearmStats, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGFirearmComponent, MuzzleComponents, Params);
	
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGFirearmComponent, ProceduralAnimComponents, ParamsOwnerOnly);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGFirearmComponent, LightLaserComponents, ParamsOwnerOnly);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGFirearmComponent, OffhandIKComponents, ParamsOwnerOnly);
	DOREPLIFETIME_WITH_PARAMS_FAST(USKGFirearmComponent, AttachmentStatComponents, ParamsOwnerOnly);
}

void USKGFirearmComponent::SetupComponents()
{
	//ensureAlwaysMsgf(!AttachmentManagerComponentName.IsEqual(NAME_None), TEXT("Attachment Manager Component Name must be valid on Actor: %s in the SKGFirearmComponent: %s"), *GetOwner()->GetName(), *GetName());
	ensureAlwaysMsgf(!FirearmMeshComponentName.IsEqual(NAME_None), TEXT("Firearm Mesh Component Name must be valid on Actor: %s in the SKGFirearmComponent: %s"), *GetOwner()->GetName(), *GetName());

	TInlineComponentArray<UActorComponent*> FirearmsComponents(GetOwner());
	for (UActorComponent* Component : FirearmsComponents)
	{
		if (Component->GetFName().IsEqual(AttachmentManagerComponentName))
		{
			if (USKGAttachmentManagerComponent* FoundAttachmentManager = Cast<USKGAttachmentManagerComponent>(Component))
			{
				AttachmentManager = FoundAttachmentManager;
				if (HasAuthority())
				{
					FoundAttachmentManager->OnAttachmentComponentAttachmentAdded.AddDynamic(this, &USKGFirearmComponent::OnAttachmentAdded);
					FoundAttachmentManager->OnAttachmentComponentAttachmentRemoved.AddDynamic(this, &USKGFirearmComponent::OnAttachmentRemoved);
					for (const FSKGAttachmentComponentItem& AttachmentComponentItem : FoundAttachmentManager->GetAttachmentComponents())
					{
						OnAttachmentAdded(AttachmentComponentItem.AttachmentComponent->GetAttachment());
					}
				}
			}
			else if (Component->GetFName().IsEqual(FirearmMeshComponentName))
			{
				FirearmMesh = Cast<UMeshComponent>(Component);
			}
		}
		else if (Component->GetFName().IsEqual(FirearmMeshComponentName))
		{
			FirearmMesh = Cast<UMeshComponent>(Component);
		}
	}
	
	//checkf(AttachmentManager, TEXT("Attachment Manager not found on Actor: %s. Make sure to add this component and that the name: %s is correct"), *GetOwner()->GetName(), *AttachmentManagerComponentName.ToString());
	checkf(FirearmMesh, TEXT("Firearm Mesh not found on Actor: %s. Make sure Firearm Mesh Component Name matches, make sure that the name: %s is correct"), *GetOwner()->GetName(), *FirearmMeshComponentName.ToString());

	for (UActorComponent* Component : FirearmsComponents)
	{
		if (USKGProceduralAnimComponent* ProceduralComponent = Cast<USKGProceduralAnimComponent>(Component))
		{
			FirearmProceduralAnimComponent = ProceduralComponent;
			if (HasAuthority() && !CurrentProceduralAnimComponent)
			{
				if (!CurrentProceduralAnimComponent)
				{
					CurrentProceduralAnimComponent = FirearmProceduralAnimComponent;
					MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentProceduralAnimComponent, this);
					OnRep_CurrentProceduralAnimComponent();
				}
			}
		}
		else if (USKGOffhandIKComponent* OffhandIKComponent = Cast<USKGOffhandIKComponent>(Component))
		{
			FirearmOffhandIKComponent = OffhandIKComponent;
			if (HasAuthority())
			{
				if (!CurrentOffhandIKComponent)
				{
					CurrentOffhandIKComponent = FirearmOffhandIKComponent;
					MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentOffhandIKComponent, this);
					OnRep_CurrentOffhandIKComponent();
				}
			}
		}
		else if (USKGMuzzleComponent* MuzzleComponent = Cast<USKGMuzzleComponent>(Component))
		{
			FirearmMuzzleComponent = MuzzleComponent;
			if (HasAuthority())
			{
				if (!CurrentMuzzleComponent)
				{
					CurrentMuzzleComponent = FirearmMuzzleComponent;
					MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentMuzzleComponent, this);
					OnRep_CurrentMuzzleComponent();
				}
			}
		}
		else if (USKGStockComponent* StockComponent = Cast<USKGStockComponent>(Component))
		{
			CurrentStockComponent = StockComponent;
			MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentStockComponent, this);
			OnRep_CurrentStockComponent();
			if (const AActor* Stock = CurrentStockComponent->GetOwner())
			{
				TArray<USKGAttachmentComponent*> StockOwnerAttachmentComponents = USKGAttachmentHelpers::GetAttachmentComponents(Stock);
				for (USKGAttachmentComponent* AttachmentComponent : StockOwnerAttachmentComponents)
				{
					if (AttachmentComponent && AttachmentComponent->GetAttachment() == Stock)
					{
						AttachmentComponent->OnOffsetChanged.AddDynamic(this, &USKGFirearmComponent::OnStockOffsetChanged);
						break;
					}
				}
			}
		}
	}
}

bool USKGFirearmComponent::SetOpticComponent()
{
	bool bChanged = false;
	if (CurrentProceduralAnimComponent)
	{
		USKGOpticComponent* NewOpticComponent = USKGOpticComponent::GetOpticComponent(CurrentProceduralAnimComponent->GetOwner());
		if (NewOpticComponent != CurrentOpticComponent)
		{
			CurrentOpticComponent = NewOpticComponent;
			bChanged = true;
		}
	}
	return bChanged;
}

void USKGFirearmComponent::AddFirearmAttachmentStats_Implementation(USKGFirearmAttachmentStatComponent* StatComponent)
{
	FirearmStats += StatComponent;
}

void USKGFirearmComponent::RemoveFirearmAttachmentStats_Implementation(USKGFirearmAttachmentStatComponent* StatComponent)
{
	FirearmStats -= StatComponent;
}

void USKGFirearmComponent::CalculateProceduralValues_Implementation()
{
	if (FirearmProceduralAnimComponent)
	{
		FSKGProceduralStats ProceduralStats;
		ProceduralStats.AimInterpolationRate = FirearmProceduralAnimComponent->GetProceduralAimingSettings().DefaultAimingSpeed;
		ProceduralStats.CycleAimingPointSpringInterpSettings = FirearmProceduralAnimComponent->GetCycleAimingPointSettings().SpringInterpSettings;
		ProceduralStats.MovementLagSpringInterpSettings = FirearmProceduralAnimComponent->GetMovementLagSettings().SpringInterpSettings;
		ProceduralStats.MovementLagInterpSetting = FirearmProceduralAnimComponent->GetMovementLagSettings().InterpSpeed;
		ProceduralStats.RotationLagSpringInterpSettings = FirearmProceduralAnimComponent->GetRotationLagSettings().SpringInterpSettings;
		ProceduralStats.RotationLagInterpSettings = FirearmProceduralAnimComponent->GetRotationLagSettings().InterpSettings;
		ProceduralStats.ControlRotationRecoilMultipliers = FVector::OneVector;
		ProceduralStats.RecoilLocationMultipliers = FVector::OneVector;
		ProceduralStats.RecoilRotationMultipliers = FVector::OneVector;

		SetProceduralStats(ProceduralStats);
		CalculateProceduralStats.Broadcast(ProceduralAnimData.ProceduralStats);
	}
}

void USKGFirearmComponent::OnAttachmentAdded(AActor* Attachment)
{
	if (Attachment)
	{
		TInlineComponentArray<UActorComponent*> Components(Attachment);
		for (UActorComponent* Component : Components)
		{
			if (USKGProceduralAnimComponent* ProceduralAnimComponent = Cast<USKGProceduralAnimComponent>(Component))
			{
				ProceduralAnimComponents.Add(ProceduralAnimComponent);
				SetBestProceduralAnimComponent();
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, ProceduralAnimComponents, this);
				OnRep_ProceduralAnimComponents();
			}
			else if (USKGLightLaserComponent* LightLaserComponent = Cast<USKGLightLaserComponent>(Component))
			{
				LightLaserComponents.Add(LightLaserComponent);
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, LightLaserComponents, this);
				OnRep_LightLaserComponents();
			}
			else if (USKGOffhandIKComponent* OffhandIKComponent = Cast<USKGOffhandIKComponent>(Component))
			{
				OffhandIKComponents.Add(OffhandIKComponent);
				SetBestOffhandIKComponent();
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, OffhandIKComponents, this);
				OnRep_OffhandIKComponents();
			}
			else if (USKGMuzzleComponent* MuzzleComponent = Cast<USKGMuzzleComponent>(Component))
			{
				MuzzleComponents.Add(MuzzleComponent);
				SetBestMuzzleComponent();
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, MuzzleComponents, this);
				OnRep_MuzzleComponents();
			}
			else if (USKGFirearmAttachmentStatComponent* AttachmentStatComponent = Cast<USKGFirearmAttachmentStatComponent>(Component))
			{
				AttachmentStatComponents.Add(AttachmentStatComponent);
				OnRep_AttachmentStatComponents();
				AddFirearmAttachmentStats(AttachmentStatComponent);
				OnRep_FirearmStats();
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, FirearmStats, this);
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, AttachmentStatComponents, this);
			}
			else if (USKGStockComponent* StockComponent = Cast<USKGStockComponent>(Component))
			{
				CurrentStockComponent = StockComponent;
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentStockComponent, this);
				OnRep_CurrentStockComponent();
				if (const AActor* Stock = CurrentStockComponent->GetOwner())
				{
					if (const AActor* StockOwner = Stock->GetOwner())
					{
						TArray<USKGAttachmentComponent*> StockOwnerAttachmentComponents = USKGAttachmentHelpers::GetAttachmentComponents(StockOwner);
						for (USKGAttachmentComponent* AttachmentComponent : StockOwnerAttachmentComponents)
						{
							if (AttachmentComponent && AttachmentComponent->GetAttachment() == Stock)
							{
								CurrentStockComponent->SetOffset(AttachmentComponent->GetAttachmentOffset());
								AttachmentComponent->OnOffsetChanged.AddDynamic(this, &USKGFirearmComponent::OnStockOffsetChanged);
								break;
							}
						}
					}
				}
			}
		}
	}
}

void USKGFirearmComponent::OnAttachmentRemoved(AActor* Attachment)
{
	if (Attachment)
	{
		TInlineComponentArray<UActorComponent*> Components(Attachment);
		for (UActorComponent* Component : Components)
		{
			if (USKGProceduralAnimComponent* ProceduralAnimComponent = Cast<USKGProceduralAnimComponent>(Component))
			{
				if (CurrentProceduralAnimComponent == ProceduralAnimComponent)
            	{
            		CurrentProceduralAnimComponent = nullptr;
					MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentProceduralAnimComponent, this);
            	}
				ProceduralAnimComponents.Remove(ProceduralAnimComponent);
				SetBestProceduralAnimComponent();
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, ProceduralAnimComponents, this);
				OnRep_ProceduralAnimComponents();
			}
			else if (USKGLightLaserComponent* LightLaserComponent = Cast<USKGLightLaserComponent>(Component))
			{
				LightLaserComponents.Remove(LightLaserComponent);
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, LightLaserComponents, this);
				OnRep_LightLaserComponents();
			}
			else if (USKGOffhandIKComponent* OffhandIKComponent = Cast<USKGOffhandIKComponent>(Component))
			{
				OffhandIKComponents.Remove(OffhandIKComponent);
				SetBestOffhandIKComponent();
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, OffhandIKComponents, this);
				OnRep_LightLaserComponents();
			}
			else if (USKGMuzzleComponent* MuzzleComponent = Cast<USKGMuzzleComponent>(Component))
			{
				MuzzleComponents.Remove(MuzzleComponent);
				SetBestMuzzleComponent();
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, MuzzleComponents, this);
			}
			else if (USKGFirearmAttachmentStatComponent* AttachmentStatComponent = Cast<USKGFirearmAttachmentStatComponent>(Component))
			{
				AttachmentStatComponents.Remove(AttachmentStatComponent);
				OnRep_AttachmentStatComponents();
				RemoveFirearmAttachmentStats(AttachmentStatComponent);
				OnRep_FirearmStats();
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, FirearmStats, this);
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, AttachmentStatComponents, this);
			}
			else if (USKGStockComponent* StockComponent = Cast<USKGStockComponent>(Component))
			{
				if (CurrentStockComponent)
				{
					if (const AActor* Stock = CurrentStockComponent->GetOwner())
					{
						if (const AActor* StockOwner = Stock->GetOwner())
						{
							TArray<USKGAttachmentComponent*> StockOwnerAttachmentComponents = USKGAttachmentHelpers::GetAttachmentComponents(StockOwner);
							for (USKGAttachmentComponent* AttachmentComponent : StockOwnerAttachmentComponents)
							{
								if (AttachmentComponent && AttachmentComponent->GetAttachment() == Stock)
								{
									AttachmentComponent->OnOffsetChanged.RemoveDynamic(this, &USKGFirearmComponent::OnStockOffsetChanged);
									OnStockOffsetChanged(0);
									break;
								}
							}
						}
					}
				}
				CurrentStockComponent = nullptr;
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentStockComponent, this);
				OnRep_CurrentStockComponent();
			}
		}
	}
}

void USKGFirearmComponent::OnStockOffsetChanged(const float Offset)
{
	if (CurrentStockComponent)
	{
		CurrentStockComponent->SetOffset(Offset);
	}
}

bool USKGFirearmComponent::Server_SetAimingDevice_Validate(USKGProceduralAnimComponent* AnimComponent)
{
	return ProceduralAnimComponents.Contains(AnimComponent) || AnimComponent == FirearmProceduralAnimComponent;
}

void USKGFirearmComponent::Server_SetAimingDevice_Implementation(USKGProceduralAnimComponent* AnimComponent)
{
	CurrentProceduralAnimComponent = AnimComponent;
	MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentProceduralAnimComponent, this);
	OnRep_CurrentProceduralAnimComponent();
}

bool USKGFirearmComponent::SetupNewAimingDevice(USKGProceduralAnimComponent* AnimComponent, const bool bIsAiming)
{
	if (AnimComponent && CurrentProceduralAnimComponent != AnimComponent)
	{
		CurrentProceduralAnimComponent = AnimComponent;
		if (CurrentOpticComponent)
		{
			CurrentOpticComponent->StoppedAiming();
		}
		const bool bNewOpticComponent = SetOpticComponent();
		if (bNewOpticComponent)
		{
			if (CurrentOpticComponent && bIsAiming)
			{
				CurrentOpticComponent->StartedAiming();
			}
			OnRep_CurrentOpticComponent();
		}
		OnRep_CurrentProceduralAnimComponent();

		if (HasAuthority())
		{
			if (bNewOpticComponent)
			{
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentOpticComponent, this);
			}
			MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentProceduralAnimComponent, this);
		}
		else
		{
			Server_SetAimingDevice(CurrentProceduralAnimComponent);
		}
		return true;
	}
	return false;
}

bool USKGFirearmComponent::CycleAimingDevice(bool bIsAiming)
{
	USKGProceduralAnimComponent* NewProceduralComponent = nullptr;
	if (!bIsPointAiming)
	{
		if (ProceduralAnimComponents.Num())
		{
			int32 TempProceduralAnimComponentIndex = FMath::Clamp(ProceduralAnimComponentIndex, 0, ProceduralAnimComponents.Num() - 1);
			for (int32 i = 0; i < ProceduralAnimComponents.Num(); ++i)
			{
				USKGProceduralAnimComponent* TempProceduralAnimComponent = ProceduralAnimComponents[TempProceduralAnimComponentIndex];
				if (TempProceduralAnimComponent && TempProceduralAnimComponent->CanAim() && TempProceduralAnimComponent != CurrentProceduralAnimComponent)
				{
					ProceduralAnimComponentIndex = TempProceduralAnimComponentIndex;
					break;
				}
				if (++TempProceduralAnimComponentIndex > ProceduralAnimComponents.Num() - 1)
				{
					TempProceduralAnimComponentIndex = 0;
				}
			}
			if (ProceduralAnimComponentIndex < ProceduralAnimComponents.Num())
			{
				NewProceduralComponent = ProceduralAnimComponents[ProceduralAnimComponentIndex];
			}
			
			if (NewProceduralComponent && !NewProceduralComponent->CanAim())
			{
				NewProceduralComponent = nullptr;
			}
		}
		else
		{
			NewProceduralComponent = FirearmProceduralAnimComponent;
		}
	}
	return SetupNewAimingDevice(NewProceduralComponent, bIsAiming);;
}

bool USKGFirearmComponent::SetAimingDevice(USKGProceduralAnimComponent* AnimComponent, const bool bIsAiming)
{
	if (!bIsPointAiming && AnimComponent)
	{
		const int32 Index = ProceduralAnimComponents.Find(AnimComponent);
		if (Index != INDEX_NONE)
		{
			ProceduralAnimComponentIndex = Index;
			return SetupNewAimingDevice(AnimComponent, bIsAiming);
		}
	}
	return false;
}

void USKGFirearmComponent::StartPointAiming(bool bRightHandDominant)
{
	if (FirearmProceduralAnimComponent && !bIsPointAiming)
	{
		if (FirearmProceduralAnimComponent->StartPointAiming(bRightHandDominant))
		{
			bIsPointAiming = true;
			if (FirearmProceduralAnimComponent != CurrentProceduralAnimComponent)
			{
				BeforePointAimProceduralAnimComponent = CurrentProceduralAnimComponent;
				CurrentProceduralAnimComponent = FirearmProceduralAnimComponent;
				OnRep_CurrentProceduralAnimComponent();
				if (HasAuthority())
				{
					MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentProceduralAnimComponent, this);
				}
				else
				{
					Server_SetAimingDevice(CurrentProceduralAnimComponent);
				}
				if (CurrentOpticComponent)
				{
					CurrentOpticComponent->StoppedAiming();
					CurrentOpticComponent = nullptr;
				}
			}
		}
	}
}

void USKGFirearmComponent::StopPointAiming(bool bIsAiming)
{
	const bool bCurrentIsPointAiming = bIsPointAiming;
	bIsPointAiming = false;
	if (IsValid(BeforePointAimProceduralAnimComponent) && bCurrentIsPointAiming)
	{
		if (CurrentProceduralAnimComponent != BeforePointAimProceduralAnimComponent)
		{
			CurrentProceduralAnimComponent = BeforePointAimProceduralAnimComponent;
			if (SetOpticComponent())
			{
				if (bIsAiming)
				{
					CurrentOpticComponent->StartedAiming();
				}
				if (HasAuthority())
				{
					MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentOpticComponent, this);
				}
			}
			
			OnRep_CurrentProceduralAnimComponent();

			if (HasAuthority())
			{
				MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentProceduralAnimComponent, this);
			}
			else
			{
				Server_SetAimingDevice(CurrentProceduralAnimComponent);
			}
		}
		
		CurrentProceduralAnimComponent->StopPointAiming();
	}
	else
	{
		CycleAimingDevice(bIsAiming);
	}
	if (CurrentProceduralAnimComponent == FirearmProceduralAnimComponent)
	{
		CurrentProceduralAnimComponent->StopPointAiming();
	}
	BeforePointAimProceduralAnimComponent = nullptr;
}

FTransform USKGFirearmComponent::GetAimSocketWorldTransform() const
{
	if (CurrentProceduralAnimComponent)
	{
		return CurrentProceduralAnimComponent->GetAimWorldTransform();
	}
	else if (FirearmProceduralAnimComponent)
	{
		return FirearmProceduralAnimComponent->GetAimWorldTransform();
	}
	return FTransform();
}

TArray<USKGOpticComponent*> USKGFirearmComponent::GetMagnifiers()
{
	TArray<USKGOpticComponent*> Magnifiers;
	for (const USKGProceduralAnimComponent* ProceduralAnimComponent : ProceduralAnimComponents)
	{
		USKGOpticComponent* OpticComponent = ProceduralAnimComponent ? USKGOpticComponent::GetOpticComponent(ProceduralAnimComponent->GetOwner()) : nullptr;
		if (OpticComponent && OpticComponent->IsMagnifier())
		{
			Magnifiers.Add(OpticComponent);
		}
	}
	return Magnifiers;
}

FGameplayTag USKGFirearmComponent::GetProceduralGameplayTag() const
{
	if (FirearmProceduralAnimComponent)
	{
		return FirearmProceduralAnimComponent->GetProceduralGameplayTag();
	}
	return FGameplayTag();
}

FSKGProceduralAnimInstanceData& USKGFirearmComponent::GetProceduralData(bool bIsAiming, bool bOffhandIKIsLeftHand)
{
	SCOPED_NAMED_EVENT(USKGFirearmComponentGetProceduralData, FColor::Blue);
	if (bIsInitialized)
	{
		if (bIsAiming)
		{
			SCOPE_CYCLE_COUNTER(STAT_SKGGetProceduralDataCurrentProceduralComponent);
			if (CurrentProceduralAnimComponent)
			{
				CacheAimingOffset();
				const FVector BaseOffset = CurrentProceduralAnimComponent == FirearmProceduralAnimComponent ? FVector::ZeroVector : CachedAimingOffset;
				CurrentProceduralAnimComponent->UpdateAimOffset(FirearmMesh, BaseOffset);
				ProceduralAnimData.AimOffset = CurrentProceduralAnimComponent->GetAimOffset();
			}
			/*else if (FirearmProceduralAnimComponent)
			{
				FirearmProceduralAnimComponent->UpdateAimOffset(FirearmMesh, CachedAimingOffset);
				ProceduralAnimData.AimOffset = FirearmProceduralAnimComponent->GetAimOffset();
			}*/

			if (bOffhandIKIsLeftHand != bOldOffhandIKIsLeftHand && FirearmProceduralAnimComponent)
			{
				bOldOffhandIKIsLeftHand = bOffhandIKIsLeftHand;
				ProceduralAnimData.ThirdPersonAimingOffset = FirearmProceduralAnimComponent->GetThirdPersonAimingOffset(bOffhandIKIsLeftHand);
			}
		}

		if (CurrentOffhandIKComponent)
		{
			CurrentOffhandIKComponent->UpdateOffhandIK(FirearmMesh, bOffhandIKIsLeftHand);
			ProceduralAnimData.OffhandIKOffset = CurrentOffhandIKComponent->GetOffhandIKOffset();
			ProceduralAnimData.OffhandIKPose = CurrentOffhandIKComponent->GetOffhandIKPose();
		}
		else if (FirearmOffhandIKComponent)
		{
			FirearmOffhandIKComponent->UpdateOffhandIK(FirearmMesh, bOffhandIKIsLeftHand);
			ProceduralAnimData.OffhandIKOffset = FirearmOffhandIKComponent->GetOffhandIKOffset();
			ProceduralAnimData.OffhandIKPose = FirearmOffhandIKComponent->GetOffhandIKPose();
		}
	
		if (CurrentMuzzleComponent)
		{
			ProceduralAnimData.FirearmCollisionSettings.MuzzleRelativeTransform = CurrentMuzzleComponent->GetMuzzleTransformRelative(FirearmMesh);
			ProceduralAnimData.FirearmCollisionSettings.RootTransform = FirearmMesh->GetComponentTransform();
			ProceduralAnimData.FirearmCollisionSettings.MuzzleTransform = CurrentMuzzleComponent->GetMuzzleTransform();
		}

		if (CurrentStockComponent)
		{
			ProceduralAnimData.LengthOfPull = CurrentStockComponent->GetLengthOfPull();
		}
		else
		{
			ProceduralAnimData.LengthOfPull = 0.0f;
		}

		if (AttachmentManager)
		{
			ProceduralAnimData.FirearmCollisionSettings.ActorsToIgnoreForTrace = AttachmentManager->GetAttachments();
		}
		ProceduralAnimData.FirearmCollisionSettings.ActorsToIgnoreForTrace.Add(GetOwner());
	}
	return ProceduralAnimData;
}

void USKGFirearmComponent::SetProceduralStats(const FSKGProceduralStats& ProceduralStatsData)
{
	ProceduralAnimData.ProceduralStats = ProceduralStatsData;
}

void USKGFirearmComponent::AddAttachment(AActor* Attachment)
{
	TArray<AActor*> AttachedActors {Attachment};
	Attachment->GetAttachedActors(AttachedActors, false, true);
	for (AActor* Actor : AttachedActors)
	{
		OnAttachmentAdded(Actor);
	}
}

void USKGFirearmComponent::RemoveAttachment(AActor* Attachment)
{
	TArray<AActor*> AttachedActors {Attachment};
	Attachment->GetAttachedActors(AttachedActors, false, true);
	for (AActor* Actor : AttachedActors)
	{
		OnAttachmentRemoved(Actor);
	}
}

void USKGFirearmComponent::Held()
{
	OnHeld.Broadcast();
}

void USKGFirearmComponent::ShotPerformed()
{
	for (USKGMuzzleComponent* MuzzleComponent : MuzzleComponents)
	{
		if (MuzzleComponent)
		{
			MuzzleComponent->ShotPerformed();
		}
	}
}

void USKGFirearmComponent::ZeroOpticsForZeroAtLocation(const FVector& Location)
{
	if (USKGProjectileWorldSubsystem* ProjectileWorldSubsystem = GetWorld()->GetSubsystem<USKGProjectileWorldSubsystem>())
	{
		const FTransform MuzzleTransform = GetMuzzleProjectileTransform(100.0f, 0.0f);
		for (const USKGProceduralAnimComponent* ProceduralAnimComponent : ProceduralAnimComponents)
		{
			if (ProceduralAnimComponent)
			{
				if (USKGOpticComponent* OpticComponent = USKGOpticComponent::GetOpticComponent(ProceduralAnimComponent->GetOwner()))
				{
					FRotator LookAtRotation;
					if (ProjectileWorldSubsystem->GetProjectileZeroAtLocation(LookAtRotation, Location, MuzzleTransform, ProceduralAnimComponent->GetAimWorldTransform()))
					{
						OpticComponent->ApplyLookAtRotationZero(LookAtRotation);
					}
				}
			}
		}
	}
}

FSKGMuzzleTransform USKGFirearmComponent::GetMuzzleProjectileTransform(float ZeroDistanceMeters, float MOA) const
{
	FSKGMuzzleTransform MuzzleTransform;
	/*FTransform AimTransform;
	if (CurrentProceduralAnimComponent)
	{
		AimTransform = CurrentProceduralAnimComponent->GetAimMuzzleTransform();
	}
	else if (FirearmProceduralAnimComponent)
	{
		AimTransform = FirearmProceduralAnimComponent->GetAimMuzzleTransform();
	}
	const FVector Start = AimTransform.GetLocation();
	FVector End = Start + AimTransform.Rotator().Vector() * 120000.0f;
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 10.0f, 0, 1.0f);*/
	if (CurrentMuzzleComponent)
	{
		MuzzleTransform = CurrentMuzzleComponent->GetMuzzleProjectileTransform(MOA);
	}
	else if (FirearmMuzzleComponent)
	{
		MuzzleTransform = FirearmMuzzleComponent->GetMuzzleProjectileTransform(MOA);
	}
	return MuzzleTransform;
}

TArray<FSKGMuzzleTransform> USKGFirearmComponent::GetMuzzleProjectileTransforms(float ZeroDistanceMeters, float MOA,
	const int32 ProjectileCount) const
{
	TArray<FSKGMuzzleTransform> MuzzleTransforms;
	MuzzleTransforms.Reserve(ProjectileCount);
	for (int32 i = 0; i < ProjectileCount; ++i)
	{
		MuzzleTransforms.Add(GetMuzzleProjectileTransform(ZeroDistanceMeters, MOA));
	}
	return MuzzleTransforms;
}

FTransform USKGFirearmComponent::GetMuzzleTransform() const
{
	if (CurrentMuzzleComponent)
	{
		return CurrentMuzzleComponent->GetMuzzleTransform();
	}
	else if (FirearmMuzzleComponent)
	{
		return FirearmMuzzleComponent->GetMuzzleTransform();
	}
	return FTransform();
}

bool USKGFirearmComponent::IsSuppressed() const
{
	return GetCurrentMuzzleComponent() ? GetCurrentMuzzleComponent()->GetMuzzleTag() == SKGGAMEPLAYTAGS::MuzzleComponentSuppressor : false;
}

bool USKGFirearmComponent::GetPose(FGameplayTag Tag, FSKGToFromCurveSettings& PoseData)
{
	if (FirearmProceduralAnimComponent)
	{
		return FirearmProceduralAnimComponent->GetPose(Tag, PoseData);
	}
	return false;
}

void USKGFirearmComponent::CacheAimingOffset()
{
	if (CachedAimingOffset.Equals(-FVector::OneVector) && FirearmProceduralAnimComponent)
	{
		FirearmProceduralAnimComponent->UpdateAimOffset(FirearmMesh);
		CachedAimingOffset = FirearmProceduralAnimComponent->GetAimOffset().GetTranslation();
	}
}

void USKGFirearmComponent::SetBestMuzzleComponent()
{
	USKGMuzzleComponent* FoundMuzzle = nullptr;
	FGameplayTag CurrentFoundTag;
	for (USKGMuzzleComponent* Muzzle : MuzzleComponents)
	{
		FGameplayTag GameplayTag = Muzzle->GetMuzzleTag();
		if (GameplayTag == SKGGAMEPLAYTAGS::MuzzleComponentSuppressor)
		{
			FoundMuzzle = Muzzle;
			break;
		}
		if (GameplayTag == SKGGAMEPLAYTAGS::MuzzleComponentMuzzleDevice)
		{
			FoundMuzzle = Muzzle;
			CurrentFoundTag = SKGGAMEPLAYTAGS::MuzzleComponentMuzzleDevice;
		}
		else if (GameplayTag == SKGGAMEPLAYTAGS::MuzzleComponentBarrel && CurrentFoundTag != SKGGAMEPLAYTAGS::MuzzleComponentMuzzleDevice)
		{
			FoundMuzzle = Muzzle;
			CurrentFoundTag = SKGGAMEPLAYTAGS::MuzzleComponentBarrel;
		}
	}

	if (CurrentMuzzleComponent != FoundMuzzle)
	{
		CurrentMuzzleComponent = FoundMuzzle;
		MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentMuzzleComponent, this);
		OnRep_CurrentMuzzleComponent();
	}
}

void USKGFirearmComponent::SetBestOffhandIKComponent()
{
	USKGOffhandIKComponent* FoundOffhandIKComponent = nullptr;
	FGameplayTag CurrentFoundTag;
	for (USKGOffhandIKComponent* OffhandIKComponent : OffhandIKComponents)
	{
		if (const IGameplayTagAssetInterface* GameplayTagInterface = Cast<IGameplayTagAssetInterface>(OffhandIKComponent))
		{
			FGameplayTagContainer Container;
			GameplayTagInterface->GetOwnedGameplayTags(Container);
			if (GameplayTagInterface->HasMatchingGameplayTag(SKGGAMEPLAYTAGS::OffhandIKComponentForwardGrip))
			{
				FoundOffhandIKComponent = OffhandIKComponent;
				CurrentFoundTag = SKGGAMEPLAYTAGS::OffhandIKComponentForwardGrip;
				break;
			}
			if (GameplayTagInterface->HasMatchingGameplayTag(SKGGAMEPLAYTAGS::OffhandIKComponentHandguard))
			{
				FoundOffhandIKComponent = OffhandIKComponent;
				CurrentFoundTag = SKGGAMEPLAYTAGS::OffhandIKComponentHandguard;
			}
			else if (GameplayTagInterface->HasMatchingGameplayTag(SKGGAMEPLAYTAGS::OffhandIKComponentFirearm) && CurrentFoundTag != SKGGAMEPLAYTAGS::OffhandIKComponentHandguard)
			{
				FoundOffhandIKComponent = OffhandIKComponent;
				CurrentFoundTag = SKGGAMEPLAYTAGS::OffhandIKComponentFirearm;
			}
		}
	}
	if (!CurrentFoundTag.IsValid())
	{
		FoundOffhandIKComponent = FirearmOffhandIKComponent;
	}
	if (CurrentOffhandIKComponent != FoundOffhandIKComponent)
	{
		CurrentOffhandIKComponent = FoundOffhandIKComponent;
		MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentOffhandIKComponent, this);
		OnRep_CurrentOffhandIKComponent();
	}
}

void USKGFirearmComponent::SetBestProceduralAnimComponent()
{
	if (!CurrentProceduralAnimComponent || (CurrentProceduralAnimComponent == FirearmProceduralAnimComponent))
	{
		USKGProceduralAnimComponent* NewProceduralAnimComponent = nullptr;
		for (int32 i = 0; i < ProceduralAnimComponents.Num(); ++i)
		{
			USKGProceduralAnimComponent* ProceduralAnimComponent = ProceduralAnimComponents[i];
			if (ProceduralAnimComponent && ProceduralAnimComponent->CanAim())
			{
				NewProceduralAnimComponent = ProceduralAnimComponent;
				ProceduralAnimComponentIndex = i;
				break;
			}
		}
		
		if (NewProceduralAnimComponent)
		{
			CurrentProceduralAnimComponent = NewProceduralAnimComponent;
			MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentProceduralAnimComponent, this);
			OnRep_CurrentProceduralAnimComponent();
		}
		else
		{
			CurrentProceduralAnimComponent = FirearmProceduralAnimComponent;
			MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentProceduralAnimComponent, this);
			OnRep_CurrentProceduralAnimComponent();
		}
		
		if (SetOpticComponent())
		{
			MARK_PROPERTY_DIRTY_FROM_NAME(USKGFirearmComponent, CurrentOpticComponent, this);
			OnRep_CurrentOpticComponent();
		}
	}
}

void USKGFirearmComponent::OnRep_FirearmStats()
{
	OnFirearmStatsChanged.Broadcast(FirearmStats);

	CalculateProceduralValues();
}

void USKGFirearmComponent::OnRep_ProceduralAnimComponents()
{
	OnProceduralAnimComponentsUpdated.Broadcast();
}

void USKGFirearmComponent::OnRep_AttachmentStatComponents()
{
	
}

void USKGFirearmComponent::OnRep_CurrentProceduralAnimComponent()
{
	if (CurrentProceduralAnimComponent && FirearmMesh)
	{
		CurrentProceduralAnimComponent->UpdateAimOffset(FirearmMesh);
		ProceduralAnimData.AimOffset = CurrentProceduralAnimComponent->GetAimOffset();
		OnAimingDeviceCycled.Broadcast(CurrentProceduralAnimComponent);
	}
}

void USKGFirearmComponent::OnRep_CurrentMuzzleComponent()
{
	OnMuzzleComponentUpdated.Broadcast();
}
