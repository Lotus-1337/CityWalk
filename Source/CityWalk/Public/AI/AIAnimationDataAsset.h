// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AIAnimationDataAsset.generated.h"

class UAnimationAsset;

/**
 * 
 */
UCLASS(BlueprintType)
class CITYWALK_API UAIAnimationDataAsset : public UDataAsset
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
	TArray<UAnimationAsset*> AnimationsArray;

public:

	FORCEINLINE UAnimationAsset* GetRandomAnimation()
	{
		int32 Index = FMath::RandRange(0, AnimationsArray.Num() - 1);
		return GetAnimationByIndex(Index);
	}

	UAnimationAsset* GetAnimationByIndex(const int32& Index)
	{
		return AnimationsArray.IsValidIndex(Index) ? AnimationsArray[Index] : nullptr;
	}
	
};
