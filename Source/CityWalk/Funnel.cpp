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

	FPortal* LastPortal = &Apex;
	FPortal* CurrentPortal = nullptr;

	// We're Skipping the first element
	for (int32 i = 1; i < InArray.Num(); i++)
	{

		CurrentPortal = &InArray[i];

		bool ShouldAddRight = CurrentPortal->Left == LastPortal->Left;

		FVector P = ShouldAddRight ? CurrentPortal->Right : CurrentPortal->Left;

		LastPortal = CurrentPortal;

		if (ShouldAddRight)
		{
			AddRight(OutArray, P);
			break;
		}

		AddLeft(OutArray, P);

	}

	return true;

}


bool FFunnel::AddLeft(TArray<FVector>& OutArray, const FVector& V) const
{

	return true;

}

bool FFunnel::AddRight(TArray<FVector>& OutArray, const FVector& V) const
{

	return true;

}