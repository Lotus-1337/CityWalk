// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AIActor.generated.h"

class UAIMovementComponent;
class USkeletalMeshComponent;
class UCapsuleComponent;

class ACityAIController;

DECLARE_LOG_CATEGORY_EXTERN(LogBenchmark, All, All)

UCLASS()
class CITYWALK_API AAIActor : public APawn
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UAIMovementComponent* MovementComponent;

protected:

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	FVector Destination = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	int32 DestinationIndex = 0;

	TArray<FVector> DestinationsArray;

	UPROPERTY(VisibleAnywhere, Category = "Benchmarks")
	double BenchmarkDuration = 0.0;

public:
	// Sets default values for this pawn's properties
	AAIActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Method responsible for changing the destination so the most relevant is actively chosen
	void MoveOnPath();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void MoveAI();

	// Method that needs to be caled every time a new Path is Found.
	void OnFoundNewPath();

	/**
	* Method Benchmarking PathFinding and outputting Time Duration
	* 
	* @param StartLocation: Pathfinding will start from the given location
	* @param GoalLocation:  Pathfinding will search for the given location
	* 
	* @param UseDestinationArray: Shoud DestinationArray be the array given for pathfinding ( will it get Pf's result? )
	* 
	* @param Controller: One can give the controller to speed up the process a little bit
	*
	*/
	void BenchmarkPathFinding(const FVector& StartLocation, const FVector& GoalLocation, bool bUseDestinationArray = false, ACityAIController * AIController = nullptr);

	void RunBenchmark();

	FTimerHandle BenchmarkTimerHandle;

	void ScheduleBenchmark();

	FORCEINLINE USkeletalMeshComponent* GetMeshComponent() const { return MeshComponent; }

};


FORCEINLINE FVector GetRandomVector(const double& MinX, const double& MaxX, const double& MinY, const double& MaxY, const double& MinZ, const double& MaxZ)
{
	return FVector(FMath::RandRange(MinX, MaxX), FMath::RandRange(MinY, MaxY), FMath::RandRange(MinZ, MaxZ));
}

FORCEINLINE FVector GetRandomVector(const double& MinX, const double& MaxX, const double& MinY, const double& MaxY, const double& Z)
{
	return FVector(FMath::RandRange(MinX, MaxX), FMath::RandRange(MinY, MaxY), Z);
}