// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/ActivityPointsSubsystem.h"

#include "ActivityPoint.h"

AActivityPoint* UActivityPointsSubsystem::GetAvailablePoint()
{

	for (AActivityPoint* Point : ActivityPoints)
	{
		if (Point->IsAvailable)
		{
			return Point;
		}
	}

	return nullptr;

}