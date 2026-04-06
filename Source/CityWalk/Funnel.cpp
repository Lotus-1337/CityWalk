// Fill out your copyright notice in the Description page of Project Settings.


#include "Funnel.h"

#include "Portal.h"

FFunnel::FFunnel()
{

}

bool FFunnel::BuildFunnelPath(TArray<FVector>& OutArray, TArray<FPortal>& InArray) const
{

	if (InArray.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("In Array Is Empty | FFunnel::BuildFunnelPath"));
		return false;
	}

	if (!InArray[0].IsFake())
	{
		UE_LOG(LogTemp, Error, TEXT("The FIRST Element is Not a Fake Portal. "));
		UE_LOG(LogTemp, Warning, TEXT("First And Last Elements Have to Be Fake Portals For the Algorithm To Work. | FFunnel::BuildFunnelPath"));
		return false;
	}

	if (!InArray.Last().IsFake())
	{
		UE_LOG(LogTemp, Error, TEXT("The LAST Element is Not a Fake Portal"));
		UE_LOG(LogTemp, Warning, TEXT("First And Last Elements Have to Be Fake Portals For the Algorithm To Work. | FFunnel::BuildFunnelPath"));
		return false;
	}

	FVector Apex = InArray[0].Left;
	FVector Goal = InArray.Last().Left;

	FVector LeftBoundary  = Apex;
	FVector RightBoundary = Apex;

	// First Element is a Fake Portal, so we skip it.
	for (int32 i = 1; i < InArray.Num(); i++)
	{

		FPortal CurrentPortal = InArray[i];

		// Add Left And Add Right return True if Apex is changed
		// If it is, We add it to the OutArray.

		if (AddLeft(Apex, LeftBoundary, RightBoundary, CurrentPortal.Left)) OutArray.Add(Apex);

		if (Apex == Goal) break;
		
		if (AddRight(Apex, RightBoundary, LeftBoundary, CurrentPortal.Right)) OutArray.Add(Apex);

		if (Apex == Goal) break;

	}

	OutArray.Add(Goal);

	return true;

}

bool FFunnel::AddLeft(FVector& Apex, FVector& LeftBoundary, const FVector& RightBoundary, const FVector& NewLeftPoint) const
{

	bool IsNarrowing = Orient2D(Apex, LeftBoundary, NewLeftPoint) >= 0.0;

	if (!IsNarrowing) return false;

	bool IsIntersecting = Orient2D(Apex, RightBoundary, NewLeftPoint) > 0.0;

	if (!IsIntersecting)
	{
		LeftBoundary = NewLeftPoint;
		return false;
	}

	Apex = RightBoundary;
	return true;

}

bool FFunnel::AddRight(FVector& Apex, FVector& RightBoundary, const FVector& LeftBoundary, const FVector& NewRightPoint) const
{

	bool IsNarrowing = Orient2D(Apex, RightBoundary, NewRightPoint) <= 0.0;

	if (!IsNarrowing) return false;

	bool IsIntersecting = Orient2D(Apex, LeftBoundary, NewRightPoint) < 0.0;

	if (!IsIntersecting)
	{
		RightBoundary = NewRightPoint;
		return false;
	}

	Apex = LeftBoundary;
	return true;

}