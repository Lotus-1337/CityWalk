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

struct dtMeshTile;
struct dtPoly;

USTRUCT()
struct FTileInfo
{

	GENERATED_BODY()

	const dtPoly* MainPoly;
	const dtPoly* OtherPoly;

	const dtMeshTile* Tile;

	const dtNavMesh* Mesh;

	FTileInfo()
	{
		MainPoly = nullptr;
		OtherPoly = nullptr;

		Tile = nullptr;
	}

	// The 'n' prefix stands for 'New', to avoid using this-> or more confusing names
	FTileInfo(const dtPoly* nMainPoly, const dtPoly* nOtherPoly, const dtMeshTile* nTile, const dtNavMesh * nMesh)
	{
		MainPoly = nMainPoly;
		OtherPoly = nOtherPoly;

		Tile = nTile;
		Mesh = nMesh;
	}

	/**
	* Checks If Any Pointer contained by this struct is Invalid. *
	*/
	FORCEINLINE bool IsValid() const
	{
		return MainPoly != nullptr && OtherPoly != nullptr && Tile != nullptr && Mesh != nullptr;
	}

};