// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Detour/DetourNavMesh.h"

struct FPortal;
struct FTileInfo;
struct FPolyNode;
class APathFinder;

class CITYWALK_API FPortalBuilder
{
	
public:	
	// Sets default values for this actor's properties
	FPortalBuilder();

	~FPortalBuilder();

	/**
	* This Method Builds a Portal From Tile's Info
	* @param TileInfo: Structure containing Tile and 2 Poly Pointers
	* @return Returns a Valid Portal if Polys specified in TileInfo are Neighbours
	*/
	static FPortal BuildPortal(FTileInfo * TileInfo);

	/**
	* If the neighbour is outside the current tile, it's searched in other tiles
	* 
	* This method of searching for the Poly is significantly slower 
	* @param TileInfo: Structure containing Tile and 2 Poly Pointers necessary for getting the Neighbour by dtPoly::links[] 
	* @return Neighbouring Poly if Found
	* This method of searching for the Poly is significantly slower  */
	static const dtPoly* GetPolyOutsideTile(const FTileInfo* TileInfo);

	bool GetPortalPath(TArray<FPortal>& PortalPath, TArray<FPolyNode>& NodeArray, const APathFinder* PathFinder);

	static bool ValidateTileInfo(const FTileInfo* TileInfo);
	
};


// Converting From Detour to UE5.
FORCEINLINE FVector RealToVector(const dtReal* V)
{
	// X -> X, Z -> Y, Y -> Z
	return FVector(V[0], V[2], V[1]);
}