#include "PFHelper.h"

#include "Detour/DetourNavMesh.h"

dtPolyRef GetPolyRef(const FPolyInfo& PolyInfo, const EWhichHandle& Handle)
{
	const dtPoly* Poly = Handle == EWhichHandle::MAIN ? PolyInfo.MainHandle.Poly : PolyInfo.OtherHandle.Poly;

	int32 PolyIndex = int32(Poly - PolyInfo.Tile->polys);
	return PolyInfo.Mesh->getPolyRefBase(PolyInfo.Tile) + PolyIndex;
}

FVector InvTileVertToVector(const dtMeshTile* Tile, dtTileVert* TV)
{
	dtReal V[3];

	TV->toWorld(Tile->header->bmin, V);
	return InvRealToVector(V);
}