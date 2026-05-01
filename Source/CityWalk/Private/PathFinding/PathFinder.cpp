// Fill out your copyright notice in the Description page of Project Settings.

#include "PathFinder.h"

#include "NavMesh/RecastNavMesh.h"
#include "NavigationSystem.h"

#include "Detour/DetourNavMesh.h"
#include "Detour/DetourNavMeshQuery.h"

#include "Trace/Trace.h"

#include "PortalBuilder.h"
#include "Portal.h"
#include "PFHelper.h"

DEFINE_LOG_CATEGORY(LogProfiler);

FVector FPolyNode::CalculateCenter(const APathFinder* PathFinder) 
{ 
	return PathFinder->GetPolygonCentroid(&Ref); 
};

// Sets default values
APathFinder::APathFinder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PolySet = TSparseSet<dtPolyRef, Index_t, FPolyNode>(1024);


}

// Called when the game starts or when spawned
void APathFinder::BeginPlay()
{
	Super::BeginPlay();	

	NodesToClean.Reserve(256);

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

bool APathFinder::FindPath(TArray<dtPolyRef> & OutArray, const FVector& StartingPosition, const FVector& FinishPosition)
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
	Index_t StartIndex = PolySet.TryAdd(StartRef, StartNode);
	
	FPolyNode LastNode;
	LastNode.Entrance = FinishPosition;
	LastNode.G = 0;
	LastNode.Ref = EndRef;
	Index_t EndIndex = PolySet.TryAdd(EndRef, LastNode);

	FPolyNode* CurrentNode = PolySet.GetByIndex(StartIndex);
	FPolyNode* EndNode	   = PolySet.GetByIndex(EndIndex);

	TArray<Index_t> NeighboursArr;
	NeighboursArr.Reserve(16);
	
	TArray<FOpenNode> OpenArr;
	OpenArr.Reserve(256);

	OpenArr.Heapify(FCompareNodes());

	OpenArr.HeapPush(FOpenNode(CurrentNode, CurrentNode->F, CurrentNode->G), FCompareNodes());

	NodesToClean.Add(CurrentNode);

	int32 Iterations = 0;

	FOpenNode CurrTempNode;

	while (!OpenArr.IsEmpty())
	{

		OpenArr.HeapPop(CurrTempNode, FCompareNodes());

		if (!CurrTempNode.IsValid())
		{
			continue;
		}

		NeighbourCount++;

		CurrentNode = CurrTempNode.Node;

		if (CurrentNode->Ref == EndNode->Ref)
		{
			return ReconstructPath(OutArray, CurrentNode);
		}

		CurrentNode->IsInOpen = false;
		CurrentNode->IsInClosed = true;

		dtPolyRef Ref = CurrentNode->Ref; // Ref returns an rvalue ( one that is not in the memory )


		GetNeighbours(NeighboursArr, Ref);

		for (Index_t& NeighbourIdx : NeighboursArr)
		{

			Iterations++;

			FPolyNode *Neighbour = PolySet.GetByIndex(NeighbourIdx);
			
			if (!Neighbour)
			{
				UE_LOG(LogTemp, Error, TEXT("APathFinder::FindPath: A Neighbour in NeighboursArray was not found in PolyMap. "));
				continue;
			}
			
			if (Neighbour->IsInClosed)
			{
				continue;
			}

			bool IsInOpen = Neighbour->IsInOpen;

			float NewG = CurrentNode->G + FVector::DistSquared2D(CurrentNode->Entrance, Neighbour->Entrance);
	
			if (IsInOpen && NewG > Neighbour->G)
			{
				continue;
			}

			Neighbour->G = NewG;
			float H = Neighbour->CalculateH(FinishPosition);

			static const float Weight = 1.2f;

			Neighbour->SetF(H, Weight);

			Neighbour->ParentIndex = CurrentNode->Index;

			Neighbour->IsInOpen = true;

			// Adding a new OpenNode. Neighbour->G has been changed, so if a node with THIS FPolyNode* Neighbour
			// is old now. It's being skipped. 
			OpenArr.HeapPush(FOpenNode(Neighbour, Neighbour->F, Neighbour->G), FCompareNodes());

			NodesToClean.Add(Neighbour);

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
		
		CurrNode = PolySet.GetByIndex(CurrNode->ParentIndex);

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

void APathFinder::GetNeighbours(TArray<Index_t>& NeighboursArr, const dtPolyRef& PolyRef)
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

		Index_t NeighbourIndexInPolySet = PolySet.TryAdd(NeighbourRef, NeighbourPolyNode);

		PolySet.GetByIndex(NeighbourIndexInPolySet)->Index = NeighbourIndexInPolySet;

		NeighboursArr.Add(NeighbourIndexInPolySet);
	
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

void APathFinder::CleanNodes()
{

	for (FPolyNode* Node : NodesToClean)
	{

		Node->Reset();

	}

	NodesToClean.Reset();

}