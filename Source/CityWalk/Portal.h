#pragma once


#include "CoreMinimal.h"
#include "Portal.generated.h"

USTRUCT()
struct FPortal
{

	GENERATED_BODY()

	FVector Left;
	FVector Right;

	FORCEINLINE FPortal()
	{
		Left = FVector::ZeroVector;
		Right = FVector::ZeroVector;
	}

	FORCEINLINE FPortal(const FVector& nLeft, const FVector& nRight)
	{
		Left = nLeft;
		Right = nRight;
	}

	FORCEINLINE FVector GetPortalMiddle() const
	{
		return (Left + Right) * 0.5; // multiplication by 0.5 is slightly faster than dividing by 2
	}

	/**
	* Builds a Fake Portal ( One where Left == Right )
	* 
	* @returns Fake Portal with Edges Location : Location
	* 
	*/
	FORCEINLINE static FPortal FakePortal(const FVector& Location)
	{
		return FPortal(Location, Location);
	}
			
	FORCEINLINE bool IsFake() const
	{
		return Left == Right;
	}

};