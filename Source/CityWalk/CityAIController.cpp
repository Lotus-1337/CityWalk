// Fill out your copyright notice in the Description page of Project Settings.


#include "CityAIController.h"

#include "AIActor.h"
#include "PathFinder.h"
#include "PortalBuilder.h"
#include "Portal.h"


ACityAIController::ACityAIController()
{
	PortalBuilder = MakeUnique<FPortalBuilder>();
}

ACityAIController::~ACityAIController() = default;

void ACityAIController::BeginPlay()
{

	Super::BeginPlay();


	FActorSpawnParameters SpawnParams;

	PathFinder = GetWorld()->SpawnActor<APathFinder>(PathFinderClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (!PathFinder)
	{
		UE_LOG(LogTemp, Error, TEXT("PathFinder is invalid. "));
	}
}


bool ACityAIController::FindPath(AAIActor* AI, TArray<FVector>& Arr, const FVector& GoalLocation)
{

	if (!AI)
	{
		UE_LOG(LogTemp, Error, TEXT("AI is invalid. ACityAIController::FindPath"));
		return false;
	}

	FVector AILocation = AI->GetActorLocation();

	if (!PathFinder)
	{
		UE_LOG(LogTemp, Error, TEXT("PathFinder is invalid. ACityAIController::FindPath"));
		return false;
	}

	TArray<FPolyNode> PolyArr = PathFinder->FindPath(AILocation, GoalLocation);

	if (PolyArr.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("PathFinding didn't return anything. "));
		return false;
	}


	TArray<FPortal> PortalArray;

	bool DidBuilderSucceed = PortalBuilder->GetPortalPath(PortalArray, PolyArr, PathFinder);

	if (PortalArray.IsEmpty() || !DidBuilderSucceed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Portal Builder didn't return anything. Empty: %d. Returned: %d"), PortalArray.IsEmpty(), DidBuilderSucceed);
		return false;
	}

	Arr.Empty();

	for (FPortal & Portal : PortalArray)
	{

		Arr.Add(Portal.GetPortalMiddle());

	}

	AI->OnFoundNewPath();

	return !Arr.IsEmpty();

}