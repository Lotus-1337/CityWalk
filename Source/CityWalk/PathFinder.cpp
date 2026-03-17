// Fill out your copyright notice in the Description page of Project Settings.

#include "PathFinder.h"

#include "NavMesh/RecastNavMesh.h"
#include "NavigationSystem.h"

#include "Detour/DetourNavMesh.h"
#include "Detour/DetourNavMeshQuery.h"

#include "PortalBuilder.h"
#include "Portal.h"

FVector FPolyNode::CalculateCenter(const APathFinder* PathFinder) 
{ 
	return PathFinder->GetPolygonCentroid(&Ref); 
};

// Sets default values
APathFinder::APathFinder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APathFinder::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APathFinder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

const dtNavMesh* APathFinder::GetDetourMesh() const
{

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (!NavSystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Navigation System is invalid. Returning nullptr. APathFinder::GetDetourMesh"));
		return nullptr;
	}

	const ARecastNavMesh* RecastMesh = Cast<ARecastNavMesh>(NavSystem->GetDefaultNavDataInstance());

	if (!RecastMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("RecastMesh is invalid. Returning nullptr. APathFinder::GetDetourMesh"));
		return nullptr;
	}

	return RecastMesh->GetRecastMesh();

}

TArray<FPolyNode> APathFinder::FindPath(const FVector& StartingPosition, const FVector& FinishPosition)
{

	const dtNavMesh* DetourMesh = APathFinder::GetDetourMesh();

	if (!DetourMesh)
	{
		GetEmptyArray();
	}

	FVector DefaultExtent = FVector(200.0f, 200.0f, 200.0f);

	dtPolyRef FirstPolyRef = 0;
	dtPolyRef LastPolyRef = 0;

	dtPolyRef* StartPoly = &FirstPolyRef; 
	dtPolyRef* EndPoly = &LastPolyRef;

	GetClosestPoly(StartPoly, StartingPosition, DefaultExtent);
	GetClosestPoly(EndPoly, FinishPosition, DefaultExtent);

	if (!StartPoly || !EndPoly)
	{
		UE_LOG(LogTemp, Error, TEXT("Either Start Poly or End Poly is NULLPOINTER BLYAT'"));
		return GetEmptyArray();
	}

	bool IsStartValid = *StartPoly != 0;
	bool IsEndValid   = *EndPoly   != 0;

	UE_LOG(LogTemp, Warning, TEXT("Start ( %llu ) End ( %llu ). Start Valid %d, End Valid %d"), *StartPoly, *EndPoly, IsStartValid, IsEndValid);

	if (!IsStartValid || !IsEndValid)
	{
		UE_LOG(LogTemp, Error, TEXT("Start ( %d ) or End ( %d ) Poly is 0 ( invalid ). "), IsStartValid, IsEndValid);
		return GetEmptyArray();
	}

	FPolyNode StartNode = BuildNode(*StartPoly);
	FPolyNode LastNode = BuildNode(*EndPoly);

	FPolyNode* CurrentNode = &StartNode;
	FPolyNode* EndNode = &LastNode;

	AddPolyToMap(*StartPoly, *CurrentNode);
	AddPolyToMap(*EndPoly, *EndNode);

	TArray<dtPolyRef> NeighboursArr;

	TArray<FPolyNode*> OpenArr;
	TArray<FPolyNode*> OpenSet;

	TArray<FPolyNode*> ClosedSet;

	OpenArr.Heapify(FCompareNodes());

	OpenArr.HeapPush(CurrentNode, FCompareNodes());
	OpenSet.Add(CurrentNode);

	while (!OpenArr.IsEmpty())
	{

		OpenArr.HeapPop(CurrentNode, FCompareNodes());

		if (CurrentNode->GetRef() == EndNode->GetRef() || CurrentNode->GetIndex() == EndNode->GetIndex())
		{
			UE_LOG(LogTemp, Log, TEXT("Found Correct Path. "));
			return ReconstructPath(CurrentNode);
		}

		ClosedSet.Add(CurrentNode);
		OpenSet.Remove(CurrentNode);

		dtPolyRef Ref = CurrentNode->GetRef(); // GetRef() returns an rvalue ( one that is not in the memory )

		GetNeighbours(NeighboursArr, &Ref);

		for (dtPolyRef& NeighbourRef : NeighboursArr)
		{

			FPolyNode *Neighbour = PolyMap.Find(NeighbourRef);

			if (!Neighbour)
			{
				UE_LOG(LogTemp, Warning, TEXT("Neighbour is nullptr. "));
				continue;
			}

			if (ClosedSet.Contains(Neighbour))
			{
				continue;
			}

			bool IsInOpen = OpenSet.Contains(Neighbour);

			int32 NewG = CurrentNode->GetG() + FVector::Dist2D(CurrentNode->GetEntrance(), Neighbour->GetEntrance());

			if (IsInOpen && NewG > Neighbour->GetG())
			{
				continue;
			}

			if (IsInOpen)
			{
				OpenArr.Remove(Neighbour);
				OpenArr.Heapify(FCompareNodes());
			}

			Neighbour->SetG(NewG);
			Neighbour->SetH(FinishPosition);

			Neighbour->SetF();

			Neighbour->SetParentIndex(CurrentNode->GetIndex());

			OpenArr.HeapPush(Neighbour, FCompareNodes());
			OpenSet.Add(Neighbour);

		}

	}

	return GetEmptyArray();

}

TArray<FPolyNode> APathFinder::ReconstructPath(FPolyNode* LastNode)
{

	FPolyNode* CurrNode = LastNode;

	TArray<FPolyNode> Array;

	while (CurrNode)
	{

		Array.Add(*CurrNode);

		if (CurrNode->IsParentIdexValid() && PolyMap.Contains(CurrNode->GetParentIndex()))
		{
			CurrNode = &PolyMap[CurrNode->GetParentIndex()];
		
			continue;
		}
		
		CurrNode = nullptr;

	}

	ReverseArray(Array);

	return Array;

}

void APathFinder::SwapNodes(FPolyNode* Poly1, FPolyNode* Poly2)
{

	FPolyNode* Temp = Poly1;

	Poly1 = Poly2;
	Poly2 = Temp;

}

void APathFinder::ReverseArray(TArray<FPolyNode>& Array)
{

	int32 Size = Array.Num() * 0.5; // MULTIPLICATION BY 0.5 IS A TOUCH FASTER THAN DIVIDING BY 2

	for (int32 i = 0; i < Size; i++)
	{

		SwapNodes(&Array[0], &Array[Size - i - 1]);

	}

}

void APathFinder::GetClosestPoly(dtPolyRef * Poly, const FVector& Location, const FVector& Extent)
{

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (!NavSystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Navigation System is invalid. Returning nullptr. APathFinder::GetDetourMesh"));
		return;
	}

	const ARecastNavMesh* RecastMesh = Cast<ARecastNavMesh>(NavSystem->GetDefaultNavDataInstance());

	if (!RecastMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("RecastMesh is invalid. Returning nullptr. APathFinder::GetDetourMesh"));
		return;
	}



	dtNavMeshQuery Query = dtNavMeshQuery();
	Query.init(GetDetourMesh(), 2048);

	dtQueryFilter QueryFilter;

	dtReal Center[3] = { Location.X, Location.Y, Location.Z };
	dtReal ExtentReal[3] = { Extent.X, Extent.Y, Extent.Z };

	dtStatus Status = Query.findNearestPoly(Center, ExtentReal, &QueryFilter, Poly, nullptr);

	UE_LOG(LogTemp, Log, TEXT("Poly: %llu. Status Success: %d, Status Fail: %d"), *Poly, dtStatusSucceed(Status), dtStatusFailed(Status));

	if (!dtStatusSucceed(Status) || *Poly == 0)
	{
		Poly = nullptr;
	}

}

void APathFinder::GetNeighbours(TArray<dtPolyRef>& NeighboursArr, dtPolyRef* PolyRef)
{

	if (!NeighboursArr.IsEmpty())
	{
		NeighboursArr.Empty();
	}

	const dtPoly* Poly;
	const dtMeshTile* Tile;

	const dtNavMesh * DetourMesh = GetDetourMesh();

	if (!DetourMesh)
	{
		return;
	}

	DetourMesh->getTileAndPolyByRef(*PolyRef, &Tile, &Poly);

	if (!Poly)
	{
		UE_LOG(LogTemp, Error, TEXT("Poly is Invalid. APathFinder::GetNeighbours "));
		return;
	}

	if (!Tile)
	{
		UE_LOG(LogTemp, Error, TEXT("Tile is Invalid. APathFinder::GetNeighbours "));
		return;
	}

	for (unsigned int i = Poly->firstLink; i != DT_NULL_LINK; i = Tile->links[i].next)
	{

		dtPolyRef NeighbourRef = Tile->links[i].ref;

		const dtPoly* NeighbourPoly = nullptr;
		const dtMeshTile* OtherTile = nullptr;

		DetourMesh->getTileAndPolyByRef(NeighbourRef, &OtherTile, &NeighbourPoly);

		FTileInfo TileInfo = FTileInfo(Poly, NeighbourPoly, Tile, DetourMesh);


		FPortal Portal = FPortalBuilder::BuildPortal(&TileInfo); // Uncomment as soon as BuildPortal is implemented 

		NeighboursArr.Add(NeighbourRef);

		if (PolyMap.Contains(NeighbourRef))
		{
			continue;
		}

		FPolyNode NeighbourPolyNode = FPolyNode();

		NeighbourPolyNode.SetEntrance(Portal.GetPortalMiddle());

		AddPolyToMap(NeighbourRef, NeighbourPolyNode);

	}

}

void APathFinder::InitNode(dtPolyRef& Ref, FPolyNode& Node)
{
	Node.SetRef(Ref);
	//Node.CalculateCenter(this);
}

FPolyNode APathFinder::BuildNode(dtPolyRef& Ref)
{
	FPolyNode Node = FPolyNode();

	UE_LOG(LogTemp, Warning, TEXT("Ref: %llu"), Ref);

	InitNode(Ref, Node);
	
	return Node;
}

void APathFinder::AddPolyToMap(dtPolyRef& Ref, FPolyNode& Node)
{

	if (PolyMap.Contains(Ref))
	{
		return;
	}

	int32 Index = PolyMap.Num();

	InitNode(Ref, Node);

	Node.SetIndex(Index);

	PolyMap.Add(Ref, Node);

}

dtNavMeshQuery APathFinder::GetNavMeshQuery() const
{
	dtNavMeshQuery Query;
	Query.init(GetDetourMesh(), 2048);

	return Query;
}

FVector APathFinder::GetPolygonCentroid(dtPolyRef* Ref) const
{

	const dtPoly* Poly;
	const dtMeshTile* Tile;

	const dtNavMesh* DetourMesh = GetDetourMesh();

	if (!DetourMesh)
	{
		return FVector::ZeroVector;
	}

	FVector Center = FVector(0.0f, 0.0f, 0.0f);

	dtStatus Status = DetourMesh->getTileAndPolyByRef(*Ref, &Tile, &Poly);

	if (!Poly || !Tile || !dtStatusSucceed(Status))
	{
		UE_LOG(LogTemp, Log, TEXT("Poly Is invalid. "));
		return FVector::ZeroVector;
	}

	for (int32 i = 0; i < Poly->vertCount; ++i)
	{

		const int vertIndex = Poly->verts[i];
		dtReal* V = &Tile->verts[vertIndex * 3];

		// Converting From Detour to UE5.

		Center = RealToVector(V);

	}

	Center /= Poly->vertCount;

	return Center;

}