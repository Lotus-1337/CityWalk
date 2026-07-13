#include "Core/FPSCounter.h"

void FFPSCounter::CalculateFPS(const float& DeltaTime)
{

	if (DeltaTime == 0.0)
	{
		UE_LOG(LogTemp, Error, TEXT("Delta Time is 0. Cannot divide by 0. "));
		return;
	}

	// looking at just the last 100 frames.
	if (++HowManyTicks >= 100)
	{
		HowManyTicks = 1;
		TotalDeltaTime = 0.0;
	}

	TotalDeltaTime += DeltaTime;

	double FPS = 1 / DeltaTime;
	double AverageFPS = HowManyTicks / TotalDeltaTime;

	MaxFPS = FMath::Max(FPS, MaxFPS);
	MinFPS = FMath::Min(FPS, MinFPS);

	UE_LOG(LogTemp, Log, TEXT("FPS: %f. Average FPS: %f. Min FPS: %f. Max FPS: %f"), FPS, AverageFPS, MinFPS, MaxFPS);

}

void FFPSCounter::ResetCounter()
{
	
	TotalDeltaTime = 0.0;
	HowManyTicks = 0.0;

	MaxFPS = 0.0;
	MinFPS = 1e10;

}