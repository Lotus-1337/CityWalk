// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FPortal;

/**
 * 
 */
class CITYWALK_API FFunnel
{
public:
	
	FFunnel();

	/**
	* Builds the most optimal path using Funnel Algorithm 
	* 
	* @param OutArray: Array of All the Necessary points to visit 
	* @param InArray: Array of All the Portals needed for the algorithm
	* 
	* @returns Whether The Algorithm Succeeded
	**/
	bool BuildFunnelPath(TArray<FVector>& OutArray, TArray<FPortal>& InArray) const;

protected:


	void ResetApex(FVector& Apex, FVector& CrossingBoundary, FVector& OtherBoundary) const;

	void ResetIndexes(int32& ApexIndex, int32& BoundaryIndex, int32& IterationIndex) const;

	/**
	* If the New Left Point is narrowing the funnel, it becomes the LeftBoundary,
	* If it Intersects with Right Boundary, it becomes the Apex
	* @returns If Apex was changed
	*/
	bool AddLeft(FVector& Apex, FVector& LeftBoundary, const FVector& RightBoundary, const FVector& NewLeftPoint) const;

	/**
	* If the New Right Point is narrowing the funnel, it becomes the RightBoundary,
	* If it Intersects with Left Boundary, it becomes the Apex
	* @returns If Apex was changed
	*/
	bool AddRight(FVector& Apex, FVector& RightBoundary, const FVector& LeftBoundary, const FVector& NewRightPoint) const;


public:

	/**
	* Calculates A 2D CrossProduct of the given Vectors
	* 
	* @returns A positive value if C is on the Left side of AB
	* 
	*/
	FORCEINLINE constexpr static double Orient2D(const FVector& A, const FVector& B, const FVector& C)
	{
		return ((B.X - A.X) * (C.Y - A.Y))
			 - ((B.Y - A.Y) * (C.X - A.X));
	}

};
