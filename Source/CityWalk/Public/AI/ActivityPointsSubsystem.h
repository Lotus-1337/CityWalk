// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ActivityPointsSubsystem.generated.h"

class AActivityPoint;

/**
 * 
 */
UCLASS()
class CITYWALK_API UActivityPointsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Activity Points")
	TArray<AActivityPoint*> ActivityPoints;

public:

	/**
	* Method returning an available point. 
	* @returns The first available Point of Interest, or nullptr if none is found
	*/
	AActivityPoint* GetAvailablePoint();

	FORCEINLINE AActivityPoint* GetActivityPoint(const int32& Index)
	{
		return ActivityPoints.IsValidIndex(Index) ? ActivityPoints[Index] : nullptr;
	}

	FORCEINLINE void AddActivityPoint(AActivityPoint* Point)
	{
		ActivityPoints.Add(Point);
	}

};
