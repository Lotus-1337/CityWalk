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

	FPortal Apex = InArray[0];
	FPortal Goal = InArray.Last();

	FVector* Left  = &Apex.Left;
	FVector* Right = &Apex.Right;

	// We're Skipping the first element
	for (FPortal& CurrentPortal : InArray)
	{



	}

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