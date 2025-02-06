// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/Objectives/LocationRotationAlignment.h"

#include "Actor/HTTaskActor.h"
#include "HandTracking/HandTracking.h"

void ULocationRotationAlignment::Activate_Implementation(UObject* InWorldContextManual)
{
	Super::Activate_Implementation(InWorldContextManual);
	
	TArray<AHTTaskActor*> TaskActors = GatherTaskActors();
	TArray<AHTTaskActor*> SolutionActors;
	TArray<AHTTaskActor*> TargetActors;
	for (AHTTaskActor* TaskActor : TaskActors)
	{
		if (TaskActor->TaskActorTag.MatchesAnyExact(SolutionObjects))
		{
			SolutionActors.Add(TaskActor);
		}
		else if (TaskActor->TaskActorTag.MatchesAnyExact(Targets))
		{
			TaskActor->SetCanBeGrabbed(false);
			TargetActors.Add(TaskActor);
		}
	}

	for (auto SolutionActor : SolutionActors)
	{
		for (auto TargetActor : TargetActors)
		{
			if (SolutionActor->OptionalTag.MatchesTagExact(TargetActor->TaskActorTag))
			{
				SolutionActor->OnDropped.AddDynamic(this, &ThisClass::OnSolutionActorDropped);
				SolutionsToTargets.Add( { SolutionActor, TargetActor } );
				break;
			}
		}
	}
}

void ULocationRotationAlignment::OnSolutionActorDropped(AHTTaskActor* TaskActor)
{
	for (const auto SolutionToTarget : SolutionsToTargets)
	{
		if (TaskActor == SolutionToTarget.Key)
		{
			const FVector ToTarget = SolutionToTarget.Key->GetActorLocation() - SolutionToTarget.Value->GetActorLocation();
			const float Distance = ToTarget.Length();

			if (Distance > MinimumAcceptableDistance)
			{
				return;
			}

			/** Check the distance and process the dropping of the object, also deny grabbing */
			UE_LOG(LogHandTracking, Log, TEXT("SolutionActorDropped kinda close to the TargetActor"));
		}
	}
}
