// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AIActor.generated.h"

class UAIMovementComponent;
class USkeletalMeshComponent;
class UCapsuleComponent;

UCLASS()
class CITYWALK_API AAIActor : public APawn
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UAIMovementComponent* MovementComponent;

protected:

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	FVector Destination;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	int32 DestinationIndex;

	TArray<FVector> DestinationsArray;

public:
	// Sets default values for this pawn's properties
	AAIActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Method responsible for changing the destination so the most relevant is actively chosen
	void MoveOnPath();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void MoveAI();

	// Method that needs to be caled every time a new Path is Found.
	void OnFoundNewPath();

	FORCEINLINE USkeletalMeshComponent* GetMeshComponent() const { return MeshComponent; }

};
