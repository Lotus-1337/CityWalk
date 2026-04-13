// Fill out your copyright notice in the Description page of Project Settings.

#include "PathFinder.h"

#include "NavMesh/RecastNavMesh.h"
#include "NavigationSystem.h"

#include "Detour/DetourNavMesh.h"
#include "Detour/DetourNavMeshQuery.h"

#include "PortalBuilder.h"
#include "Portal.h"
#include "PFHelper.h"

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

	PolyMap.Reserve(1024);
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

bool APathFinder::AStar(TArray<dtPolyRef> & OutArray, const FVector& StartingPosition, const FVector& FinishPosition)
{

	OutArray.Reset();
	OutArray.Reserve(64);

	NeighbourCount = 0;

	const dtNavMesh* DetourMesh = APathFinder::GetDetourMesh();

	if (!DetourMesh)
	{
		GetEmptyArray();
	}

	FVector DefaultExtent = FVector(100.0f, 100.0f, 200.0f);

	dtPolyRef StartRef = 0;
	dtPolyRef EndRef = 0;

	GetClosestPoly(&StartRef, StartingPosition, DefaultExtent);
	GetClosestPoly(&EndRef, FinishPosition, DefaultExtent);

	bool IsStartValid = StartRef != 0;
	bool IsEndValid   = EndRef   != 0;

	if (!IsStartValid || !IsEndValid)
	{
		UE_LOG(LogTemp, Error, TEXT("Start ( %d ) or End ( %d ) Poly is 0 ( invalid ). "), IsStartValid, IsEndValid);
		return false;
	}

	FPolyNode StartNode;
	StartNode.Entrance = StartingPosition;
	StartNode.G = 0;
	StartNode.Ref = StartRef;
	AddPolyToMap(StartRef, StartNode);
	
	FPolyNode LastNode;
	LastNode.Entrance = FinishPosition;
	LastNode.G = 0;
	LastNode.Ref = EndRef;
	AddPolyToMap(EndRef, LastNode);

	FPolyNode* CurrentNode = PolyMap.Find(StartRef);
	FPolyNode* EndNode = PolyMap.Find(EndRef);

	TArray<dtPolyRef> NeighboursArr;
	NeighboursArr.Reserve(16);
	
	TArray<FPolyNode*> OpenArr;
	OpenArr.Reserve(256);

	TSet<dtPolyRef> OpenSet;
	OpenSet.Reserve(256);

	TSet<dtPolyRef> ClosedSet;
	ClosedSet.Reserve(128);

	OpenArr.Heapify(FCompareNodes());

	OpenArr.HeapPush(CurrentNode, FCompareNodes());
	OpenSet.Add(CurrentNode->Ref);

	int32 Iterations = 0;

	while (!OpenArr.IsEmpty())
	{

		OpenArr.HeapPop(CurrentNode, FCompareNodes());

		NeighbourCount++;

		if (CurrentNode->Ref == EndNode->Ref)
		{
			UE_LOG(LogTemp, Log, TEXT("Found Correct Path. "));
			return ReconstructPath(OutArray, CurrentNode);
		}

		ClosedSet.Add(CurrentNode->Ref);
		OpenSet.Remove(CurrentNode->Ref);

		dtPolyRef Ref = CurrentNode->Ref; // Ref returns an rvalue ( one that is not in the memory )

		GetNeighbours(NeighboursArr, Ref);

		for (dtPolyRef& NeighbourRef : NeighboursArr)
		{

			Iterations++;

			FPolyNode *Neighbour = PolyMap.Find(NeighbourRef);
			
			if (!Neighbour)
			{
				UE_LOG(LogTemp, Error, TEXT("APathFinder::FindPath: A Neighbour in NeighboursArray was not found in PolyMap. "));
				continue;
			}
			
			if (ClosedSet.Contains(Neighbour->Ref))
			{
				continue;
			}

			bool IsInOpen = OpenSet.Contains(Neighbour->Ref);

			int32 NewG = CurrentNode->G + FVector::Dist2D(CurrentNode->Entrance, Neighbour->Entrance);
	
			if (IsInOpen && NewG > Neighbour->G)
			{
				continue;
			}

			if (IsInOpen)
			{
				OpenArr.Remove(Neighbour);
				OpenArr.Heapify(FCompareNodes());
				IsInOpen = false;
			}

			Neighbour->G = NewG;
			int32 H = Neighbour->CalculateH(FinishPosition);

			static const float Weight = 1.2f;

			Neighbour->SetF(H, Weight);

			Neighbour->ParentRef = CurrentNode->Ref;

			OpenArr.HeapPush(Neighbour, FCompareNodes());

			if (!IsInOpen) OpenSet.Add(Neighbour->Ref);

		}

	}

	return false;

}

bool APathFinder::ReconstructPath(TArray<dtPolyRef>& OutArray, const FPolyNode* LastNode)
{

	const FPolyNode* CurrNode = LastNode;


	while (CurrNode)
	{

		OutArray.Add(CurrNode->Ref);

		if (!CurrNode->IsParentRefValid() || !PolyMap.Contains(CurrNode->ParentRef))
		{
			break;
		}
		
		CurrNode = PolyMap.Find(CurrNode->ParentRef);

	}

	ReverseArray(OutArray);
	
	return true;

}

void APathFinder::SwapNodes(dtPolyRef& Node1, dtPolyRef &Node2)
{

	dtPolyRef Temp = Node1;

	Node1 = Node2;
	Node2 = Temp;

}

void APathFinder::ReverseArray(TArray<dtPolyRef>& OutArray)
{

	int32 Size = OutArray.Num() * 0.5; // MULTIPLICATION BY 0.5 IS A TOUCH FASTER THAN DIVIDING BY 2

	for (int32 i = 0; i < Size; i++)
	{

		SwapNodes(OutArray[i], OutArray[OutArray.Num() - i - 1]);

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

	dtReal Center[3];
	dtReal ExtentReal[3];

	dtReal NearestPt[3];

	//VectorToReal(Location, Center);
	InvVectorToReal(Location, Center);
	VectorToReal(Extent, ExtentReal);

	dtStatus Status = Query.findNearestPoly(Center, ExtentReal, &QueryFilter, Poly, NearestPt);

	if (!dtStatusSucceed(Status) || *Poly == 0)
	{
		*Poly = 0;
	}

}

void APathFinder::GetNeighbours(TArray<dtPolyRef>& NeighboursArr, const dtPolyRef& PolyRef)
{

	NeighboursArr.Empty();

	const dtPoly* Poly = nullptr;
	const dtMeshTile* Tile = nullptr;;

	const dtNavMesh * DetourMesh = GetDetourMesh();

	if (!DetourMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("Nav mesh Is Invalid. APathFinder::GetNeighbours. "));
		return;
	}

	dtStatus Status = DetourMesh->getTileAndPolyByRef(PolyRef, &Tile, &Poly);

	if (!dtStatusSucceed(Status))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Ref Has been given. Status Hasn't Succeeded. "));
		UE_LOG(LogTemp, Warning, TEXT("Is The Given Ref %llu Valid? %d"), PolyRef, DetourMesh->isValidPolyRef(PolyRef));
		return;
	}

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

	if (!Tile->header)
	{
		UE_LOG(LogTemp, Error, TEXT("Tile header Is Invalid. APathFinder::GetNeighbours "));
		return;
	}


	FPolyHandle MainHandle = FPolyHandle(PolyRef, Poly);
	FPolyHandle OtherHandle = FPolyHandle(); // Other Handle HAS to be invalid for now

	FPolyInfo PolyInfo = FPolyInfo(MainHandle, OtherHandle, Tile, DetourMesh);

	const dtPoly* Neighbour = nullptr;
	const dtMeshTile* NeighbourTile = nullptr;

	dtPolyRef NeighbourRef = 0;

	for (int i = 0; i < Poly->vertCount; i++)
	{

		unsigned short NeighbourIndex = PolyInfo.MainHandle.Poly->neis[i];

		//UE_LOG(LogTemp, Log, TEXT("Neighbour Index: %d"), NeighbourIndex);

		if (NeighbourIndex == 0)
		{
			continue;
		}

		bool IsOutsideTile = false;

		if (NeighbourIndex & DT_EXT_LINK) // is the neighbour index outside of current tile? more expensive calculations
		{
			NeighbourRef = FPortalBuilder::GetRefOutsideTile(&PolyInfo, i);
			DetourMesh->getTileAndPolyByRef(NeighbourRef, &NeighbourTile, &Neighbour);
			IsOutsideTile = true;
		}
		else
		{
			// If true, poly is trash 
			if (NeighbourIndex - 1 >= Tile->header->polyCount)
			{
				UE_LOG(LogTemp, Log, TEXT("Neighbour Index is above PolyCount"));
				continue;
			}


			Neighbour = &PolyInfo.Tile->polys[NeighbourIndex - 1];

			PolyInfo.OtherHandle.Poly = Neighbour;

			NeighbourRef = GetPolyRef(PolyInfo, EWhichHandle::OTHER);
		}

		if (NeighbourRef == 0) continue;

		PolyInfo.OtherHandle.Ref = NeighbourRef;
		PolyInfo.OtherHandle.Poly = Neighbour;

		FPortal Portal = FPortalBuilder::BuildPortal(&PolyInfo);
		FPolyNode NeighbourPolyNode = FPolyNode();

		NeighbourPolyNode.Entrance = Portal.GetPortalMiddle();
		NeighbourPolyNode.Ref = NeighbourRef;

		NeighboursArr.Add(NeighbourRef);
		AddPolyToMap(NeighbourRef, NeighbourPolyNode);

	
	}

}

void APathFinder::InitNode(dtPolyRef& Ref, FPolyNode& Node)
{
	Node.Ref = Ref;
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

	if (PolyMap.Contains(Ref) || PolyMap.Num() > 10e2)
	{
		return;
	}

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

		Center += InvRealToVector(V);

	}

	Center /= Poly->vertCount;

	return Center;

}

void APathFinder::CleanMap()
{

	for (auto& Node : PolyMap)
	{

		Node.Value.Reset();

	}

}