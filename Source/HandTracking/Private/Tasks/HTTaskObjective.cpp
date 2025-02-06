// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/HTTaskObjective.h"

#include "EngineUtils.h"
#include "Actor/HTTaskActor.h"


void UHTTaskObjective::Activate_Implementation(UObject* InWorldContextManual)
{
	if (!IsWaitingToStart())
	{
		return;
	}
	
	TrackedData = FString();
	State = EObjectiveState::InProgress;
	WorldContextManual = InWorldContextManual;
	OnObjectiveStarted.Broadcast(this);
}

EObjectiveState UHTTaskObjective::GetObjectiveState() const
{
	return State;
}

const FString& UHTTaskObjective::GetTrackedData() const
{
	return TrackedData;
}

void UHTTaskObjective::Complete()
{
	if (!IsInProgress())
	{
		return;
	}

	State = EObjectiveState::Completed;
	OnObjectiveCompleted.Broadcast(this);
}

bool UHTTaskObjective::IsComplete() const
{
	return State == EObjectiveState::Completed;
}

bool UHTTaskObjective::IsInProgress() const
{
	return State == EObjectiveState::InProgress;
}

bool UHTTaskObjective::IsWaitingToStart() const
{
	return State == EObjectiveState::WaitingToStart;
}

TArray<AHTTaskActor*> UHTTaskObjective::GatherTaskActors() const
{
	TArray<AHTTaskActor*> RelativeTaskActors;
	UWorld* World = WorldContextManual->GetWorld();
	
	/** Actually not that slow, as it uses HashBuckets for different actor classes */
	for (TActorIterator<AHTTaskActor> It(World, AHTTaskActor::StaticClass()); It; ++It)
	{
		AHTTaskActor* Actor = *It;
		if (IsValid(Actor) && TaskActorTags.HasTagExact(Actor->TaskActorTag))
		{
			RelativeTaskActors.Add(Actor);
		}
	}

	return RelativeTaskActors;
}
