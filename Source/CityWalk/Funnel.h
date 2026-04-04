// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FPortal;

/**
 * 
 */
class CITYWALK_API FFunnel
{
public:
	
	FFunnel();

	bool BuildFunnelPath(TArray<FVector>& OutArray, TArray<FPortal>& InArray) const;

	bool AddLeft(TArray<FVector>& OutArray, const FVector& V) const;

	bool AddRight(TArray<FVector>& OutArray, const FVector& V) const;

};
