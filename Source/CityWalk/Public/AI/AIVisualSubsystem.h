// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AIVisualSubsystem.generated.h"

class UAIAnimationDataAsset;
class UAIMeshDataAsset;

/**
 * 
 */
UCLASS(Blueprintable)
class CITYWALK_API UAIVisualSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
	TObjectPtr<UAIMeshDataAsset> MeshDataAsset;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAIAnimationDataAsset> AnimationDataAsset;


public:

	UAIVisualSubsystem();

	USkeletalMesh* GetRandomMesh();

	USkeletalMesh* GetMeshByIndex(const int32& Index);

	UAnimationAsset* GetRandomAnimation();

	UAnimationAsset* GetAnimationByIndex(const int32& Index);

};
