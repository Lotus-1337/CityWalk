// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActivityPoint.generated.h"

class AAIActor;

UCLASS()
class CITYWALK_API AActivityPoint : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	AAIActor* AI = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Location")
	FVector Location = FVector::ZeroVector;

public:

	UPROPERTY(EditDefaultsOnly, Category = "Activity")
	int32 ActivityIndex = 0;

public:	
	// Sets default values for this actor's properties
	AActivityPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	FORCEINLINE FVector GetLocation() const
	{
		return Location;
	}

};
