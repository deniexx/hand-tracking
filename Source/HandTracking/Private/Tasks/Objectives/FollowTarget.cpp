// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/Objectives/FollowTarget.h"

#include "SplineComponent.h"
#include "Actor/HTTaskActor.h"
#include "Async/CAsyncAction_FollowTargetTask.h"

void UFollowTarget::Activate_Implementation(UObject* InWorldContextManual)
{
	Super::Activate_Implementation(InWorldContextManual);

	TArray<AHTTaskActor*> Actors = GatherTaskActors();

	SolutionObject = nullptr;
	SplineComponent = nullptr;
	
	for (auto Actor : Actors)
	{
		if (SolutionObjectTag.MatchesTagExact(Actor->TaskActorTag))
		{
			SolutionObject = Actor;
			SolutionObject->OnGrabbed.AddDynamic(this, &ThisClass::OnSolutionObjectGrabbbed);
			SolutionObject->OnDropped.AddDynamic(this, &ThisClass::OnSolutionObjectDropped);

			if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(SolutionObject->GetRootComponent()))
			{
				PrimitiveComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnSolutionObjectHit);
			}
		}
		else if (SplineActorTag.MatchesTagExact(Actor->TaskActorTag))
		{
			SplineActor = Actor;
			SplineComponent = Actor->GetComponentByClass<USplineComponent>();
		}
	}

	FollowTargetTask = UCAsyncAction_FollowTargetTask::Execute(SolutionObject, SplineComponent,
		AcceptableDistanceDelta, GoodColor, BadColor);
	
	FollowTargetTask->OnLocationCloseToEnd.AddDynamic(this, &ThisClass::OnLocationCloseToEnd);
}

void UFollowTarget::Complete_Implementation()
{
	const FVector EndSplineLocation = SplineComponent->GetLocationAtSplinePoint(SplineComponent->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);
	const float Distance = FVector::Dist2D(EndSplineLocation, SolutionObject->GetActorLocation());

	if (Distance > AcceptableDistanceDelta)
	{
		return;
	}
	
	TrackedData.Empty();

	const float TimeTaken = WorldContextManual->GetWorld()->GetTimeSeconds() - TimeStarted;
	float AveragePositionDelta, TotalPositionSamples, TotalPositionDelta, MaxDistanceDelta;
	FollowTargetTask->GetTrackedData(AveragePositionDelta, TotalPositionDelta, TotalPositionSamples, MaxDistanceDelta);
	
	TrackedData += FString::Printf(TEXT("Time taken to complete: %.2f"), TimeTaken) + LINE_TERMINATOR;
	TrackedData += FString::Printf(TEXT("Number of Collisions: %d"), NumberOfHits)+ LINE_TERMINATOR;
	TrackedData += FString::Printf(TEXT("AveragePositionDelta, %.2f"), AveragePositionDelta) + LINE_TERMINATOR;
	TrackedData += FString::Printf(TEXT("MaxDistanceDelta, %.2f"), MaxDistanceDelta) + LINE_TERMINATOR;
	TrackedData += FString::Printf(TEXT("TotalPositionSamples, %.0f"), TotalPositionSamples) + LINE_TERMINATOR;
	TrackedData += FString::Printf(TEXT("TotalPositionDelta, %.2f"), TotalPositionDelta) + LINE_TERMINATOR;
	
	Super::Complete_Implementation();
}

void UFollowTarget::OnSolutionObjectHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == SplineActor)
	{
		++NumberOfHits;
	}
}

void UFollowTarget::OnSolutionObjectGrabbbed(AHTTaskActor* TaskActor)
{
	FollowTargetTask->SetIsTracking(true);
}

void UFollowTarget::OnSolutionObjectDropped(AHTTaskActor* TaskActor)
{
	FollowTargetTask->SetIsTracking(false);
}

void UFollowTarget::OnLocationCloseToEnd()
{
	OnObjectiveReadyToBeCompleted.Broadcast(true);
}
