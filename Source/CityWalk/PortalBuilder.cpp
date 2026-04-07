// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalBuilder.h"

#include "Portal.h"
#include "PFHelper.h"
#include "PathFinder.h"

#include "Detour/DetourNavMesh.h"

// Sets default values
FPortalBuilder::FPortalBuilder()
{

}

FPortalBuilder::~FPortalBuilder()
{

}

FPortal FPortalBuilder::BuildPortal(FPolyInfo* PolyInfo)
{

	if (!ValidatePolyInfo(PolyInfo))
	{
		return FPortal();
	}


	const dtPoly* Neighbour = nullptr;

	for (int i = 0; i < DT_VERTS_PER_POLYGON; i++)
	{

		unsigned short NeighbourIndex = PolyInfo->MainHandle.Poly->neis[i];

		if (NeighbourIndex == 0)
		{
			continue;
		}

		if (NeighbourIndex & DT_EXT_LINK) // is the neighbour index outside of current tile? more expensive calculations
		{
			Neighbour = FPortalBuilder::GetPolyOutsideTile(PolyInfo, i);
		}
		else
		{
			Neighbour = &PolyInfo->Tile->polys[NeighbourIndex - 1];
		}

		if (Neighbour != PolyInfo->OtherHandle.Poly)
		{
			continue;
		}

		int32 VertIndex0 = PolyInfo->MainHandle.Poly->verts[i];
		int32 VertIndex1 = PolyInfo->MainHandle.Poly->verts[(i + 1) % PolyInfo->MainHandle.Poly->vertCount];

		FVector V0 = InvRealToVector(&PolyInfo->Tile->verts[VertIndex0 * 3]); // Inverted Conversions are useful here too
		FVector V1 = InvRealToVector(&PolyInfo->Tile->verts[VertIndex1 * 3]);

		return FPortal(V1, V0);


	}

	UE_LOG(LogTemp, Error, TEXT("Couldn't Find A Portal Between These Two Polys. Returning Default Portal"));

	return FPortal();

}

const dtPoly * FPortalBuilder::GetPolyOutsideTile(FPolyInfo* PolyInfo, const int32 &Index)
{

	if (!ValidatePolyInfo(PolyInfo, false))
	{
		UE_LOG(LogTemp, Error, TEXT("PolyInfo IsInvalid. FPortalBuilder::GetPolyOutsideTile"));
		return nullptr;
	}
	if (!PolyInfo->Tile->header)
	{
		UE_LOG(LogTemp, Error, TEXT("Tile Header Is Invalid. FPortalBuilder::GetPolyOutsideTile"));
		return 0;
	}

	for (unsigned int i = PolyInfo->MainHandle.Poly->firstLink; i != DT_NULL_LINK; i = PolyInfo->Tile->links[i].next)
	{

		const dtLink& Link = PolyInfo->Tile->links[i];

		// if the edge is not equal to the current iteration of Poly::neis[]
		// if they're equal then we found the neighbour.
		if (Link.edge != Index) 
		{
			continue;
		}

		dtPolyRef Ref = Link.ref;

		const dtPoly* Poly = nullptr;
		const dtMeshTile* Tile = nullptr;

		PolyInfo->Mesh->getTileAndPolyByRef(Ref, &Tile, &Poly);
	
		return Poly;

	}

	UE_LOG(LogTemp, Error, TEXT("The Said Neighbour of Index: %d Has not been Found. FPortalBuilder::GetPolyOutsideTile"), Index);

	return nullptr;

}

const dtPolyRef FPortalBuilder::GetRefOutsideTile(const FPolyInfo* PolyInfo, const int32& Index)
{

	if (!ValidatePolyInfo(PolyInfo, false))
	{
		UE_LOG(LogTemp, Error, TEXT("PolyInfo IsInvalid. FPortalBuilder::GetRefOutsideTile"));
		return 0;
	}
	if (!PolyInfo->Tile->header)
	{
		UE_LOG(LogTemp, Error, TEXT("Tile Header Is Invalid. FPortalBuilder::GetRefOutsideTile"));
		return 0;
	}

	for (unsigned int i = PolyInfo->MainHandle.Poly->firstLink; i != DT_NULL_LINK; i = PolyInfo->Tile->links[i].next)
	{

		const dtLink& Link = PolyInfo->Tile->links[i];

		// if the edge is not equal to the current iteration of Poly::neis[]
		// if they're equal then we found the neighbour.
		if (Link.edge != Index)
		{
			continue;
		}

		return Link.ref;

	}

	UE_LOG(LogTemp, Error, TEXT("The Said Neighbour of Index: %d Has not been Found. FPortalBuilder::GetRefOutsideTile"), Index);

	return 0;

}


bool FPortalBuilder::GetPortalPath(TArray<FPortal>& PortalPath, TArray<dtPolyRef>& RefArray, const APathFinder* PathFinder)
{

	if (!PathFinder)
	{

		UE_LOG(LogTemp, Error, TEXT("PathFinder is Invalid. Cannot Build Portals Path"));
		return false;

	}

	if (RefArray.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Given Ref Array Is Empty. "));
		return false;
	}

	const dtMeshTile* Tile = nullptr;
	const dtMeshTile* NeighbourTile = nullptr;


	const dtPoly* NodePoly = nullptr;
	const dtPoly* NeighbourPoly = nullptr;

	const dtNavMesh* DetourNavMesh = PathFinder->GetDetourMesh();

	for (int32 i = 0; i < RefArray.Num() - 1; i++)
	{

		dtPolyRef CurrRef = RefArray[i];
		dtPolyRef NextRef = RefArray[i + 1];

		DetourNavMesh->getTileAndPolyByRef(CurrRef, &Tile, &NodePoly);
		DetourNavMesh->getTileAndPolyByRef(NextRef, &NeighbourTile, &NeighbourPoly);

		FPolyHandle MainHandle  = FPolyHandle(CurrRef, NodePoly);
		FPolyHandle OtherHandle = FPolyHandle(NextRef, NeighbourPoly);

		FPolyInfo PolyInfo = FPolyInfo(MainHandle, OtherHandle, Tile, DetourNavMesh);

		if (!PolyInfo.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("Tile Info Is invalid. Might Implement FTInfoStatus"));
			continue;
		}

		PortalPath.Add(FPortalBuilder::BuildPortal(&PolyInfo));

	}

	return PortalPath.Num() > 0;

}

bool FPortalBuilder::ValidatePolyInfo(const FPolyInfo* PolyInfo, const bool& ShouldCheckOtherHandle)
{

	if (!PolyInfo)
	{
		UE_LOG(LogTemp, Error, TEXT("Tile Info Is nullptr. "));
		return false;
	}
	if (!PolyInfo->MainHandle.IsPolyValid() || (ShouldCheckOtherHandle && !PolyInfo->OtherHandle.IsPolyValid()))
	{
		UE_LOG(LogTemp, Error, TEXT("Either Main or OtherPoly is nullptr.  "));
		return false;
	}
	if (!PolyInfo->Tile)
	{
		UE_LOG(LogTemp, Error, TEXT("Tile is nullptr. "));
		return false;
	}
	if (!PolyInfo->Mesh)
	{
		UE_LOG(LogTemp, Error, TEXT("Mesh is nullptr. "));
		return false;
	}

	return true;

}