// Fill out your copyright notice in the Description page of Project Settings.


#include "AIActor.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIMovementComponent.h"

#include "CityAIController.h"

DEFINE_LOG_CATEGORY(LogBenchmark);

// Sets default values
AAIActor::AAIActor()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(RootComponent);

	MeshComponent->SetOnlyOwnerSee(false);
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

	MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	MeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	MovementComponent = CreateDefaultSubobject<UAIMovementComponent>(TEXT("Movement Component"));


	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ACityAIController::StaticClass();

}

static int32 BenchmarkIndex = 1;

// Called when the game starts or when spawned
void AAIActor::BeginPlay()
{
	Super::BeginPlay();

	ACityAIController* CityAIController = Cast<ACityAIController>(GetController());

	if (!CityAIController)
	{
		UE_LOG(LogTemp, Log, TEXT("City AI Controller is invalid. Nicen't "));
		return;
	}

	FVector GoalLocation = FVector(1800.0f, -500.0f, 90.0f);

	BenchmarkIndex = 1;


	//ScheduleBenchmark();

	//DestinationArray.Reserve(64);

	ACityAIController* AIController = Cast<ACityAIController>(GetController());

	FPathRequest Request = FPathRequest(this, GoalLocation);

	AIController->RequestPathFinding(Request);
}

// Called every frame
void AAIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveAI();

}

void AAIActor::MoveAI()
{

	MoveOnPath();

	FVector MovementVector = Destination - GetActorLocation();

	FVector NormalizedVector = MovementVector.GetSafeNormal();

	MovementComponent->Move(NormalizedVector);
	MovementComponent->Rotate(NormalizedVector.Rotation());

}

void AAIActor::MoveOnPath()
{
	if (DestinationsArray.IsEmpty() || !DestinationsArray.IsValidIndex(DestinationIndex))
	{
		return;
	}
	
	float MaxDistance = 50.0f;

	if (FVector::Dist2D(GetActorLocation(), Destination) < MaxDistance && DestinationsArray.IsValidIndex(DestinationIndex + 1))
	{
		DestinationIndex++;
	}

	Destination = DestinationsArray[DestinationIndex];
	

}

void AAIActor::OnFoundNewPath()
{

	if (DestinationsArray.IsEmpty())
	{
		return;
	}

	Destination = DestinationsArray[0];

}

void AAIActor::BenchmarkPathFinding(const FVector& StartLocation, const FVector& GoalLocation, bool bUseDestinationArray, ACityAIController* AIController)
{

	if (!AIController)
	{
		AIController = Cast<ACityAIController>(GetController());

		if (!AIController) return;
	}

	TArray<FVector> Arr;


	double Duration = 0.0;

	if (!bUseDestinationArray)
	{
		Arr.Reserve(64);
		Duration = AIController->FindPathTimered(StartLocation, Arr, GoalLocation);
	}
	else
	{
		Duration = AIController->FindPathTimered(StartLocation, DestinationsArray, GoalLocation);
	}

	if (Duration != -1.0f)
	{

		BenchmarkDuration += FTimers::MicroSeconds(Duration);
	}

	UE_LOG(LogBenchmark, Log, TEXT("Benchmark %d Finished. Duration in MicroSeconds: %f. VisitedNodes: %d"), BenchmarkIndex, FTimers::MicroSeconds(Duration), AIController->GetVisitedNodes());

	BenchmarkIndex++;

}

void AAIActor::RunBenchmark()
{
	ACityAIController * AIController = Cast<ACityAIController>(GetController());

	if (!AIController) return;

	FVector2D MeshMin = AIController->MeshMin;
	FVector2D MeshMax = AIController->MeshMax;

	FVector Start = GetRandomVector(MeshMin.X, MeshMax.X, MeshMin.Y, MeshMax.Y, 50.0);
	FVector Goal =  GetRandomVector(MeshMin.X, MeshMax.X, MeshMin.Y, MeshMax.Y, 50.0);

	UE_LOG(LogTemp, Log, TEXT("Start: %s. Goal: %s"), *Start.ToString(), *Goal.ToString());
	
	BenchmarkPathFinding(Start, Goal, false, AIController);

	const int32 MaxBenchmarks = 100;

	if (BenchmarkIndex >= MaxBenchmarks)
	{

		UE_LOG(LogBenchmark, Warning, TEXT("Just Ran %d Benchmarks: Here's the performance: "), BenchmarkIndex);
		UE_LOG(LogBenchmark, Log, TEXT("Total Benchmarks Duration: %f. Average Benchmark Duration: %f"), BenchmarkDuration, BenchmarkDuration / BenchmarkIndex);

		AIController->BenchmarksArray.Sort();

		int32 BenchmarksCount = AIController->BenchmarksArray.Num() - 1;

		double Median = FTimers::MicroSeconds(AIController->BenchmarksArray[BenchmarksCount * 0.5]);

		double Percentile95 = FTimers::MicroSeconds(AIController->BenchmarksArray[BenchmarksCount * 0.95]);
		double Percentile99 = FTimers::MicroSeconds(AIController->BenchmarksArray[BenchmarksCount]);

		double BestBenchmark = FTimers::MicroSeconds(AIController->BenchmarksArray[0]);

		UE_LOG(LogBenchmark, Log, TEXT("Median of Benchmarks: %f. BestBenchmark: %f. "), Median, BestBenchmark);
		UE_LOG(LogBenchmark, Log, TEXT("95 Percentile: %f, 99 Percentile: %f"), Percentile95, Percentile99);

		return;
	}

	ScheduleBenchmark();

}

void AAIActor::ScheduleBenchmark()
{
	GetWorld()->GetTimerManager().SetTimer(BenchmarkTimerHandle, this, &AAIActor::RunBenchmark, 0.1f, false);
}