// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIVisualSubsystem.h"

#include "AIMeshDataAsset.h"
#include "AIAnimationDataAsset.h"

UAIVisualSubsystem::UAIVisualSubsystem()
{

	//FString Path = FPaths::ProjectContentDir() / TEXT("DataAssets/");
	FString Path = TEXT("/Game/DataAssets/");

	FString MeshPath = Path / TEXT("DA_MeshDataAsset.DA_MeshDataAsset");

	static ConstructorHelpers::FObjectFinder<UAIMeshDataAsset> MeshAsset(*MeshPath);

	// Try this path:
	// /Game/DataAssets/DA_MeshDataAsset.DA_MeshDataAsset

	if (!MeshAsset.Succeeded())
	{
		return;
	}
	
	MeshDataAsset = MeshAsset.Object;
	

	FString AnimationsPath = Path / TEXT("DA_AnimationDataAsset.DA_AnimationDataAsset");

	static ConstructorHelpers::FObjectFinder<UAIAnimationDataAsset> AnimAsset(*AnimationsPath);

	if (AnimAsset.Succeeded())
	{
		return;
	}

	AnimationDataAsset = AnimAsset.Object;
	
}

USkeletalMesh* UAIVisualSubsystem::GetRandomMesh()
{

	if (!MeshDataAsset) return nullptr;
	return MeshDataAsset->GetRandomMesh();

}

USkeletalMesh* UAIVisualSubsystem::GetMeshByIndex(const int32& Index)
{
	if (!MeshDataAsset) return nullptr;
	return MeshDataAsset->GetMeshByIndex(Index);
}

UAnimationAsset* UAIVisualSubsystem::GetRandomAnimation()
{
	if (!AnimationDataAsset) return nullptr;
	return AnimationDataAsset->GetRandomAnimation();
}

UAnimationAsset* UAIVisualSubsystem::GetAnimationByIndex(const int32& Index)
{
	if (!AnimationDataAsset) return nullptr;
	return AnimationDataAsset->GetAnimationByIndex(Index);
}