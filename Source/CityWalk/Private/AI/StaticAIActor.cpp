// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticAIActor.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AStaticAIActor::AStaticAIActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	SetRootComponent(CapsuleComponent);
	
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("No Collision"));

	MeshComponent->SetOnlyOwnerSee(false);

}

void AStaticAIActor::SetAnimation(UAnimationAsset* Animation)
{

	MeshComponent->SetAnimation(Animation);

}