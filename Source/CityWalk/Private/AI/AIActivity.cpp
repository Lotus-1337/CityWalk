#include "AIActivity.h"

#include "AIActor.h"
#include "AIBehaviourComponent.h"
#include "AIVisualSubsystem.h"


void FIdleActivity::OnActivityStarted(AAIActor& AI)
{

	AI.BehaviourComponent->SetIdle(true);

}

void FIdleActivity::OnActivityEnded(AAIActor& AI)
{

	AI.BehaviourComponent->SetIdle(false);

}

void FWanderingActivity::OnActivityStarted(AAIActor& AI)
{

	UE_LOG(LogTemp, Log, TEXT("FWanderingActivity::OnActivityStarted "));

	FVector Goal = GetRandomVectorInsideMesh(AI);

	AI.RequestPathFinding(Goal);

}

void FWanderingActivity::OnActivityEnded(AAIActor& AI)
{

	OnActivityStarted(AI);

}

void FTalkingActivity::OnActivityStarted(AAIActor& AI)
{

	UAIVisualSubsystem* VisualSubsystem = AI.GetWorld()->GetSubsystem<UAIVisualSubsystem>();

	if (!VisualSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("FTalkingActivity::OnActivityStarted"));
		return;
	}

	UAnimationAsset* Animation = VisualSubsystem->GetRandomAnimation();

	if (!Animation)
	{
		return;
	}

	AI.GetMeshComponent()->SetAnimation(Animation);

}

void FTalkingActivity::OnActivityEnded(AAIActor& AI)
{



}