// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActivityPoint.generated.h"

class AAIActor;
class UCapsuleComponent;
class UStaticMeshComponent;

class FAIActivity;

UCLASS()
class CITYWALK_API AActivityPoint : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Location")
	FVector Location = FVector::ZeroVector;

public:

	UPROPERTY(EditDefaultsOnly, Category = "Activity Point")
	int32 MaxAIAtPoint = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Activity Point")
	int32 AIAtPoint = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Activity Point")
	bool IsAvailable = true;

public:

	TSharedPtr<FAIActivity> Activity;

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

	FORCEINLINE void AddAIAtPoint()
	{
		AIAtPoint++;
		if (AIAtPoint >= MaxAIAtPoint) { IsAvailable = false; }
	}

	FORCEINLINE void RemoveAIAtPoint()
	{
		AIAtPoint--;
		if (AIAtPoint < MaxAIAtPoint) { IsAvailable = true; }
	}
};
