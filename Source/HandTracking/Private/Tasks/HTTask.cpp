// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/HTTask.h"

#include "Tasks/HTTaskObjective.h"

void UHTTask::BeginNextObjective(UObject* WorldContext)
{
	if (CurrentObjective == Objectives.Num())
	{
		return;
	}

	Objectives[CurrentObjective]->Activate(WorldContext);
	Objectives[CurrentObjective]->OnObjectiveCompleted.AddDynamic(this, &ThisClass::OnObjectiveCompleted);
}

void UHTTask::OnObjectiveCompleted(UHTTaskObjective* Objective)
{
	/** Clear delegate */
	Objective->OnObjectiveCompleted.RemoveDynamic(this, &ThisClass::OnObjectiveCompleted);

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
		OnCompleted.Broadcast(this);
	}
}
