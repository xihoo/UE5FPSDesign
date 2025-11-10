// Copyright 2023, Dakota Dawe, All rights reserved


#include "DataAssets/SKGPDAOpticMagnificationSettings.h"
#include "Statics/SKGOpticLibrary.h"

float FSKGOpticMagnificationSettings::GetCurrentMagnificationFOV() const
{
	return USKGOpticLibrary::MagnificationToFOVAngle(GetCurrentMagnification());
}
