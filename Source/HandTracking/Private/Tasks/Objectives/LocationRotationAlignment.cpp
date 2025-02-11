// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/Objectives/LocationRotationAlignment.h"

#include "Actor/HTTaskActor.h"
#include "HandTracking/HandTracking.h"

ULocationRotationAlignment::ULocationRotationAlignment()
{
	bAutoComplete = false;
}

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
				UPrimitiveComponent* Component = Cast<UPrimitiveComponent>(SolutionActor->GetRootComponent());
				Component->CreateAndSetMaterialInstanceDynamic(0);
				break;
			}
		}
	}
}

void ULocationRotationAlignment::Complete_Implementation()
{
	TrackedData.Empty();
	for (const auto& SolutionToTarget : SolutionsToTargets)
	{
		const FVector ToTarget = SolutionToTarget.Key->GetActorLocation() - SolutionToTarget.Value->GetActorLocation();
		const float Distance = ToTarget.Length();
		if (Distance > MinimumAcceptableDistance)
		{
			// Do not allow user to complete objective, unless they are within acceptable range
			return;
		}
		// Track data
	}
	
	/** Last as we want to broadcast after we have finished here */
	Super::Complete_Implementation();
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
				// Yellow-y orange
				SetMaterialColor(TaskActor, FVector(0.244792f, 0.223655f, 0.000000f));
				return;
			}

			// Green
			SetMaterialColor(TaskActor, FVector(0.000000f, 0.244792f, 0.000000f));
			UE_LOG(LogHandTracking, Log, TEXT("SolutionActorDropped kinda close to the TargetActor"));
		}
	}
}
