// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIBehaviourComponent.generated.h"

class AActivityPoint;

UENUM()
enum class EAIState
{

	Idle,
	Walking,
	Talking,
	Working

};

FORCEINLINE EAIState GetNextState(const EAIState& State)
{
	switch (State)
	{
	case EAIState::Idle: return EAIState::Walking;
	case EAIState::Walking: return EAIState::Talking;
	case EAIState::Talking: return EAIState::Working;
	case EAIState::Working: return EAIState::Idle;
	default: return EAIState::Idle;
	}
}

class FAIActivity;
class AAIActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CITYWALK_API UAIBehaviourComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class AAIActor;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI State")
	EAIState State;

	UPROPERTY(EditDefaultsOnly, Category = "AI Activity")
	AActivityPoint* ActivityPoint;

	UPROPERTY(VisibleAnywhere, Category = "AI Activity")
	uint8 ActivityIndex;

public:	
	// Sets default values for this component's properties
	UAIBehaviourComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnActionEnded();

	void OnSetNewActivity();

	FORCEINLINE void SetNewActivity()
	{
		OnSetNewActivity();
	}

	FORCEINLINE void SetState(const EAIState& NewState) { State = NewState;  }
	FORCEINLINE EAIState GetState() { return State; }

	FORCEINLINE void SetActivityIndex(const uint8& NewIndex) { ActivityIndex = NewIndex;  }
	FORCEINLINE uint8 GetActivityIndex() { return ActivityIndex;  }

	FAIActivity* GetActivity() const;
		
};
