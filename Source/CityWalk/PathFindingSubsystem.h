// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PathFindingSubsystem.generated.h"

class APathFinder;
class AAIActor;
class FPortalBuilder;
class FFunnel;

class dtNavMesh;

USTRUCT()
struct FPathRequest
{

	GENERATED_BODY()

public:

	FPathRequest(AAIActor* NewAI, const FVector& NewGoal)
	{
		AI = NewAI;
		Goal = NewGoal;
	}

	FPathRequest() {}

	AAIActor* AI = nullptr;
	FVector Goal = FVector::ZeroVector;

};

/**
 * 
 */
UCLASS()
class CITYWALK_API UPathFindingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Path Finder")
	TSubclassOf<APathFinder> PathFinderClass;

	UPROPERTY(VisibleAnywhere, Category = "Path Finder")
	APathFinder* PathFinder;

	/** Object responsible for building Portal Paths **/
	TUniquePtr<FPortalBuilder> PortalBuilder;

	/** Object responsible for building Funneled Paths **/
	TUniquePtr<FFunnel> Funnel;

	TQueue<FPathRequest> AIQueue;FTimerHandle PFTimerHandle;

public:

	UPathFindingSubsystem();

	~UPathFindingSubsystem();

	UPROPERTY(VisibleAnywhere, Category = "Mesh Boundaries")
	FVector2D MeshMin = FVector2D(-4096.0f, -4096.0f);

	UPROPERTY(VisibleAnywhere, Category = "Mesh Boundaries")
	FVector2D MeshMax = FVector2D(4096.0f, 4096.0f);

	TArray<double> BenchmarksArray;

protected:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/**
	* Performs A* Pathfinding Algorithm for X First AI's in the Queue
	*
	* Very Performance Heavy Method. It's performed EVERY TICK
	*/
	void ProcessPathFinding();

public:

	/**
	* Finds the most optimal Path from AI's Location to Goal
	*
	* @param AI: AI's location is used to find the path to goal
	*
	* @param Arr: Array of points on the Path
	*
	* @returns Whether PathFinding was successfull
	*/
	bool FindPathAI(AAIActor* AI, TArray<FVector>* Arr, const FVector& GoalLocation);

	/**
	* Finds the most optimal Path from AI's Location to Goal
	*
	* @param Arr: Array of points on the Path
	*
	* @returns Whether PathFinding was successfull
	*/
	bool FindPath(const FVector& StartLocation, TArray<FVector>& Arr, const FVector& GoalLocation);

	/**
	* Finds The most optimal Path from AI's Location to Goal
	*
	* @param AI: AI's location is used to find the path to goal
	*
	* @param Arr: Array of points on the Path
	*
	* @returns Time PathFinding took measured in seconds
	*/
	double FindPathAITimered(AAIActor* AI, TArray<FVector>* Arr, const FVector& GoalLocation);

	/**
	* Finds The most optimal Path from AI's Location to Goal
	*
	* @param Arr: Array of points on the Path
	*
	* @returns Time PathFinding took measured in seconds
	*/
	double FindPathTimered(const FVector& StartLocation, TArray<FVector>& Arr, const FVector& GoalLocation);

	int32 GetVisitedNodes() const;

	const dtNavMesh* GetNavMesh() const;

	void CalculateMeshBoundaries();

	FORCEINLINE void RequestPathFinding(FPathRequest& Request)
	{
		AIQueue.Enqueue(Request);
	}

	FORCEINLINE void SchedulePathFinding()
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UPathFindingSubsystem::ProcessPathFinding);
	}

};
