// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalBuilder.h"

#include "Portal.h"
#include "PathFinder.h"

// Sets default values
FPortalBuilder::FPortalBuilder()
{


}

FPortal FPortalBuilder::BuildPortal(FTileInfo* TileInfo)
{

	if (!ValidateTileInfo(TileInfo))
	{
		return FPortal();
	}


	const dtPoly* Neighbour = nullptr;

	for (int i = 0; i < DT_VERTS_PER_POLYGON; i++)
	{

		unsigned short NeighbourIndex = TileInfo->MainPoly->neis[i];

		if (NeighbourIndex == 0)
		{
			continue;
		}

		if (NeighbourIndex & DT_EXT_LINK) // is the neighbour index outside of current tile? more expensive calculations
		{
			Neighbour = FPortalBuilder::GetPolyOutsideTile(TileInfo);
		}
		else
		{
			Neighbour = &TileInfo->Tile->polys[NeighbourIndex - 1];
		}

		if (Neighbour != TileInfo->OtherPoly)
		{
			continue;
		}

		int32 VertIndex0 = TileInfo->MainPoly->verts[i];
		int32 VertIndex1 = TileInfo->MainPoly->verts[(i + 1) % TileInfo->MainPoly->vertCount];

		FVector V0 = RealToVector(&TileInfo->Tile->verts[VertIndex0 * 3]);
		FVector V1 = RealToVector(&TileInfo->Tile->verts[VertIndex1 * 3]);

		return FPortal(V1, V0);


	}

	UE_LOG(LogTemp, Error, TEXT("Couldn't Find A Portal Between These Two Polys. Returning Default Portal"));

	return FPortal();

}

const dtPoly * FPortalBuilder::GetPolyOutsideTile(const FTileInfo* TileInfo)
{

	for (unsigned int i = TileInfo->MainPoly->firstLink; i != DT_NULL_LINK; i = TileInfo->Tile->links[i].next)
	{

		dtPolyRef Ref = TileInfo->Tile->links[i].ref;

		const dtPoly* Poly = nullptr;

		const dtMeshTile* Tile = nullptr;

		TileInfo->Mesh->getTileAndPolyByRef(Ref, &Tile, &Poly);

		if (Poly == TileInfo->OtherPoly)
		{
			return Poly;
		}

	}

	return nullptr;

}


bool FPortalBuilder::GetPortalPath(TArray<FPortal>& PortalPath, TArray<FPolyNode>& NodeArray, const APathFinder* PathFinder)
{

	if (!PathFinder)
	{

		UE_LOG(LogTemp, Error, TEXT("PathFinder is Invalid. Cannot Build Portals Path"));
		return false;

	}

	if (!PortalPath.IsEmpty())
	{
		PortalPath.Empty();
	}

	const dtMeshTile* Tile = nullptr;
	const dtMeshTile* NeighbourTile = nullptr;


	const dtPoly* NodePoly = nullptr;
	const dtPoly* NeighbourPoly = nullptr;

	const dtNavMesh* DetourNavMesh = PathFinder->GetDetourMesh();

	const FPolyNode* CurrNode;
	const FPolyNode* NextNode;

	for (int32 i = 0; i < NodeArray.Num() - 1; i++)
	{

		CurrNode = &NodeArray[i];
		NextNode = &NodeArray[i + 1];

		DetourNavMesh->getTileAndPolyByRef(CurrNode->GetRef(), &Tile, &NodePoly);
		DetourNavMesh->getTileAndPolyByRef(NextNode->GetRef(), &NeighbourTile, &NeighbourPoly);

		FTileInfo TileInfo = FTileInfo(NodePoly, NeighbourPoly, Tile, DetourNavMesh);

		if (!TileInfo.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("Tile Info Is invalid. Might Implement FTInfoStatus"));
			continue;
		}

		PortalPath.Add(FPortalBuilder::BuildPortal(&TileInfo));

	}

	return PortalPath.Num() > 0;

}

bool FPortalBuilder::ValidateTileInfo(const FTileInfo* TileInfo)
{

	if (!TileInfo)
	{
		UE_LOG(LogTemp, Error, TEXT("Tile Info Is nullptr. "));
		return false;
	}
	if (!TileInfo->MainPoly || !TileInfo->OtherPoly)
	{
		UE_LOG(LogTemp, Error, TEXT("Either Main or OtherPoly is nullptr.  "));
		return false;
	}
	if (!TileInfo->Tile)
	{
		UE_LOG(LogTemp, Error, TEXT("Tile is nullptr. "));
		return false;
	}
	if (!TileInfo->Mesh)
	{
		UE_LOG(LogTemp, Error, TEXT("Mesh is nullptr. "));
		return false;
	}

	return true;

}