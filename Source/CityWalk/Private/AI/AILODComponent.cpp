// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AILODComponent.h"

#include "AI/AIActor.h"

// Sets default values for this component's properties
UAILODComponent::UAILODComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UAILODComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UAILODComponent::ProcessLOD()
{

	switch (LODState)
	{

	case EAILODState::Full:
		MaxTickID = MAX_FULL_TICKS;
		break;
	case EAILODState::Medium:
		MaxTickID = MAX_MED_TICKS;
		break;
	case EAILODState::Low:
		MaxTickID = MAX_LOW_TICKS;
		break;
	case EAILODState::NoDetail:
		MaxTickID = MAX_NO_DETAIL_TICKS;
		break;
	case EAILODState::WaitingToDie:
		MaxTickID = MAX_DIE_TICKS;
		break;
	default:
		MaxTickID = INT8_MAX;
		break;

	}

}

bool UAILODComponent::ShouldTickBeExecuted(float DeltaTime)
{
	TicksTime += DeltaTime;

	return IncrementTicks();
}

bool UAILODComponent::IncrementTicks()
{

	TickID++;
	if (TickID > MaxTickID)
	{
		TickID = 0;
		TicksTime = 0;
	}

	return TickID == 0;
}