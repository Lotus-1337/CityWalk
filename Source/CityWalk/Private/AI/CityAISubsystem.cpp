// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CityAISubsystem.h"

#include "AI/AIActor.h"
#include "Player/WalkerCharacter.h"

// Sets default values
UCityAISubsystem::UCityAISubsystem()
{

	FString Path = TEXT("/Game/Blueprints/");

	FString AIClassPath = Path / TEXT("BP_AIActor.BP_AIActor_C");

	static ConstructorHelpers::FClassFinder<AAIActor> AIClassAsset(*AIClassPath);

	if (!AIClassAsset.Succeeded())
	{
		return;
	}

	AIClass = AIClassAsset.Class;

}

void UCityAISubsystem::OnWorldBeginPlay(UWorld& World)
{

	Super::OnWorldBeginPlay(World);

}

void UCityAISubsystem::SpawnAI()
{

	const int32 MaxAI = 32;
	const int32 MaxSpawnedAIPerFrame = 8;

	const int32 MaxAIThisFrame = FMath::Clamp(MaxAI - AIArray.Num(), 0, MaxSpawnedAIPerFrame);

	const FVector MaxAIPositionRange = FVector(4000.0f, 4000.0f, 0.0f);
	const FVector PlayerLocation = GetPlayerLocation();

	FVector Min = PlayerLocation - MaxAIPositionRange;
	FVector Max = PlayerLocation + MaxAIPositionRange;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int32 i = 0; i < MaxAIThisFrame; i++)
	{

		FVector RandomLocation = GetRandomVector(Min.X, Max.X, Min.Y, Max.Y, MaxAIPositionRange.Z);

		AAIActor* AI = GetWorld()->SpawnActor<AAIActor>(AIClass, RandomLocation, FRotator::ZeroRotator, Params);

		AIArray.Add(AI);

	}

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UCityAISubsystem::SpawnAI);

}

FVector UCityAISubsystem::GetPlayerLocation()
{
	return Player != nullptr ? Player->GetActorLocation() : FVector::ZeroVector;
}