// Copyright 2024, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/CollisionProfile.h"
#include "DataAssets/SKGPDABase.h"
#include "SKGPDAFirearm.generated.h"

class USKGPDAProceduralOffsets;
class USKGPDACycleAimingPointSettings;
class USKGPDAProceduralAnimInitialize;
class USKGPDAOffhandIKInitialize;
class USKGPDAOffhandIKSettings;
class USKGAttachmentComponent;
class UGameplayEffect;
class USKGPDAMuzzleInitialize;
class USKGPDAMuzzleSettings;
class USKGPDADeadzoneSettings;
class USKGPDARecoilSettings;
class USKGPDARotationLagSettings;
class USKGPDAMovementLagSettings;
class USKGPDAMovementSwaySettings;
class USKGPDAAimingSettings;
class USKGPDAFirearmCollisionSettings;
class USKGPDAProceduralPoseSettings;
class UGameplayAbility;
class UNiagaraSystem;
class USkeletalMesh;


UCLASS()
class SKGSHOOTERFRAMEWORKACTORS_API USKGPDAFirearm : public USKGPDABase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary")
	FCollisionProfileName MeshCollisionProfile;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary", meta = (AssetBundles = "Visual"))
	TSoftObjectPtr<USkeletalMesh> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Procedural")
	TObjectPtr<USKGPDAProceduralAnimInitialize> ProceduralAnimInitializeSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Procedural")
	TObjectPtr<USKGPDAAimingSettings> AimingSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Procedural")
	TObjectPtr<USKGPDACycleAimingPointSettings> CycleAimingPointSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Procedural")
	TObjectPtr<USKGPDAProceduralOffsets> ProceduralOffsetsSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Procedural")
	TObjectPtr<USKGPDAMovementSwaySettings> MovementSwaySettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Procedural")
	TObjectPtr<USKGPDAMovementLagSettings> MovementLagSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Procedural")
	TObjectPtr<USKGPDARotationLagSettings> RotationLagSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Procedural")
	TObjectPtr<USKGPDARecoilSettings> RecoilSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Procedural")
	TObjectPtr<USKGPDADeadzoneSettings> DeadzoneSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Procedural")
	TObjectPtr<USKGPDAFirearmCollisionSettings> FirearmCollisionSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Procedural")
	TArray<TObjectPtr<USKGPDAProceduralPoseSettings>> Poses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Muzzle")
	bool bHasMuzzle {false};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Muzzle")
	TObjectPtr<USKGPDAMuzzleInitialize> MuzzleInitializeSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Muzzle")
	TObjectPtr<USKGPDAMuzzleSettings> MuzzleSettings;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|OffhandIK")
	bool bHasOffhandIKComponent {false};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|OffhandIK")
	TObjectPtr<USKGPDAOffhandIKInitialize> OffhandIKInitializeSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|OffhandIK")
	TObjectPtr<USKGPDAOffhandIKSettings> OffhandIKSettings;
};
