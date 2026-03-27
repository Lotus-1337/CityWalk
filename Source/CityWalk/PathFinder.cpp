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

	FVector DefaultExtent = FVector(200.0f, 200.0f, 1000.0f);

	dtPolyRef FirstPolyRef = 0;
	dtPolyRef LastPolyRef = 0;

	dtPolyRef* StartRef = &FirstPolyRef; 
	dtPolyRef* EndRef = &LastPolyRef;

	GetClosestPoly(StartRef, StartingPosition, DefaultExtent);
	GetClosestPoly(EndRef, FinishPosition, DefaultExtent);

	bool IsStartValid = *StartRef != 0;
	bool IsEndValid   = *EndRef   != 0;

	UE_LOG(LogTemp, Warning, TEXT("Start ( %llu ) End ( %llu ). Start Valid %d, End Valid %d"), *StartRef, *EndRef, IsStartValid, IsEndValid);

	if (!IsStartValid || !IsEndValid)
	{
		UE_LOG(LogTemp, Error, TEXT("Start ( %d ) or End ( %d ) Poly is 0 ( invalid ). "), IsStartValid, IsEndValid);
		return GetEmptyArray();
	}

	FPolyNode StartNode;
	StartNode.SetEntrance(StartingPosition);
	StartNode.SetG(0);
	StartNode.SetRef(*StartRef);
	AddPolyToMap(*StartRef, StartNode);

	FPolyNode LastNode;
	LastNode.SetEntrance(FinishPosition);
	LastNode.SetG(0);
	LastNode.SetRef(*EndRef);
	AddPolyToMap(*EndRef, LastNode);

	TArray<dtPolyRef> NeighboursArray;

	GetNeighbours(NeighboursArray, *StartRef);

	int32 Index = 0;

	for (dtPolyRef& NeiRef : NeighboursArray)
	{

		FPolyNode* Neighbour = PolyMap.Find(NeiRef);

		if (!Neighbour)
		{
			UE_LOG(LogTemp, Error, TEXT("APathFinder::FindPath : Neighbour is nullptr. "));
			continue;
		}

		UE_LOG(LogTemp, Log, TEXT("Neighbour Index: %d"), Index);

	}



	//FPolyNode* CurrentNode = PolyMap.Find(*StartRef);
	//FPolyNode* EndNode = PolyMap.Find(*EndRef);

	//TArray<dtPolyRef> NeighboursArr;
	//
	//TArray<FPolyNode*> OpenArr;
	//TArray<FPolyNode*> OpenSet;
	//TArray<FPolyNode*> ClosedSet;

	//OpenArr.Heapify(FCompareNodes());

	//OpenArr.HeapPush(CurrentNode, FCompareNodes());
	//OpenSet.Add(CurrentNode);

	//int32 Iterations = 0;

	//while (!OpenArr.IsEmpty())
	//{

	//	OpenArr.HeapPop(CurrentNode, FCompareNodes());

	//	if (CurrentNode->GetRef() == EndNode->GetRef() || CurrentNode->GetRef() == EndNode->GetRef())
	//	{
	//		UE_LOG(LogTemp, Log, TEXT("Found Correct Path. "));
	//		return ReconstructPath(CurrentNode);
	//	}

	//	ClosedSet.Add(CurrentNode);
	//	OpenSet.Remove(CurrentNode);

	//	dtPolyRef Ref = CurrentNode->GetRef(); // GetRef() returns an rvalue ( one that is not in the memory )

	//	GetNeighbours(NeighboursArr, &Ref);

	//	for (dtPolyRef& NeighbourRef : NeighboursArr)
	//	{

	//		Iterations++;

	//		FPolyNode *Neighbour = PolyMap.Find(NeighbourRef);

	//		if (!Neighbour)
	//		{
	//			UE_LOG(LogTemp, Error, TEXT("APathFinder::FindPath: A Neighbour in NeighboursArray was not found in PolyMap. "));
	//			continue;
	//		}
	//		
	//		if (ClosedSet.Contains(Neighbour))
	//		{
	//			continue;
	//		}

	//		bool IsInOpen = OpenSet.Contains(Neighbour);

	//		int32 NewG = CurrentNode->GetG() + FVector::Dist2D(CurrentNode->GetEntrance(), Neighbour->GetEntrance());
	//
	//		if (IsInOpen && NewG > Neighbour->GetG())
	//		{
	//			continue;
	//		}

	//		if (IsInOpen)
	//		{
	//			OpenArr.Remove(Neighbour);
	//			OpenArr.Heapify(FCompareNodes());
	//		}

	//		UE_LOG(LogTemp, Log, TEXT("Iteration: %d | Neighbour Ref: %llu"), Iterations, NeighbourRef);

	//		Neighbour->SetG(NewG);
	//		Neighbour->SetH(FinishPosition);

	//		static const float Weight = 1.1f;

	//		Neighbour->SetF(Weight);

	//		Neighbour->SetParentRef(CurrentNode->GetRef());

	//		OpenArr.HeapPush(Neighbour, FCompareNodes());
	//		OpenSet.Add(Neighbour);

	//	}

	//}

	return GetEmptyArray();

}

TArray<FPolyNode> APathFinder::ReconstructPath(FPolyNode* LastNode)
{

	FPolyNode* CurrNode = LastNode;

	TArray<FPolyNode> Array;

	while (CurrNode)
	{

		Array.Add(*CurrNode);

		if (!CurrNode->IsParentRefValid() && !PolyMap.Contains(CurrNode->GetParentRef()))
		{
			break;
		}
		
		CurrNode = &PolyMap[CurrNode->GetParentRef()];

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

	dtReal Center[3];
	dtReal ExtentReal[3];

	VectorToReal(Location, Center);
	VectorToReal(Extent, ExtentReal);

	dtStatus Status = Query.findNearestPoly(Center, ExtentReal, &QueryFilter, Poly, nullptr);

	UE_LOG(LogTemp, Log, TEXT("Poly: %llu. Status Success: %d, Status Fail: %d"), *Poly, dtStatusSucceed(Status), dtStatusFailed(Status));

	if (!dtStatusSucceed(Status) || *Poly == 0)
	{
		*Poly = 0;
	}

}

static int32 NeighbourCount = 0;

void APathFinder::GetNeighbours(TArray<dtPolyRef>& NeighboursArr, dtPolyRef& PolyRef)
{

	NeighboursArr.Empty();

	const dtPoly* Poly = nullptr;
	const dtMeshTile* Tile = nullptr;;

	const dtNavMesh * DetourMesh = GetDetourMesh();

	if (!DetourMesh)
	{
		return;
	}

	dtStatus Status = DetourMesh->getTileAndPolyByRef(PolyRef, &Tile, &Poly);

	if (!dtStatusSucceed(Status))
	{
		UE_LOG(LogTemp, Error, TEXT("Status Hasn't Succeeded. "));
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

		if (NeighbourIndex == 0)
		{
			continue;
		}

		bool IsOutsideTile = false;

		if (NeighbourIndex & DT_EXT_LINK) // is the neighbour index outside of current tile? more expensive calculations
		{
			if (NeighbourIndex - 1 >= Tile->header->maxLinkCount)
			{
				continue;
			}

			NeighbourRef = FPortalBuilder::GetRefOutsideTile(&PolyInfo, i);
			DetourMesh->getTileAndPolyByRef(NeighbourRef, &NeighbourTile, &Neighbour);
			IsOutsideTile = true;
		}
		else
		{
			// If true, poly is trash 
			if (NeighbourIndex - 1 >= Tile->header->polyCount)
			{
				continue;
			}


			Neighbour = &PolyInfo.Tile->polys[NeighbourIndex - 1];

			NeighbourRef = GetPolyRef(PolyInfo, EWhichHandle::OTHER);
		}

		if (NeighbourRef == 0) continue;

		PolyInfo.OtherHandle.Ref = NeighbourRef;
		PolyInfo.OtherHandle.Poly = Neighbour;

		FPortal Portal = FPortalBuilder::BuildPortal(&PolyInfo);
		FPolyNode NeighbourPolyNode = FPolyNode();

		NeighbourPolyNode.SetEntrance(Portal.GetPortalMiddle());
		NeighbourPolyNode.SetRef(NeighbourRef);

		NeighboursArr.Add(NeighbourRef);
		AddPolyToMap(NeighbourRef, NeighbourPolyNode);

		NeighbourCount++;

		// Logging 
		UE_LOG(LogTemp, Log, TEXT("Neighbour %d: | Is External: %d | Portal Midpoint: %s | Ref: %llu"),
			NeighbourCount, IsOutsideTile, *NeighbourPolyNode.GetEntrance().ToString(), NeighbourRef);
	
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