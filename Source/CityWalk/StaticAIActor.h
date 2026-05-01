// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StaticAIActor.generated.h"

class USkeletalMeshComponent;
class UCapsuleComponent;

class UAnimationAsset;

UCLASS()
class CITYWALK_API AStaticAIActor : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USkeletalMeshComponent* MeshComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UCapsuleComponent* CapsuleComponent;

public:	
	// Sets default values for this actor's properties
	AStaticAIActor();

	void SetAnimation(UAnimationAsset* Animation);

};
