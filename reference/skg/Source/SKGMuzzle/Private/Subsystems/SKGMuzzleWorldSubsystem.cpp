// Copyright 2023, Dakota Dawe, All rights reserved


#include "Subsystems/SKGMuzzleWorldSubsystem.h"
#include "Components/SKGMuzzleComponent.h"

void USKGMuzzleWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int32 i = 0; i < Components.Num(); ++i)
	{
		if (USKGMuzzleComponent* Component = Components[i])
		{
			Component->CoolMuzzle(DeltaTime);
		}
	}
}

void USKGMuzzleWorldSubsystem::RegisterComponent(USKGMuzzleComponent* Component)
{
	Components.Emplace(Component);
}

void USKGMuzzleWorldSubsystem::UnregisterComponent(USKGMuzzleComponent* Component)
{
	Components.Remove(Component);
}