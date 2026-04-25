// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "PortalBuilder.h"
#include "Funnel.h"

#include "CityAIController.generated.h"

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
* AI Controller Receiving PathFinding Requests and running PathFinding, Portal Building and Funnel Algorithms * 
*/
UCLASS()
class CITYWALK_API ACityAIController : public AAIController
{
	GENERATED_BODY()

public:

	ACityAIController();

	~ACityAIController();
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Path Finder")
	TSubclassOf<APathFinder> PathFinderClass;

	UPROPERTY(VisibleAnywhere, Category = "Path Finder")
	APathFinder* PathFinder;

	/** Object responsible for building Portal Paths **/
	TUniquePtr<FPortalBuilder> PortalBuilder;

	/** Object responsible for building Funneled Paths **/
	TUniquePtr<FFunnel> Funnel;

	TQueue<FPathRequest> AIQueue;

	FTimerHandle PFTimerHandle;

public:

	UPROPERTY(VisibleAnywhere, Category = "Mesh Boundaries")
	FVector2D MeshMin = FVector2D(-4096.0f, -4096.0f);

	UPROPERTY(VisibleAnywhere, Category = "Mesh Boundaries")
	FVector2D MeshMax = FVector2D(4096.0f, 4096.0f);

	TArray<double> BenchmarksArray;

protected:

	virtual void BeginPlay() override;

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
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ACityAIController::ProcessPathFinding);
	}

};

namespace FTimers
{

	/**
	* @param Seconds time converted to milliseconds ( multiplied by 10 ^ 3 )
	* @returns Seconds -> Milliseconds
	*/
	FORCEINLINE double MilliSeconds(const double& Seconds)
	{
		return Seconds * 1e3;
	}

	/**
	* @param Seconds time converted to microseconds ( multiplied by 10 ^ 6 )
	* @returns Seconds -> Microseconds
	*/
	FORCEINLINE double MicroSeconds(const double& Seconds)
	{
		return Seconds * 1e6;
	}


};
