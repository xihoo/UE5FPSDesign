// Copyright 2024, Dakota Dawe, All rights reserved


#include "Actors/SKGBaseActor.h"

#include "Runtime/Launch/Resources/Version.h"

ASKGBaseActor::ASKGBaseActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
#if ENGINE_MINOR_VERSION >= 5
	SetNetUpdateFrequency(10.0f);
#else
	NetUpdateFrequency = 10.0f;
#endif
}

void ASKGBaseActor::OnBundlesLoaded()
{
	bDAConstructionBundlesLoaded = true;
	DAConstructionBundlesLoaded();
	OnDAConstructionBundlesLoaded.Broadcast();
	OnActorConstructed.Broadcast(this);
}
