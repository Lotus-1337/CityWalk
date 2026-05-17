#include "Core/FPSCounter.h"

void FFPSCounter::CalculateFPS(const float& DeltaTime)
{

	HowManyTicks += 1;
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