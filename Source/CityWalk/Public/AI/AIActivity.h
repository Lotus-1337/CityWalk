#pragma once

#include "CoreMinimal.h"

class AAIActor;

class FAIActivity;

class FAIActivity
{

public:

	FORCEINLINE FAIActivity(const float& NewDuration)
	{
		Duration = NewDuration;
	}

	FORCEINLINE FAIActivity() {}

	// If this this field is left at 0.0f, activity is invinite
	float Duration = 0.0f;
	
	uint8 Index = 0;
	uint8 TypeID;

	virtual void OnActivityStarted(AAIActor& AI) {
		UE_LOG(LogTemp, Log, TEXT("FAIActivity::OnActivityStarted "));
	}

	virtual void OnActivityEnded(AAIActor& AI) {}

	virtual void ExecuteActivity(AAIActor& AI) {}


};

class FIdleActivity : public FAIActivity
{

#define UE_IDLE_ID 1

public:

	FIdleActivity(float NewDuration) : FAIActivity(NewDuration) { TypeID = UE_IDLE_ID; }

	FIdleActivity() : FAIActivity() { TypeID = UE_IDLE_ID; }

	virtual void OnActivityStarted(AAIActor& AI) override;
	
	virtual void OnActivityEnded(AAIActor& AI) override;

	FORCEINLINE static bool IsActivityThis(const FAIActivity& Act) { return Act.TypeID == UE_IDLE_ID; }

};

class FWanderingActivity : public FAIActivity
{

#define UE_WANDERING_ID  2

public:


	FWanderingActivity(float NewDuration) : FAIActivity(NewDuration) { TypeID = UE_WANDERING_ID; }

	FWanderingActivity() : FAIActivity() { TypeID = UE_WANDERING_ID; }

	virtual void OnActivityStarted(AAIActor& AI) override;

	virtual void OnActivityEnded(AAIActor& AI) override;

	FORCEINLINE static bool IsActivityThis(const FAIActivity& Act) { return Act.TypeID == UE_WANDERING_ID; }


};

class FTalkingActivity : public FAIActivity
{

#define UE_TALKING_ID 3

public:

	FTalkingActivity(float NewDuration) : FAIActivity(NewDuration) { TypeID = UE_TALKING_ID; }

	FTalkingActivity() : FAIActivity() { TypeID = UE_TALKING_ID; }

	virtual void OnActivityStarted(AAIActor& AI) override;

	virtual void OnActivityEnded(AAIActor& AI) override;

	FORCEINLINE static bool IsActivityThis(const FAIActivity& Act) { return Act.TypeID == UE_TALKING_ID; }

};

class FWalkingActivity : public FAIActivity
{

#define UE_WALKING_ID 4

public:

	FWalkingActivity(float NewDuration) : FAIActivity(NewDuration) { TypeID = UE_WALKING_ID; }

	FWalkingActivity() : FAIActivity() { TypeID = UE_WALKING_ID; }

	virtual void OnActivityStarted(AAIActor& AI) override;

	virtual void OnActivityEnded(AAIActor& AI) override;

	FORCEINLINE static bool IsActivityThis(const FAIActivity& Act) { return Act.TypeID == UE_WALKING_ID; }

};