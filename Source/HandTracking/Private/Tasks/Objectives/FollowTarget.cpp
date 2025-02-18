// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/Objectives/FollowTarget.h"

#include "SplineComponent.h"
#include "Actor/HTTaskActor.h"
#include "Async/CAsyncAction_FollowTargetTask.h"

void UFollowTarget::Activate_Implementation(UObject* InWorldContextManual)
{
	Super::Activate_Implementation(InWorldContextManual);

	TArray<AHTTaskActor*> Actors = GatherTaskActors();

	AHTTaskActor* SolutionObject = nullptr;
	AHTTaskActor* TargetObject = nullptr;
	USplineComponent* SplineComponent = nullptr;
	
	for (auto Actor : Actors)
	{
		if (SolutionObjectTag.MatchesTagExact(Actor->TaskActorTag))
		{
			SolutionObject = Actor;
			SolutionObject->OnGrabbed.AddDynamic(this, &ThisClass::BeginTracking);
			SolutionObject->OnDropped.AddDynamic(this, &ThisClass::StopTracking);
		}
		else if (TargetTag.MatchesTagExact(Actor->TaskActorTag))
		{
			TargetObject = Actor;
		}
		else if (SplineActorTag.MatchesTagExact(Actor->TaskActorTag))
		{
			SplineComponent = Actor->GetComponentByClass<USplineComponent>();
		}
	}

	TargetObject->SetActorLocation(SplineComponent->GetWorldLocationAtDistanceAlongSpline(0.f));
	
	FollowTargetTask = UCAsyncAction_FollowTargetTask::Execute(TargetObject, SolutionObject, SplineComponent,
		MovementSpeed, AcceptableDistanceDelta);
}

void UFollowTarget::Complete_Implementation()
{
	if (!FollowTargetTask->IsObjectCloseToEnd())
	{
		// Force user to get to close to the end before completing objective
		return;
	}

	TrackedData.Empty();

	float AveragePositionDelta, TotalPositionDelta, TotalPositionSamples, Duration;
	FollowTargetTask->GetTrackedData(AveragePositionDelta, TotalPositionDelta, TotalPositionSamples, Duration);
	TrackedData += FString("Average Position Delta:, ") + FString::Printf(TEXT("%.2f"), AveragePositionDelta) + LINE_TERMINATOR;
	TrackedData += FString("Time Taken:, ") + FString::Printf(TEXT("%.2f"), Duration) + LINE_TERMINATOR;
	Super::Complete_Implementation();
}

void UFollowTarget::BeginTracking(AHTTaskActor* TaskActor)
{
	FollowTargetTask->SetIsTracking(true);
}

void UFollowTarget::StopTracking(AHTTaskActor* TaskActor)
{
	FollowTargetTask->SetIsTracking(false);
}
