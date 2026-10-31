// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "AI/AILODManager.h"

#include "CityAISubsystem.generated.h"

class AAIActor;
class AWalkerCharacter;


UCLASS()
class CITYWALK_API UCityAISubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TArray<AAIActor*> AIArray;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AAIActor> AIClass;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	AWalkerCharacter* Player;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	int32 MaxAI = 128;

	TUniquePtr<FAILODManager> LODManager;

public:	
	// Sets default values for this actor's properties
	UCityAISubsystem();

protected:

	// Called when the game starts or when spawned
	virtual void OnWorldBeginPlay(UWorld& World) override;

public:	

	void SpawnAI();

	void ManageAILOD();

	FORCEINLINE void SetPlayer(AWalkerCharacter* NewPlayer)
	{
		Player = NewPlayer;
	}

	/** @returns Player Location or Zero Vector if Player is nullptr. */
	FVector GetPlayerLocation();

};
