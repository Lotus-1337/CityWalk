// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIBehaviourComponent.generated.h"

class AActivityPoint;
class FAIActivity;
class AAIActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CITYWALK_API UAIBehaviourComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class AAIActor;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI Activity")
	AActivityPoint* ActivityPoint;

	TSharedPtr<FAIActivity> Activity;

	UPROPERTY(EditDefaultsOnly, Category = "AI Activity")
	bool bIsIdle;

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

	void SetNewActivity(const TSharedPtr<FAIActivity>& NewActivity);

	FORCEINLINE FAIActivity* GetActivity() 
	{
		return Activity.Get();
	}

	FORCEINLINE void SetIdle(const bool& Flag)
	{
		bIsIdle = Flag;
	}

	FORCEINLINE bool IsIdle() const
	{
		return bIsIdle;
	}
		
};
