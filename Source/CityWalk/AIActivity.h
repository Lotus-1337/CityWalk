#pragma once

#include "CoreMinimal.h"

class AAIActor;

class FAIActivity;

extern TArray<TUniquePtr<FAIActivity>> Activities;

class FAIActivity
{

public:

	FORCEINLINE FAIActivity(const float& NewDuration)
	{
		Duration = NewDuration;
	}

	FORCEINLINE FAIActivity() {}

	float Duration = 0.0f;
	uint8 Index = 0;
	uint8 TypeID;

	virtual void OnActivityStarted(AAIActor& AI) {
		UE_LOG(LogTemp, Log, TEXT("FAIActivity::OnActivityStarted "));
	}

	virtual void OnActivityEnded(AAIActor& AI) {}

	virtual void ExecuteActivity(AAIActor& AI) {}


};

class FWanderingActivity : public FAIActivity
{

#define UE_WANDERING_ID  1

public:


	FWanderingActivity(float NewDuration) : FAIActivity(NewDuration) { TypeID = 1; }

	FWanderingActivity() : FAIActivity() { TypeID = 1; }

	virtual void OnActivityStarted(AAIActor& AI) override;

	virtual void OnActivityEnded(AAIActor& AI) override;

	FORCEINLINE static bool IsActivityThis(const FAIActivity& Act) { return Act.TypeID == UE_WANDERING_ID; }


};