// Fill out your copyright notice in the Description page of Project Settings.


#include "AIActor.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIMovementComponent.h"

#include "CityAIController.h"

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

	FVector GoalLocation = FVector(-1800.0f, -200.0f, 0.0f);

	BenchmarkPathFinding(GetActorLocation(), GoalLocation, true, CityAIController);

	if (DestinationsArray.IsEmpty())
	{
		return;
	}

	Destination = DestinationsArray[0];

	int32 Index = 0;

	for (FVector V : DestinationsArray)
	{
		UE_LOG(LogTemp, Log, TEXT("Destination %d: %s"), Index, *V.ToString());
		Index++;

		DrawDebugPoint(GetWorld(), V, 5.0f, FColor::Red, false, 10.0f);
	}

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

static int32 BenchmarkIndex = 0;

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
		Duration = AIController->FindPathTimered(StartLocation, Arr, GoalLocation);
	}
	else
	{
		Duration = AIController->FindPathTimered(StartLocation, DestinationsArray, GoalLocation);
	}


	UE_LOG(LogBenchmark, Log, TEXT("Benchmark %d Finished. Duration in MicroSeconds: %f. VisitedNodes: %d"), BenchmarkIndex, FTimers::MicroSeconds(Duration), Controller->GetVisitedNodes());

	BenchmarkIndex++;

}