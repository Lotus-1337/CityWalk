// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AIMeshDataAsset.generated.h"

class USkeletalMesh;

/**
 * 
 */
UCLASS(BlueprintType)
class CITYWALK_API UAIMeshDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
	TArray<USkeletalMesh*> MeshesArray;

public:

	FORCEINLINE USkeletalMesh* GetRandomMesh()
	{
		int32 Index = FMath::RandRange(0, MeshesArray.Num());
		return GetMeshByIndex(Index);
	}

	FORCEINLINE USkeletalMesh* GetMeshByIndex(const int32& Index)
	{
		return MeshesArray.IsValidIndex(Index) ? MeshesArray[Index] : nullptr;
	}

};
