// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "PortalBuilder.h"
#include "Funnel.h"

#include "CityAIController.generated.h"

class APathFinder;
class AAIActor;
class FPortalBuilder;
class FFunnel;

class dtNavMesh;
class UPathFindingSubsystem;


UCLASS()
class CITYWALK_API ACityAIController : public AAIController
{
	GENERATED_BODY()

public:

	ACityAIController();

	~ACityAIController();
	
public:


	/* Subsystem responsible for finding the most optimal path */
	UPROPERTY(EditDefaultsOnly, Category = "PathFinding")
	UPathFindingSubsystem* PathFindingSubsystem;

	virtual void BeginPlay() override;	

};
