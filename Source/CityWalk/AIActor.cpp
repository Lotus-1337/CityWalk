// Fill out your copyright notice in the Description page of Project Settings.


#include "AIActor.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIMovementComponent.h"

#include "CityAIController.h"
#include "PathFindingSubsystem.h"
#include "Timers.h"

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

}

static int32 BenchmarkIndex = 1;

// Called when the game starts or when spawned
void AAIActor::BeginPlay()
{
	Super::BeginPlay();

	UPathFindingSubsystem* PFSubsystem = GetWorld()->GetSubsystem<UPathFindingSubsystem>();

	if (!PFSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Pathfinding Subsystem is nullptr. AAIActor::BeginPlay "));
		return;
	}

	FVector2D Min = PFSubsystem->MeshMin;
	FVector2D Max = PFSubsystem->MeshMax;

	FVector GoalLocation = GetRandomVector(Min.X, Max.X, Min.Y, Max.Y, 90.0f);
	

	BenchmarkIndex = 1;


	//ScheduleBenchmark();

	//DestinationArray.Reserve(64);

	FPathRequest Request = FPathRequest(this, GoalLocation);

	PFSubsystem->RequestPathFinding(Request);
}

// Called every frame
void AAIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (State == EAIState::Idle)
	{
		return;
	}

	MoveAI();
}

void AAIActor::MoveAI()
{

	MoveOnPath();

	FVector ActorLocation = GetActorLocation();

	double Distance = FVector::DistSquared2D(ActorLocation, DestinationsArray.Last());

	const double MaxDistance = FMath::Square(MovementComponent->GetMovementScalar());

	// Setting the Location to Destination to avoid random movement close to destination. 
	if (Distance < MaxDistance)
	{
		State = EAIState::Idle;
		MovementComponent->MovementVector = FVector::ZeroVector;
		return;
	}

	FVector MovementVector = Destination - ActorLocation;
	MovementVector.Z = 0.0;

	FVector NormalizedVector = MovementVector.GetSafeNormal();

	MovementComponent->AddMovementInput(NormalizedVector);
	
	// Not calculating Pitch and Roll for a little performance boost
	FRotator NewRotation = FRotator(0.0f, FMath::RadiansToDegrees(FMath::Atan2(NormalizedVector.Y, NormalizedVector.X)), 0.0f);

	MovementComponent->Rotate(NewRotation);

}

double AAIActor::MoveOnPath()
{
	if (DestinationsArray.IsEmpty() || !DestinationsArray.IsValidIndex(DestinationIndex))
	{
		return -10;
	}
	
	float MaxDistance = 50.0f;

	if (!DestinationsArray.IsValidIndex(DestinationIndex + 1)) // MicroOptimsation, no unnecessary Dist2D checking.
	{
		return -10;
	}

	double Distance = FVector::Dist2D(GetActorLocation(), Destination);

	if (Distance < MaxDistance)
	{
		DestinationIndex++;
	}

	Destination = DestinationsArray[DestinationIndex];

	return Distance;
	

}

void AAIActor::OnFoundNewPath()
{

	if (DestinationsArray.IsEmpty())
	{
		return;
	}

	State = EAIState::Walking;

	Destination = DestinationsArray[0];

}

void AAIActor::BenchmarkPathFinding(const FVector& StartLocation, const FVector& GoalLocation, bool bUseDestinationArray, UPathFindingSubsystem* PFSubsystem)
{

	if (!PFSubsystem)
	{
		PFSubsystem = GetWorld()->GetSubsystem<UPathFindingSubsystem>();

		if (!PFSubsystem) return;
	}

	TArray<FVector> Arr;


	double Duration = 0.0;

	if (!bUseDestinationArray)
	{
		Arr.Reserve(64);
		Duration = PFSubsystem->FindPathTimered(StartLocation, Arr, GoalLocation);
	}
	else
	{
		Duration = PFSubsystem->FindPathTimered(StartLocation, DestinationsArray, GoalLocation);
	}

	if (Duration != -1.0f)
	{

		BenchmarkDuration += FTimers::MicroSeconds(Duration);
	}

	UE_LOG(LogBenchmark, Log, TEXT("Benchmark %d Finished. Duration in MicroSeconds: %f. VisitedNodes: %d"), BenchmarkIndex, FTimers::MicroSeconds(Duration), PFSubsystem->GetVisitedNodes());

	BenchmarkIndex++;

}

void AAIActor::RunBenchmark()
{
	UPathFindingSubsystem* PFSubsystem = GetWorld()->GetSubsystem<UPathFindingSubsystem>();

	if (!PFSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Pathfinding Subsystem is nullptr. AAIActor::BeginPlay "));
		return;
	}

	FVector2D MeshMin = PFSubsystem->MeshMin;
	FVector2D MeshMax = PFSubsystem->MeshMax;

	FVector Start = GetRandomVector(MeshMin.X, MeshMax.X, MeshMin.Y, MeshMax.Y, 50.0);
	FVector Goal =  GetRandomVector(MeshMin.X, MeshMax.X, MeshMin.Y, MeshMax.Y, 50.0);

	UE_LOG(LogTemp, Log, TEXT("Start: %s. Goal: %s"), *Start.ToString(), *Goal.ToString());
	
	BenchmarkPathFinding(Start, Goal, false, PFSubsystem);

	const int32 MaxBenchmarks = 100;

	if (BenchmarkIndex >= MaxBenchmarks)
	{

		UE_LOG(LogBenchmark, Warning, TEXT("Just Ran %d Benchmarks: Here's the performance: "), BenchmarkIndex);
		UE_LOG(LogBenchmark, Log, TEXT("Total Benchmarks Duration: %f. Average Benchmark Duration: %f"), BenchmarkDuration, BenchmarkDuration / BenchmarkIndex);

		PFSubsystem->BenchmarksArray.Sort();

		int32 BenchmarksCount = PFSubsystem->BenchmarksArray.Num() - 1;

		double Median = FTimers::MicroSeconds(PFSubsystem->BenchmarksArray[BenchmarksCount * 0.5]);

		double Percentile95 = FTimers::MicroSeconds(PFSubsystem->BenchmarksArray[BenchmarksCount * 0.95]);
		double Percentile99 = FTimers::MicroSeconds(PFSubsystem->BenchmarksArray[BenchmarksCount]);

		double BestBenchmark = FTimers::MicroSeconds(PFSubsystem->BenchmarksArray[0]);

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