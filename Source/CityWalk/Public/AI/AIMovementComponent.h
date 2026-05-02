// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIMovementComponent.generated.h"

class USkeletalMeshComponent;
class UCapsuleComponent;

class AAIActor;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CITYWALK_API UAIMovementComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class AAIActor;

protected:

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	double ZVelocity = 0.0;
	
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	float fDeltaTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	int32 MovementSpeed = 100;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	FVector MovementVector;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	bool IsInTheAir = false;

public:	
	// Sets default values for this component's properties
	UAIMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	void Fall();

public:	

	void Move();

	void AddMovementInput(const FVector& MovementVector);

	void Jump();

	void Rotate(const FRotator& NewRotation);

	FORCEINLINE void SetMovementSpeed(const int32& NewSpeed) { MovementSpeed = NewSpeed; }

	FORCEINLINE double GetMovementScalar() const { return fDeltaTime * MovementSpeed; }


};
