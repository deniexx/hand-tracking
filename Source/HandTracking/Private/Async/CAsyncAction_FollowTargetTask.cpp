// Fill out your copyright notice in the Description page of Project Settings.


#include "Async/CAsyncAction_FollowTargetTask.h"

#include "SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"

UCAsyncAction_FollowTargetTask* UCAsyncAction_FollowTargetTask::Execute(AActor* SolutionActor,
	USplineComponent* SplineComponent, float AcceptableDistanceDelta, const FLinearColor& GoodColor, const FLinearColor& BadColor)
{
	UCAsyncAction_FollowTargetTask* Action = NewObject<UCAsyncAction_FollowTargetTask>(SolutionActor);
	Action->RegisterWithGameInstance(SolutionActor);
	
	Action->SolutionActor = SolutionActor;
	Action->SplineComponent = SplineComponent;
	Action->AcceptableDistanceDelta = AcceptableDistanceDelta;
	Action->GoodColor = GoodColor;
	Action->BadColor = BadColor;
	Action->PrimitiveComponent = Cast<UPrimitiveComponent>(SolutionActor->GetRootComponent());
	Action->DynamicMaterial = Action->PrimitiveComponent->CreateDynamicMaterialInstance(0);
	Action->PrimitiveComponent->SetMaterial(0, Action->DynamicMaterial);
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

	if (!IsValid(SolutionActor) || !bTracking)
	{
		return;
	}
	
	const FVector CurrentLocation = SolutionActor->GetActorLocation();
	const FVector TargetLocation = SplineComponent->FindLocationClosestToWorldLocation(CurrentLocation, ESplineCoordinateSpace::World);
	const float Distance = FVector::Dist(CurrentLocation, TargetLocation);
	TotalPositionDelta += Distance;
	++TotalPositionSamples;

	MaxDistanceDelta = FMath::Max(Distance, MaxDistanceDelta);

	const float Alpha = FMath::Clamp(Distance / AcceptableDistanceDelta, 0.f, 1.f);
	const FLinearColor FinalColor = UKismetMathLibrary::LinearColorLerp(GoodColor, BadColor, Alpha);
	DynamicMaterial->SetVectorParameterValue("HologramColor", FinalColor);
	
	const float Distance2D = FVector::Dist2D(CurrentLocation, TargetLocation);
	if (Distance2D < AcceptableDistanceDelta)
	{
		OnLocationCloseToEnd.Broadcast();
	}

	/** This does not work as we can not set a tick group that will be post parent location setting
	if (Distance > MaxDistanceDelta)
	{
		const FVector ToObject = (TargetLocation - CurrentLocation).GetSafeNormal();
		SolutionActor->SetActorLocation(TargetLocation + (ToObject * AcceptableDistanceDelta));
	}
	*/
}

bool UCAsyncAction_FollowTargetTask::IsObjectCloseToEnd() const
{
	const FVector CurrentLocation = SolutionActor->GetActorLocation();
	const FVector TargetLocation = SplineComponent->GetLocationAtSplinePoint(SplineComponent->GetNumberOfSplinePoints(),
		ESplineCoordinateSpace::World);

	return FVector::Dist2D(CurrentLocation, TargetLocation) < AcceptableDistanceDelta;
}

void UCAsyncAction_FollowTargetTask::SetIsTracking(bool bNewValue)
{
	bTracking = bNewValue;
}

void UCAsyncAction_FollowTargetTask::GetTrackedData(float& OutAveragePositionDelta, float& OutTotalPositionDelta,
	float& OutTotalPositionSamples, float& OutMaxDistanceDelta) const
{
	OutAveragePositionDelta = TotalPositionDelta / TotalPositionSamples;
	OutTotalPositionDelta = TotalPositionDelta;
	OutTotalPositionSamples = TotalPositionSamples;
	OutMaxDistanceDelta = MaxDistanceDelta;
}
