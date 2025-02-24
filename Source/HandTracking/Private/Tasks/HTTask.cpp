// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/HTTask.h"

#include "System/HTResultWriterSubsystem.h"
#include "Tasks/HTTaskObjective.h"

void UHTTask::StartTask(UObject* WorldContext, FString Configuration)
{
	WorldContextManual = WorldContext;

	UHTResultWriterSubsystem* ResultWriterSubsystem = GetResultWriterSubsystem();
	ResultWriterSubsystem->BeginWritingTest(Configuration);
}

void UHTTask::BeginNextObjective(UObject* WorldContext)
{
	if (CurrentObjective == Objectives.Num())
	{
		return;
	}

	Objectives[CurrentObjective]->Activate(WorldContext);
	Objectives[CurrentObjective]->OnObjectiveCompleted.AddDynamic(this, &ThisClass::OnObjectiveCompleted);
	Objectives[CurrentObjective]->OnObjectiveReadyToBeCompleted.AddDynamic(this, &ThisClass::OnObjectiveReadyToBeCompletedFunc);
	OnObjectiveStarted.Broadcast(Objectives[CurrentObjective], CurrentObjective);
}

void UHTTask::CompleteOrBeginNextObjective(UObject* WorldContext)
{
	if (CurrentObjective == Objectives.Num())
	{
		return;
	}

	if (!Objectives[CurrentObjective]->IsInProgress())
	{
		BeginNextObjective(WorldContext);
	}
	else
	{
		CompleteCurrentObjective();
	}
}

void UHTTask::CompleteCurrentObjective()
{
	if (CurrentObjective == Objectives.Num())
	{
		return;
	}

	if (!Objectives[CurrentObjective]->IsInProgress())
	{
		return;
	}

	Objectives[CurrentObjective]->Complete();
}

void UHTTask::OnObjectiveReadyToBeCompletedFunc(bool bReady)
{
	OnObjectiveReadyToBeCompleted.Broadcast(bReady);
}

UHTResultWriterSubsystem* UHTTask::GetResultWriterSubsystem() const
{
	if (WorldContextManual)
	{
		return WorldContextManual->GetWorld()->GetGameInstance()->GetSubsystem<UHTResultWriterSubsystem>();
	}

	return nullptr;
}

void UHTTask::OnObjectiveCompleted(UHTTaskObjective* Objective)
{
	/** Clear delegate */
	Objective->OnObjectiveCompleted.RemoveDynamic(this, &ThisClass::OnObjectiveCompleted);
	Objective->OnObjectiveReadyToBeCompleted.RemoveDynamic(this, &ThisClass::OnObjectiveReadyToBeCompletedFunc);

	/** Broadcast our own, and increment objective counter */
	OnTaskObjectiveCompleted.Broadcast(Objective, CurrentObjective);
	++CurrentObjective;
	
	/** Check if all objectives are completed */
	/** @NOTE (Denis): Could be optimized by keeping a running total of completed objectives, but is probably not required
	 * will have to keep it in mind though */
	const int32 NumObjectives = Objectives.Num();
	int32 CompletedObjectives = 0;
	for (const auto Obj : Objectives)
	{
		if (Obj->IsComplete())
		{
			++CompletedObjectives;
		}
	}

	if (NumObjectives == CompletedObjectives)
	{
		GetResultWriterSubsystem()->EndWritingTest();
		OnCompleted.Broadcast(this);
	}
}
