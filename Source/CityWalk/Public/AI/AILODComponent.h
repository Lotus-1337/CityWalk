// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AILODComponent.generated.h"


UENUM()
enum class EAILODState
{

#define MAX_FULL_TICKS 0i8
#define MAX_MED_TICKS 2i8
#define MAX_LOW_TICKS 8i8
#define MAX_NO_DETAIL_TICKS 16i8
#define MAX_DIE_TICKS INT8_MAX

	Full,
	Medium,
	Low,
	NoDetail,
	WaitingToDie

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CITYWALK_API UAILODComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "LOD")
	EAILODState LODState = EAILODState::Full;

public:

	UPROPERTY(EditDefaultsOnly, Category = "Tick")
	int8 TickID = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Tick")
	int8 MaxTickID = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Tick")
	float TicksTime = 0;

public:	
	// Sets default values for this component's properties
	UAILODComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	void ProcessLOD();

	bool IncrementTicks();

	bool ShouldTickBeExecuted(float DeltaTime);

	FORCEINLINE void SetLOD(const EAILODState& NewLOD)
	{
		LODState = NewLOD;
		ProcessLOD();
	}
		
};
