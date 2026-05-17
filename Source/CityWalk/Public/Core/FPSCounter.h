#pragma once

#include "CoreMinimal.h"

#include "FPSCounter.generated.h"

USTRUCT()
struct FFPSCounter
{

	GENERATED_BODY()

	FFPSCounter() {}

	double TotalDeltaTime = 0.0;
	double HowManyTicks = 0.0;

	double MaxFPS = 0.0;
	double MinFPS = 1e10;

	void CalculateFPS(const float& DeltaTime);
	
	void ResetCounter();

};