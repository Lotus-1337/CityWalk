#pragma once

#include "CoreMinimal.h"

class AAIActor;

enum class EAILODState;

#define FULL_LOD_DISTANCE 0.0
#define MED_LOD_DISTANCE 2000.0
#define LOW_LOD_DISTANCE 4000.0
#define NO_DETAIL_DISTANCE 8000.0
#define WAITING_TO_DIE_DISTANCE 10000.0

class FAILODManager
{

public:
	
	FAILODManager() {}

protected:

	int32 ProcessedAI = 0;

public:

	void ManageAILOD(TArray<AAIActor*>& AIArray, const FVector& PlayerLocation = FVector::ZeroVector, const int32& MaxProcessedAIPerFrame = 16);

	EAILODState GetLODByDistance(const double& Distance);

};