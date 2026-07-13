#include "AI/AILODManager.h"

#include "AI/AIActor.h"
#include "AI/AILODComponent.h"

void FAILODManager::ManageAILOD(TArray<AAIActor*>& AIArray, const FVector& PlayerLocation, const int32& MaxProcessedAIPerFrame)
{

	const int32 AIProcessedThisFrame = FMath::Clamp(AIArray.Num() - ProcessedAI, 0, MaxProcessedAIPerFrame);

	for (int32 i = 0; i < AIProcessedThisFrame; ++i)
	{

		const int32 Index = ProcessedAI + i;

		AAIActor* AI = AIArray[Index];

		if (!AI) continue;

		if (!AI->LODComponent) continue;

		EAILODState LOD = GetLODByDistance(FVector::Dist2D(AI->GetActorLocation(), PlayerLocation));

		AI->LODComponent->SetLOD(LOD);

	}

	ProcessedAI += AIProcessedThisFrame;

	if (ProcessedAI >= AIArray.Num() - 1)
	{
		ProcessedAI = 0;
	}

}

EAILODState FAILODManager::GetLODByDistance(const double& Distance)
{

	if (Distance > WAITING_TO_DIE_DISTANCE)
	{
		return EAILODState::WaitingToDie;
	}
	else if (Distance > NO_DETAIL_DISTANCE)
	{
		return EAILODState::NoDetail;
	}
	else if (Distance > LOW_LOD_DISTANCE)
	{
		return EAILODState::Low;
	}	
	else if (Distance > MED_LOD_DISTANCE)
	{
		return EAILODState::Medium;
	}
	else 
	{
		return EAILODState::Full;
	}
}