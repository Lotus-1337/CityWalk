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

	int32 ApexIndex = 0;
	
	int32 RightIndex = 0;
	int32 LeftIndex = 0;

	// First Element is a Fake Portal, so we skip it.
	for (int32 i = 1; i < InArray.Num(); i++)
	{

		if (Apex.Equals(Goal)) break;

		if (OutArray.Num() > 100)
		{
			UE_LOG(LogTemp, Error, TEXT("Out Array is larger than 100. Invalidating the result."));
			UE_LOG(LogTemp, Warning, TEXT("Apex Index : %d, RightIndex: %d, LeftIndex: %d"), ApexIndex, RightIndex, LeftIndex);
			return false;
		}

		FPortal CurrentPortal = InArray[i];

		// Add Left And Add Right return True if Apex is changed
		// If it is, We add it to the OutArray.

		EFunnelAddStatus RightStatus = AddRight(Apex, RightBoundary, LeftBoundary, CurrentPortal.Right);

		if (RightStatus == EFunnelAddStatus::INTERSECTING)
		{
			OutArray.Add(Apex);

			ApexIndex = LeftIndex;

			LeftBoundary = Apex;
			RightBoundary = Apex;

			i = FMath::Max(1, ApexIndex - 1);

			continue;

		}
		else if (RightStatus == EFunnelAddStatus::NARROWING)
		{
			RightIndex = i;
		}

		if (Apex.Equals(Goal)) break;

		EFunnelAddStatus LeftStatus = AddLeft(Apex, LeftBoundary, RightBoundary, CurrentPortal.Left);

		if (LeftStatus == EFunnelAddStatus::INTERSECTING)
		{
			OutArray.Add(Apex);

			ApexIndex = RightIndex;

			LeftBoundary = Apex;
			RightBoundary = Apex;

			i = FMath::Max(1, ApexIndex - 1);
		}
		else if (LeftStatus == EFunnelAddStatus::NARROWING)
		{
			LeftIndex = i;
		}



	}

	OutArray.Add(Goal);

	return true;

}


void FFunnel::ResetIndexes(int32& ApexIndex, int32& BoundaryIndex, int32& IterationIndex) const
{

	IterationIndex = ApexIndex;

	ApexIndex = BoundaryIndex;

}

static const double Epsilon = 0.5;

EFunnelAddStatus FFunnel::AddLeft(FVector& Apex, FVector& LeftBoundary, const FVector& RightBoundary, const FVector& NewLeftPoint) const
{

	double Cross = Orient2D(Apex, LeftBoundary, NewLeftPoint);

	if (FMath::Abs(Cross) < Epsilon)
	{
		return EFunnelAddStatus::COLINEAR;
	}

	bool IsNarrowing = Cross >= Epsilon;

	if (!IsNarrowing) return EFunnelAddStatus::EXPANDING;

	bool IsIntersecting = Orient2D(Apex, RightBoundary, NewLeftPoint) > Epsilon;

	if (IsIntersecting)
	{
		Apex = RightBoundary;
		return EFunnelAddStatus::INTERSECTING;
	}

	LeftBoundary = NewLeftPoint;
	return EFunnelAddStatus::NARROWING;

}

EFunnelAddStatus FFunnel::AddRight(FVector& Apex, FVector& RightBoundary, const FVector& LeftBoundary, const FVector& NewRightPoint) const
{

	double Cross = Orient2D(Apex, RightBoundary, NewRightPoint);

	if (FMath::Abs(Cross) < Epsilon)
	{
		return EFunnelAddStatus::COLINEAR;
	}

	bool IsNarrowing = Cross <= Epsilon;

	if (!IsNarrowing) return EFunnelAddStatus::EXPANDING;

	bool IsIntersecting = Orient2D(Apex, LeftBoundary, NewRightPoint) < Epsilon;

	if (IsIntersecting)
	{
		Apex = LeftBoundary;
		return EFunnelAddStatus::INTERSECTING;
	}

	RightBoundary = NewRightPoint;
	return EFunnelAddStatus::NARROWING;

}