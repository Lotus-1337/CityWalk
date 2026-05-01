#include "AIActivity.h"

#include "AIActor.h"
#include "AIBehaviourComponent.h"

TArray<TUniquePtr<FAIActivity>> Activities;


void FWanderingActivity::OnActivityStarted(AAIActor& AI)
{

	UE_LOG(LogTemp, Log, TEXT("FWanderingActivity::OnActivityStarted "));

	FVector Goal = GetRandomVectorInsideMesh(AI);

	AI.RequestPathFinding(Goal);
	AI.BehaviourComponent->SetState(EAIState::Walking);
	AI.BehaviourComponent->SetActivityIndex(Index);

}

void FWanderingActivity::OnActivityEnded(AAIActor& AI)
{

	OnActivityStarted(AI);

}