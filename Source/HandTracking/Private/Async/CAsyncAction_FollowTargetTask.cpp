// Fill out your copyright notice in the Description page of Project Settings.


#include "Async/CAsyncAction_FollowTargetTask.h"

#include "SplineComponent.h"

UCAsyncAction_FollowTargetTask* UCAsyncAction_FollowTargetTask::Execute(AActor* TargetActor, AActor* SolutionActor,
	USplineComponent* SplineComponent, float MovementSpeed, float AcceptableDistanceDelta)
{
	UCAsyncAction_FollowTargetTask* Action = NewObject<UCAsyncAction_FollowTargetTask>(TargetActor);
	Action->RegisterWithGameInstance(TargetActor);
	
	Action->TargetActor = TargetActor;
	Action->SolutionActor = SolutionActor;
	Action->SplineComponent = SplineComponent;
	Action->Speed = MovementSpeed;
	Action->AcceptableDistanceDelta = AcceptableDistanceDelta;
	Action->Activate();
	
	return Action;
}

void UCAsyncAction_FollowTargetTask::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LastFrameNumberWeTicked == GFrameCounter)
	{
		return;
	}

	if (!IsValid(TargetActor) || !IsValid(SolutionActor) || !bTracking)
	{
		return;
	}
	
	const FVector CurrentLocation = SolutionActor->GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();
	const float Distance = FVector::Dist(CurrentLocation, TargetLocation);
	TotalPositionDelta += Distance;
	++TotalPositionSamples;

	if (Distance < AcceptableDistanceDelta)
	{
		DistanceAlongSpline += Speed * DeltaTime;
		const FVector NewLocation = SplineComponent->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
		TargetActor->SetActorLocation(NewLocation);
	}
}

bool UCAsyncAction_FollowTargetTask::IsObjectCloseToEnd() const
{
	UE_LOG(LogTemp, Log, TEXT("Distance along splint: %.2f"), DistanceAlongSpline);
	UE_LOG(LogTemp, Log, TEXT("Spline Length: %.2f"), SplineComponent->GetSplineLength());
	UE_LOG(LogTemp, Log, TEXT("Delta: %.2f"), DistanceAlongSpline - SplineComponent->GetSplineLength());
	return DistanceAlongSpline > (SplineComponent->GetSplineLength() - AcceptableDistanceDelta);
}

void UCAsyncAction_FollowTargetTask::SetIsTracking(bool bNewValue)
{
	bTracking = bNewValue;
	if (bTracking)
	{
		TimeStartedTracking = TargetActor->GetWorld()->GetTimeSeconds();
	}
	else
	{
		TimeTracked += TargetActor->GetWorld()->GetTimeSeconds() - TimeStartedTracking;
	}
}

void UCAsyncAction_FollowTargetTask::GetTrackedData(float& OutAveragePositionDelta, float& OutTotalPositionDelta,
	float& OutTotalPositionSamples, float& OutDuration)
{
	OutAveragePositionDelta = TotalPositionDelta / TotalPositionSamples;
	OutTotalPositionDelta = TotalPositionDelta;
	OutTotalPositionSamples = TotalPositionSamples;
	OutDuration = TimeTracked;
}
