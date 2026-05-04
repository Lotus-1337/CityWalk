// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBehaviourComponent.h"

#include "ActivityPoint.h"
#include "AIActivity.h"
#include "AIActor.h"

// Sets default values for this component's properties
UAIBehaviourComponent::UAIBehaviourComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UAIBehaviourComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!Cast<AAIActor>(GetOwner()))
	{
		UE_LOG(LogTemp, Error, TEXT("Owner is not AAIActor. Please attach this component to AAIActors ONLY. "));
		return;
	}
	
}

void UAIBehaviourComponent::OnSetNewActivity()
{

	AAIActor* AI = Cast<AAIActor>(GetOwner());

	if (!AI)
	{
		UE_LOG(LogTemp, Error, TEXT("AI is invalid or not AAIActor. "));
		return;
	}

	if (!ActivityPoint)
	{
		UE_LOG(LogTemp, Error, TEXT("Activity Point is invalid. "));
		return;
	}

	AI->RequestPathFinding(ActivityPoint->GetLocation());
	
	Activity->OnActivityStarted(*AI);

}

void UAIBehaviourComponent::OnActionEnded()
{
	AAIActor* AI = Cast<AAIActor>(GetOwner());

	if (!AI)
	{
		UE_LOG(LogTemp, Error, TEXT("AI is invalid or not AAIActor. "));
		return;
	}

	Activity->OnActivityEnded(*AI);


}

void UAIBehaviourComponent::SetNewActivity(const TSharedPtr<FAIActivity>& NewActivity)
{
	Activity.Reset();
	Activity = NewActivity;
	OnSetNewActivity();
}