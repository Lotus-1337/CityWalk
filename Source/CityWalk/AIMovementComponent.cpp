// Fill out your copyright notice in the Description page of Project Settings.


#include "AIMovementComponent.h"

bool IsNearlyZero(const double& number, const double& epsilon = UE_KINDA_SMALL_NUMBER)
{
	return FMath::Abs(number) < epsilon;
}

// Sets default values for this component's properties
UAIMovementComponent::UAIMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAIMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	IsInTheAir = false;
	
}

void UAIMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	fDeltaTime = DeltaTime;

	//Fall();
	Move();

}
void UAIMovementComponent::Jump()
{

	if (IsInTheAir)
	{
		return;
	}

	ZVelocity += 600;

}

void UAIMovementComponent::Fall()
{

	if (!IsInTheAir) 
	{
		return; 
	}

	float FallingMultiplier = 10.0f;

	ZVelocity += -98 * FallingMultiplier * fDeltaTime;

}


void UAIMovementComponent::AddMovementInput(const FVector& NewMovementVector)
{

	MovementVector.X = NewMovementVector.X;
	MovementVector.Y = NewMovementVector.Y;

}

void UAIMovementComponent::Move()
{

	FVector OwnerLocation = GetOwner()->GetActorLocation();
	FVector NewLocation = OwnerLocation;

	NewLocation.X += MovementVector.X * MovementSpeed * fDeltaTime;
	NewLocation.Y += MovementVector.Y * MovementSpeed * fDeltaTime;
	NewLocation.Z += ZVelocity * fDeltaTime;

	FHitResult Hit;

	bool bDidSucceed = GetOwner()->SetActorLocation(NewLocation, true, &Hit);

	if (IsNearlyZero(Hit.ImpactNormal.Z))
	{
		IsInTheAir = true;
	}
	else
	{
		IsInTheAir = false;
		ZVelocity = 0.0;
	}


	if (bDidSucceed) // if the move was succesfull, we're returning
	{
		return ;
	}

	const static float ImpactScalar = 5.0f;

	NewLocation += Hit.ImpactNormal * ImpactScalar;

	if (!IsInTheAir) { NewLocation.Z = OwnerLocation.Z + Hit.ImpactNormal.Z; }

	GetOwner()->SetActorLocation(NewLocation, true, &Hit);

}

void UAIMovementComponent::Rotate(const FRotator& NewRotation)
{

	FRotator Rotation = NewRotation;

	Rotation.Pitch = 0.0f;
	Rotation.Roll = 0.0f;

	GetOwner()->SetActorRotation(Rotation);

}