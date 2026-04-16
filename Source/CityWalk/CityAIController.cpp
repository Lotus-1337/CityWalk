// Fill out your copyright notice in the Description page of Project Settings.


#include "CityAIController.h"

#include "AIActor.h"
#include "PathFinder.h"
#include "PortalBuilder.h"
#include "Portal.h"
#include "Funnel.h"

#include "Detour/DetourNavMesh.h"

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

	CalculateMeshBoundaries();

	if (!PathFinder)
	{
		UE_LOG(LogTemp, Error, TEXT("PathFinder is invalid. "));
	}
}


bool ACityAIController::FindPath(const FVector& StartLocation, TArray<FVector>& Arr, const FVector& GoalLocation)
{
	if (!PathFinder)
	{
		UE_LOG(LogTemp, Error, TEXT("PathFinder is invalid. ACityAIController::FindPath"));
		return false;
	}

	TArray<dtPolyRef> PolyArr;
	PolyArr.Reserve(64);

	PathFinder->FindPath(PolyArr, StartLocation, GoalLocation);

	if (PolyArr.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("PathFinding didn't return anything. "));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Poly Arr Num: %d"), PolyArr.Num());


	TArray<FPortal> PortalArray;
	PortalArray.Reserve(PolyArr.Num() + 8);

	PortalArray.Add(FPortal::FakePortal(StartLocation)); // Adding a Fake Portal of the Start

	bool DidBuilderSucceed = PortalBuilder->GetPortalPath(PortalArray, PolyArr, PathFinder);

	PortalArray.Add(FPortal::FakePortal(GoalLocation)); // Adding a Fake Portal of the End

	UE_LOG(LogTemp, Log, TEXT("Portal Array Num: %d"), PortalArray.Num());

	if (PortalArray.IsEmpty() || !DidBuilderSucceed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Portal Builder didn't return anything. Empty: %d. Returned: %d"), PortalArray.IsEmpty(), DidBuilderSucceed);
		return false;
	}

	Funnel->BuildFunnelPath(Arr, PortalArray);

	UE_LOG(LogTemp, Log, TEXT("Funnel Arr Num: %d"), Arr.Num());

	PathFinder->CleanNodes();

	return !Arr.IsEmpty();
}

bool ACityAIController::FindPathAI(AAIActor* AI, TArray<FVector>& Arr, const FVector& GoalLocation)
{

	if (!AI)
	{
		UE_LOG(LogTemp, Error, TEXT("AI is invalid. ACityAIController::FindPath"));
		return false;
	}
	
	if (!FindPath(AI->GetActorLocation(), Arr, GoalLocation))
	{
		return false;
	}

	AI->OnFoundNewPath();

	return true;

}


double ACityAIController::FindPathTimered(const FVector & StartLocation, TArray<FVector>&Arr, const FVector & GoalLocation)
{
	double Start = FPlatformTime::Seconds();

	if (FindPath(StartLocation, Arr, GoalLocation))
	{
		double Duration = FPlatformTime::Seconds() - Start;
		BenchmarksArray.Add(Duration);
		return Duration; // returns time of pathfinding if it succeeded
	}

	return -1.0;
}

double ACityAIController::FindPathAITimered(AAIActor* AI, TArray<FVector>& Arr, const FVector& GoalLocation)
{

	double Start = FPlatformTime::Seconds();

	if (FindPathAI(AI, Arr, GoalLocation))
	{
		double Duration = FPlatformTime::Seconds() - Start;
		BenchmarksArray.Add(Duration);
		return Duration; // returns time of pathfinding if it succeeded
	}

	return -1.0;

}

int32 ACityAIController::GetVisitedNodes() const
{
	return PathFinder->NeighbourCount;
}


const dtNavMesh* ACityAIController::GetNavMesh() const
{

	return PathFinder == nullptr ? nullptr : PathFinder->GetDetourMesh();

}

void ACityAIController::CalculateMeshBoundaries()
{

	const dtNavMesh* Mesh = GetNavMesh();

	if (!Mesh)
	{
		UE_LOG(LogTemp, Error, TEXT("Mesh is Invalid. AAIActor::CalculateMeshBoundaries"));
		return;
	}

	
	float MinX = FLT_MAX;
	float MinY = FLT_MAX;

	float MaxX = -FLT_MAX;
	float MaxY = -FLT_MAX;

	float TileMinX = MinX;
	float TileMinY = MinY;

	float TileMaxX = MaxX;
	float TileMaxY = MaxY;

	for (int32 i = 0; i < Mesh->getMaxTiles(); i++)
	{

		const dtMeshTile* Tile = Mesh->getTile(i);

		if (!Tile || !Tile->header)
		{
			continue;
		}

		TileMinX = Tile->header->bmin[0];
		TileMinY = Tile->header->bmin[2];

		TileMaxX = Tile->header->bmax[0];
		TileMaxY = Tile->header->bmax[2];

		MaxX = FMath::Max(MaxX, TileMaxX);
		MaxY = FMath::Max(MaxY, TileMaxY);

		MinX = FMath::Min(MinX, TileMinX);
		MinY = FMath::Min(MinY, TileMinY);

	}

	MeshMin = FVector2D(MinX, MinY);
	MeshMax = FVector2D(MaxX, MaxY);

}