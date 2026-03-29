#pragma once

#include "CoreMinimal.h"

#include "Detour/DetourNavMesh.h"

#include "PFHelper.generated.h"

struct dtMeshTile;
struct dtPoly;

USTRUCT()
struct FPolyHandle
{
	GENERATED_BODY()

	dtPolyRef Ref;
	const dtPoly* Poly;

	FPolyHandle()
	{
		Ref = 0;
		Poly = nullptr;
	}

	FPolyHandle(const dtPolyRef& nRef, const dtPoly* nPoly)
	{
		Ref = nRef;
		Poly = nPoly;
	}

	FORCEINLINE bool IsRefValid() const { return Ref != 0; };

	FORCEINLINE bool IsPolyValid() const { return Poly != nullptr; };

	FORCEINLINE bool IsValidBoth() const { return IsRefValid() && IsPolyValid(); };

};

/** Struct Containing Tile Info For Looking For Polys **/
USTRUCT()
struct FPolyInfo
{

	GENERATED_BODY()

	FPolyHandle MainHandle;
	FPolyHandle OtherHandle;

	const dtMeshTile* Tile;

	const dtNavMesh* Mesh;

	FPolyInfo()
	{

		MainHandle  = FPolyHandle();
		OtherHandle = FPolyHandle();

		Tile = nullptr;
		Mesh = nullptr;
	}

	// The 'n' prefix stands for 'New', to avoid using this-> or more confusing names
	FPolyInfo(const FPolyHandle & nMainHandle, const FPolyHandle & nOtherHandle, const dtMeshTile* nTile, const dtNavMesh* nMesh)
	{

		MainHandle  = nMainHandle;
		OtherHandle = nOtherHandle;

		Tile = nTile;
		Mesh = nMesh;
	}

	/**
	* Checks If Any Pointer contained by this struct is Invalid. *
	*/
	FORCEINLINE bool IsValid() const
	{
		return MainHandle.IsValidBoth() && OtherHandle.IsValidBoth() && Tile != nullptr && Mesh != nullptr;
	}

};

/**
*
* @returns A FVector Converted From dtReal
* 
*/ 
FORCEINLINE FVector RealToVector(const dtReal* V)
{
	// X -> X, Z -> Y, Y -> Z
	return FVector(V[0], V[2], V[1]);
}

/**
* @param Out: Ouputs a Real Converted From a Vector
*/
FORCEINLINE void VectorToReal(const FVector& V, dtReal Out[3])
{
	// X -> X, Y -> Z, Z -> Y
	Out[0] = V.X;
	Out[1] = V.Z;
	Out[2] = V.Y;
}

/**
* Use When X and Y Axis Are Inverted
* @return Returns a Vector with Inverted X and Y Axis Converted From Real 
**/
FORCEINLINE FVector InvRealToVector(const dtReal* V)
{
	// X -> X, Z -> Y, Y -> Z
	return FVector(-V[0], -V[2], V[1]);
}

/**
* Use When X and Y Axis Are Inverted.
* 
* @param Out: Ouputs a Real Converted From a Vector with Inverted X and Y Axis
**/
FORCEINLINE void InvVectorToReal(const FVector& V, dtReal Out[3])
{
	// X -> X, Y -> Z, Z -> Y
	Out[0] = -V.X;
	Out[1] = V.Z;
	Out[2] = -V.Y;
}

UENUM()
enum class EWhichHandle
{
	MAIN,
	OTHER
};

/** Returns Poly Ref From PolyInfo's MainHandle Poly **/
FORCEINLINE dtPolyRef GetPolyRef(const FPolyInfo& PolyInfo, const EWhichHandle& Handle)
{
	const dtPoly* Poly = Handle == EWhichHandle::MAIN ? PolyInfo.MainHandle.Poly : PolyInfo.OtherHandle.Poly;

	int32 PolyIndex = int32(Poly - PolyInfo.Tile->polys);
	return PolyInfo.Mesh->getPolyRefBase(PolyInfo.Tile) + PolyIndex;
}