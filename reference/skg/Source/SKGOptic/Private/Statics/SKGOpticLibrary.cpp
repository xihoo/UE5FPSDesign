// Copyright 2023, Dakota Dawe, All rights reserved


#include "Statics/SKGOpticLibrary.h"

float USKGOpticLibrary::MagnificationToFOVAngle(const float Magnification)
{
	const float factor = 2.0f * FMath::Tan(FMath::DegreesToRadians(0.5f * 35.0f));
	return FMath::RadiansToDegrees(2.0f * FMath::Atan(factor / (2.0f * Magnification)));
	//return FMath::RadiansToDegrees(FMath::Atan(2.0f / Magnification) * 0.5f);
}
