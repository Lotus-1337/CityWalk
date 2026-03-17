// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CityAIController.generated.h"

class APathFinder;
class AAIActor;
class FPortalBuilder;

/**
 * 
 */
UCLASS()
class CITYWALK_API ACityAIController : public AAIController
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Path Finder")
	TSubclassOf<APathFinder> PathFinderClass;

	UPROPERTY(VisibleAnywhere, Category = "Path Finder")
	APathFinder* PathFinder;

	TObjectPtr<

protected:

	virtual void BeginPlay() override;

public:

	/** */
	bool FindPath(AAIActor* AI, TArray<FVector> & Arr, const FVector& GoalLocation);

};
