// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Detour/DetourNavMesh.h"

#include "PathFinder.generated.h"

class dtNavMesh;
class dtNavMeshQuery;

class APathFinder;

USTRUCT()
struct FPolyNode
{

	GENERATED_BODY()

public:

	FPolyNode()
	{
		Entrance = FVector::ZeroVector;
		Ref = 0;
	}

protected:

	dtPolyRef Ref;

	// Entrance portal midpoint
	FVector Entrance;

	int32 H = 10;
	int32 G = 1e10;
	int32 F = 0;

	int32 ParentIndex = -1;
	int32 Index = -1;

public:

	FORCEINLINE void SetRef(dtPolyRef& NewRef) { Ref = NewRef; };

	/** I KNOW IT LOOKS WEIRD THAT YOU HAVE TO GIVE PATHFINDER AS AN ARGUMENT
	But I Swear is makes sense... */
	FVector CalculateCenter(const APathFinder* PathFinder); // { Center = PathFinder->GetPolygonCentroid(&Ref); }

	FORCEINLINE FVector GetEntrance() const { return Entrance; };

	FORCEINLINE void SetEntrance(const FVector& NewEntrance) { Entrance = NewEntrance; };

	FORCEINLINE dtPolyRef GetRef() const { return Ref; };

	FORCEINLINE int32 GetH() const { return H; };

	FORCEINLINE int32 GetF() const { return F; };

	FORCEINLINE int32 GetG() const { return G; };

	FORCEINLINE void SetF() { F = G + H; };

	FORCEINLINE void SetG(const int32& NewG) { G = NewG; };

	FORCEINLINE void SetH(const FVector& FinishLocation) { H = FVector::Dist2D(Entrance, FinishLocation); };

	// Index Stuff

	FORCEINLINE bool IsIndexValid() { return Index != 1; };

	FORCEINLINE bool IsParentIdexValid() { return ParentIndex != -1; };

	FORCEINLINE int32 GetIndex() { return Index; };

	FORCEINLINE int32 GetParentIndex() { return ParentIndex; };

	FORCEINLINE void SetIndex(const int32& NewIndex) { Index = NewIndex; };

	FORCEINLINE void SetParentIndex(const int32& NewIndex) { ParentIndex = NewIndex; };

};

struct FCompareNodes
{

	FORCEINLINE bool operator()(const FPolyNode& A, const FPolyNode& B) const
	{
		return A.GetF() < B.GetF();
	}

	FORCEINLINE bool operator()(FPolyNode& A, FPolyNode& B) const
	{
		return A.GetF() < B.GetF();
	}

};

UCLASS()
class CITYWALK_API APathFinder : public AActor
{
	GENERATED_BODY()
	

protected:

	/**
	* Hash Map of All the Poly Nodes, where dtPolyRefs Are Hashed
	* This Allows Finding PolyNodes by their Refs 
	*/
	TMap<dtPolyRef, FPolyNode> PolyMap;
	
public:	
	// Sets default values for this actor's properties
	APathFinder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void AddPolyToMap(dtPolyRef& Ref, FPolyNode& Node);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	* Initializes Values in an existing Node 
	* 
	* @param Node: Initialized Node
	* 
	*/
	void InitNode(dtPolyRef& Ref, FPolyNode& Node);


	FPolyNode BuildNode(dtPolyRef& Ref);

	/**
	* Finds A Corridor of Polys From Starting Position to The Finish Position
	* Uses A* PathFinding Algorithm
	* 
	* @returns An Array of Polys
	*/
	TArray<FPolyNode> FindPath(const FVector& StartingPosition, const FVector& FinishPosition);
	
	/**
	* Goes Through Last Node's Parents to Reconstruct the Path
	* 
	* @param LastNode: The Last Node found in PathFinding Algorithm
	* 
	* @returns An Array of Parents From Last Node to Starting Node
	*/
	TArray<FPolyNode> ReconstructPath(FPolyNode* LastNode);

	/**
	* Neccesary for APathFinder::ReconstructPath()
	* It's Important because in the said function Path Starts From The Last Node, which is not wanted
	*/
	void ReverseArray(TArray<FPolyNode>& Arr);

	void SwapNodes(FPolyNode* Node1, FPolyNode* Node2);

	/**
	* Simple One-Liner Function to Get The Closest Poly
	* 
	* @param Poly: Pointer to a poly which the closest Poly is assigned to, might  become nullptr
	* 
	**/
	void GetClosestPoly(dtPolyRef * Poly, const FVector& Location, const FVector& Extent);

	

	void GetNeighbours(TArray<dtPolyRef> &NeighboursArr, dtPolyRef* Poly);

	FORCEINLINE TArray<FPolyNode> GetEmptyArray() const
	{
		return TArray<FPolyNode>();
	}
	
	/** One Liner Function to Get dtNavMeshQuery quickly, might be nullptr **/
	dtNavMeshQuery GetNavMeshQuery() const;
	
	/** One Liner Function to Get dtNavMesh quickly, might be nullptr **/
	const dtNavMesh* GetDetourMesh() const;

	FVector GetPolygonCentroid(dtPolyRef* Ref) const;

};