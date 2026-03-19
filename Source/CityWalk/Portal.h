#pragma once


#include "CoreMinimal.h"
#include "Portal.generated.h"

USTRUCT()
struct FPortal
{

	GENERATED_BODY()

	FVector Edge1;
	FVector Edge2;

	FPortal()
	{
		Edge1 = FVector::ZeroVector;
		Edge2 = FVector::ZeroVector;
	}

	FPortal(const FVector& V1, const FVector& V2)
	{
		Edge1 = V1;
		Edge2 = V2;
	}

	FVector GetPortalMiddle() const
	{
		return (Edge1 + Edge2) * 0.5; // multiplication by 0.5 is slightly faster than dividing by 2
	}

};