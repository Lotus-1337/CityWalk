// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFindingSubsystem.h"

#include "AI/AIActor.h"
#include "PathFinding/PathFinder.h"
#include "PathFinding/PortalBuilder.h"
#include "PathFinding/Portal.h"
#include "PathFinding/Funnel.h"
#include "PathFinding/PFHelper.h"

#include "Detour/DetourNavMesh.h"

UPathFindingSubsystem::UPathFindingSubsystem()
{
	PortalBuilder = MakeUnique<FPortalBuilder>();
}

UPathFindingSubsystem::~UPathFindingSubsystem() = default;

void UPathFindingSubsystem::OnWorldBeginPlay(UWorld& World)
{
	Super::OnWorldBeginPlay(World);


	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UE_LOG(LogTemp, Log, TEXT("Spawning PathFinder. "));

	PathFinder = GetWorld()->SpawnActor<APathFinder>(APathFinder::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);


	if (!PathFinder)
	{
		UE_LOG(LogTemp, Error, TEXT("PathFinder is invalid. "));
	}

	FTimerManager TimerManager;

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UPathFindingSubsystem::InitializePathFinding);

	SchedulePathFinding();

}

void UPathFindingSubsystem::ProcessPathFinding()
{

	// Warning. Changing this variable highly impacts performance.
	const int32 MaxPathFindingsPerFrame = 8;

	for (int32 i = 0; i < MaxPathFindingsPerFrame; i++)
	{

		FPathRequest Request;
		if (!AIQueue.Dequeue(Request))
		{
			break;
		}

		AAIActor* AI = Request.AI;

		PFStatus Status = FindPath(AI->GetActorLocation(), AI->DestinationsArray, Request.Goal);

		// Sets AI's new activity to default
		if (PFStatus_DidFail(Status))
		{
			AI->SetDefaultActivity();
		}

		AI->OnFoundNewPath();

	}

	// Schedules This function for the next tick, so it's repeated every tick.
	SchedulePathFinding();

}


PFStatus UPathFindingSubsystem::FindPath(const FVector& StartLocation, TArray<FVector>& Arr, const FVector& GoalLocation)
{
	if (!PathFinder)
	{
		UE_LOG(LogTemp, Error, TEXT("PathFinder is invalid. UPathFindingSubsystem::FindPath"));
		return PFStatus_OtherFailed;
	}

	if (!IsInitialized)
	{
		UE_LOG(LogTemp, Error, TEXT("Subsystem Is not fully initialized. UPathFindingSubsystem::FindPath was called too early. "));
		return PFStatus_OtherFailed;
	}

	TArray<dtPolyRef> PolyArr;
	PolyArr.Reserve(64);

	PathFinder->FindPath(PolyArr, StartLocation, GoalLocation);

	if (PolyArr.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("PathFinding from %s to %s didn't return anything. "), *StartLocation.ToString(), *GoalLocation.ToString());
		PathFinder->CleanNodes();
		return PFStatus_PFFailed;
	}


	TArray<FPortal> PortalArray;
	PortalArray.Reserve(PolyArr.Num() + 8);

	PortalArray.Add(FPortal::FakePortal(StartLocation)); // Adding a Fake Portal of the Start

	bool DidBuilderSucceed = PortalBuilder->GetPortalPath(PortalArray, PolyArr, PathFinder);

	PortalArray.Add(FPortal::FakePortal(GoalLocation)); // Adding a Fake Portal of the End

	if (PortalArray.IsEmpty() || !DidBuilderSucceed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Portal Builder didn't return anything. Empty: %d. Returned: %d"), PortalArray.IsEmpty(), DidBuilderSucceed);
		PathFinder->CleanNodes();
		return PFStatus_PBFailed;
	}

	Funnel->BuildFunnelPath(Arr, PortalArray);

	PathFinder->CleanNodes();

	return !Arr.IsEmpty() ? PFStatus_Succeeded : PFStatus_FunnelFailed;
}



double UPathFindingSubsystem::FindPathTimered(const FVector& StartLocation, TArray<FVector>& Arr, const FVector& GoalLocation)
{
	double Start = FPlatformTime::Seconds();


	PFStatus Status = FindPath(StartLocation, Arr, GoalLocation);

	if (PFStatus_DidFail(Status)) return -1.0;

	double Duration = FPlatformTime::Seconds() - Start;
	BenchmarksArray.Add(Duration);
	return Duration; // returns time of pathfinding if it succeeded

}

int32 UPathFindingSubsystem::GetVisitedNodes() const
{
	return PathFinder->NeighbourCount;
}


const dtNavMesh* UPathFindingSubsystem::GetNavMesh() const
{

	return PathFinder == nullptr ? nullptr : PathFinder->GetDetourMesh();

}

void UPathFindingSubsystem::InitializePathFinding()
{
	if (IsInitialized)
	{
		return;
	}

	CalculateMeshBoundaries();
	PathFinder->InitializeQuery(GetNavMesh());

	IsInitialized = true;
}

void UPathFindingSubsystem::CalculateMeshBoundaries()
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