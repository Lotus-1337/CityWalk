// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "PathFinder.generated.h"

class dtNavMesh;
class dtNavMeshQuery;

class APathFinder;

typedef uint64_t UEType_uint64;
typedef UEType_uint64 dtPolyRef;

USTRUCT()
struct FPolyNode
{

	GENERATED_BODY()

public:

	FPolyNode()
	{
		Entrance = FVector::ZeroVector;
		Ref = 0;
		ParentRef = 0;
	}

protected:

	dtPolyRef Ref = 0;

	// Entrance portal midpoint
	FVector Entrance = FVector::ZeroVector;

	int32 H = 10;
	int32 G = 1e10;
	int32 F = 0;

	dtPolyRef ParentRef = 0;

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

	/**
	* Claculates F Cost ( Total Cost: G + H )
	* @param Weight: Heuretic Multiplier in F = G + H Equasion, higher values might speed up the algorithm, generating worse paths
	**/
	FORCEINLINE void SetF(const float &Weight = 1.0f) { F = G + (H * Weight ); };

	FORCEINLINE void SetG(const int32& NewG) { G = NewG; };

	FORCEINLINE void SetH(const FVector& FinishLocation) { H = FVector::Dist2D(Entrance, FinishLocation); };

	// Index Stuff

	FORCEINLINE bool IsParentRefValid() const { return ParentRef != 0; };

	FORCEINLINE dtPolyRef GetParentRef() const { return ParentRef; };

	FORCEINLINE void SetParentRef(const dtPolyRef& NewRef) { ParentRef = NewRef; };

	// Operators 

	FORCEINLINE bool operator==(const FPolyNode* OtherNode) const { return Ref == OtherNode->GetRef(); };

	FORCEINLINE bool operator==(const FPolyNode& OtherNode) const { return Ref == OtherNode.GetRef(); };

	FORCEINLINE bool operator!=(const FPolyNode* OtherNode) const { return Ref != OtherNode->GetRef(); };
	
	FORCEINLINE bool operator!=(const FPolyNode& OtherNode) const { return Ref != OtherNode.GetRef(); };

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
	TMap<dtPolyRef, FPolyNode> PolyMap; // Can be a set
	
public:	
	// Sets default values for this actor's properties
	APathFinder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void AddPolyToMap(dtPolyRef& Ref, FPolyNode& Node);

public:	

	int32 NeighbourCount = 0;

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
	bool FindPath(TArray<dtPolyRef> & OutArray, const FVector& StartingPosition, const FVector& FinishPosition);
	
	/**
	* Goes Through Last Node's Parents to Reconstruct the Path
	* 
	* @param OutArray: Outputs Reconstructed Path to the Array
	* @param LastNode: The Last Node found in PathFinding Algorithm
	* 
	* @returns The success of the Operation
	*/
	bool ReconstructPath(TArray<dtPolyRef> & OutArray, const FPolyNode* LastNode);

	/**
	* Neccesary for APathFinder::ReconstructPath()
	* It's Important because in the said function Path Starts From The Last Node, which is not wanted
	*/
	void ReverseArray(TArray<dtPolyRef>& Arr);

	void SwapNodes(dtPolyRef& Node1, dtPolyRef& Node2);

	/**
	* Simple One-Liner Function to Get The Closest Poly
	* 
	* @param Poly: Pointer to a poly which the closest Poly is assigned to, might  become nullptr
	* 
	**/
	void GetClosestPoly(dtPolyRef * Poly, const FVector& Location, const FVector& Extent);

	void GetNeighbours(TArray<dtPolyRef> &NeighboursArr, const dtPolyRef& Poly);

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