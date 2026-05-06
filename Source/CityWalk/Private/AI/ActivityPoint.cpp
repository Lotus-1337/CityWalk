// Fill out your copyright notice in the Description page of Project Settings.


#include "ActivityPoint.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "ActivityPointsSubsystem.h"

// Sets default values
AActivityPoint::AActivityPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	SetRootComponent(CapsuleComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(RootComponent);

	MeshComponent->SetOnlyOwnerSee(false);

}

// Called when the game starts or when spawned
void AActivityPoint::BeginPlay()
{
	Super::BeginPlay();

	Location = GetActorLocation();

	UActivityPointsSubsystem* POISubsystem = GetWorld()->GetSubsystem<UActivityPointsSubsystem>();
	
	if (!POISubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Activity Points Subsystem is invalid. AActivityPoint::BeginPlay"))
		return;
	}

	POISubsystem->AddActivityPoint(this);

}

