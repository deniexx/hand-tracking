// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/HTTask.h"

#include "Tasks/HTTaskObjective.h"

void UHTTask::BeginTask(UObject* WorldContext)
{
	for (const auto Objective : Objectives)
	{
		Objective->OnObjectiveCompleted.AddDynamic(this, &ThisClass::OnObjectiveCompleted);
		Objective->Activate(WorldContext);
	}
}

void UHTTask::OnObjectiveCompleted(UHTTaskObjective* Objective)
{
	/** Clear delegate */
	Objective->OnObjectiveCompleted.RemoveDynamic(this, &ThisClass::OnObjectiveCompleted);
	
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
