// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SparseSet.h"

#include "PathFinder.generated.h"

class dtNavMesh;
class dtNavMeshQuery;

class APathFinder;

typedef uint64_t UEType_uint64;
typedef UEType_uint64 dtPolyRef;

typedef int32 Index_t;

DECLARE_LOG_CATEGORY_EXTERN(LogProfiler, Log, Log);

USTRUCT()
struct FPolyNode
{

	GENERATED_BODY()

public:

	FPolyNode()
	{

	}

	dtPolyRef Ref = 0;

	// Entrance portal midpoint
	FVector Entrance = FVector::ZeroVector;

	float G = 1e10;
	float F = 1e10;

	Index_t ParentIndex = -1;
	Index_t Index = -1;

	bool IsInOpen = false;
	bool IsInClosed = false;

	/** I KNOW IT LOOKS WEIRD THAT YOU HAVE TO GIVE PATHFINDER AS AN ARGUMENT
	But I Swear is makes sense... */
	FVector CalculateCenter(const APathFinder* PathFinder); // { Center = PathFinder->GetPolygonCentroid(&Ref); }

	FORCEINLINE float GetF() const { return F; };

	/**
	* Claculates F Cost ( Total Cost: G + H )
	* @param Weight: Heuretic Multiplier in F = G + H Equasion, higher values might speed up the algorithm, generating worse paths
	**/
	FORCEINLINE void SetF(const float& H, const float &Weight = 1.0f) { F = G + (H * Weight ); };

	FORCEINLINE float CalculateH(const FVector& FinishLocation) { return FVector::DistSquared2D(Entrance, FinishLocation); };

	FORCEINLINE void Reset() { G = 1e10; F = 1e10; ParentIndex = -1; IsInOpen = false; IsInClosed = false; };

	// Index Stuff

	FORCEINLINE bool IsParentIdxValid() const { return ParentIndex != -1; };


	// Operators 

	FORCEINLINE bool operator==(const FPolyNode* OtherNode) const { return Ref == OtherNode->Ref; };

	FORCEINLINE bool operator==(const FPolyNode& OtherNode) const { return Ref == OtherNode.Ref; };

	FORCEINLINE bool operator!=(const FPolyNode* OtherNode) const { return Ref != OtherNode->Ref; };
	
	FORCEINLINE bool operator!=(const FPolyNode& OtherNode) const { return Ref != OtherNode.Ref; };

	dtPolyRef GetKey() const { return Ref; };

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

	TSparseSet<dtPolyRef, Index_t, FPolyNode> PolySet;

	TArray<FPolyNode*> NodesToClean;

public:

	int32 NeighbourCount = 0;
	
public:	
	// Sets default values for this actor's properties
	APathFinder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	* Initializes Values in an existing Node 
	* 
	* @param Node: Initialized Node
	* 
	*/
	void InitNode(dtPolyRef& Ref, FPolyNode& Node);


	FPolyNode BuildNode(dtPolyRef& Ref);

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

	void GetNeighbours(TArray<Index_t> &NeighboursArr, const dtPolyRef& Poly);

public:

	/**
	* Finds A Corridor of Polys From Starting Position to The Finish Position
	* Uses A* PathFinding Algorithm
	*
	* @returns An Array of Polys
	*/
	bool FindPath(TArray<dtPolyRef>& OutArray, const FVector& StartingPosition, const FVector& FinishPosition);

	/**
	* Cleans An Array of Used Nodes.
	* Calling this method during Pathfinding on another Thread will break the said PathFinding
	*/
	void CleanNodes();

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


