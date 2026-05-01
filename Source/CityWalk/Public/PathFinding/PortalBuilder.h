// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FPortal;
struct FPolyNode;
struct FPolyInfo;
struct dtPoly;
class APathFinder;

typedef uint64_t UEType_uint64;
typedef UEType_uint64 dtPolyRef;

class CITYWALK_API FPortalBuilder
{
	
public:	
	// Sets default values for this actor's properties
	FPortalBuilder();

	~FPortalBuilder();

	/**
	* This Method Builds a Portal From Tile's Info
	* @param TileInfo: Structure containing Tile and 2 Poly Handles
	* @return Returns a Valid Portal if Polys specified in TileInfo are Neighbours
	*/
	static FPortal BuildPortal(FPolyInfo * TileInfo);

	/**
	* If the neighbour is outside the current tile, it's searched in other tiles
	* 
	* This method of searching for the Poly is significantly slower 
	* 
	* @param PolyInfo: Structure containing Tile and 2 Poly Handles necessary for getting the Neighbour by dtPoly::links[] 
	* 
	* @param Index: We can check if we found the right link if link.edge == Index
	* 
	* @return Neighbouring Poly if Found
	*/
	static const dtPoly* GetPolyOutsideTile(FPolyInfo* PolyInfo, const int32 & Index);

	/**
	* If the neighbour is outside the current tile, it's searched in other tiles
	* 
	* This Version Returns Neighbour's Ref
	*
	* This method of searching for the Poly is significantly slower
	*
	* @param PolyInfo: Structure containing Tile and 2 Poly Handles necessary for getting the Neighbour by dtPoly::links[]
	*
	* @param Index: We can check if we found the right link if link.edge == Index
	*
	* @return Neighbouring PolyRef if Found
	*/
	static const dtPolyRef GetRefOutsideTile(const FPolyInfo* PolyInfo, const int32& Index);

	bool GetPortalPath(TArray<FPortal>& PortalPath, TArray<dtPolyRef>& InArray, const APathFinder* PathFinder);

	static bool ValidatePolyInfo(const FPolyInfo* PolyInfo, const bool& ShouldCheckOtherHandle = true);
	
};

