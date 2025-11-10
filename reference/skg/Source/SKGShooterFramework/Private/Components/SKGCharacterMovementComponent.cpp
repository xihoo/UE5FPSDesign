// Copyright 2022, Dakota Dawe, All rights reserved


#include "Components/SKGCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

USKGCharacterMovementComponent::USKGCharacterMovementComponent()
{
	MaxAcceleration = 1365.0f;
	BrakingFriction = 1.0f;
	MaxWalkSpeed = 150.0f;

	MaxWalkSpeedCrouched = 75.0f;
}

void USKGCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

FNetworkPredictionData_Client* USKGCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		USKGCharacterMovementComponent* MutableThis = const_cast<USKGCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_SKGSF(*this);
	}

	return ClientPredictionData;
}

void USKGCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

float USKGCharacterMovementComponent::GetMaxSpeed() const
{
	switch(MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		return bWantsToSprint ? MaxSprintSpeed : (IsCrouching() ? MaxWalkSpeedCrouched : MaxWalkSpeed);
	case MOVE_Falling:
		return MaxWalkSpeed;
	default: return 0.0f;
	}
}

bool FSavedMove_SKGSF::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	if (bSavedWantsToSprint != static_cast<FSavedMove_SKGSF*>(NewMove.Get())->bSavedWantsToSprint)
	{
		return false;
	}
	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void FSavedMove_SKGSF::Clear()
{
	Super::Clear();
	bSavedWantsToSprint = false;
}

void FSavedMove_SKGSF::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	const USKGCharacterMovementComponent* CharacterMovementComponent = Cast<USKGCharacterMovementComponent>(C->GetMovementComponent());
	bSavedWantsToSprint = CharacterMovementComponent->bWantsToSprint;
}

void FSavedMove_SKGSF::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);

	USKGCharacterMovementComponent* CharacterMovementComponent = Cast<USKGCharacterMovementComponent>(C->GetMovementComponent());
	CharacterMovementComponent->bWantsToSprint = bSavedWantsToSprint;
}

uint8 FSavedMove_SKGSF::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	if (bSavedWantsToSprint)
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
}

FNetworkPredictionData_Client_SKGSF::FNetworkPredictionData_Client_SKGSF(const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement){}

FSavedMovePtr FNetworkPredictionData_Client_SKGSF::AllocateNewMove()
{
	return (FSavedMovePtr)new FSavedMove_SKGSF();
}
