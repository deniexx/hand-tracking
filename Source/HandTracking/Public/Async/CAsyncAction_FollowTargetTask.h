// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Async/CTickableAsyncAction.h"
#include "CAsyncAction_FollowTargetTask.generated.h"

class USplineComponent;
/**
 * 
 */
UCLASS()
class HANDTRACKING_API UCAsyncAction_FollowTargetTask : public UCTickableAsyncAction
{
	GENERATED_BODY()

public:
	
	static UCAsyncAction_FollowTargetTask* Execute(AActor* TargetActor, AActor* SolutionActor,
		USplineComponent* SplineComponent, float MovementSpeed, float AcceptableDistanceDelta);

	virtual void Tick(float DeltaTime) override;

	bool IsObjectCloseToEnd() const;
	void SetIsTracking(bool bNewValue);
	void GetTrackedData(float& OutAveragePositionDelta, float& OutTotalPositionDelta, float& OutTotalPositionSamples, float& OutDuration);
	
private:
	
	bool bTracking = false;

	float Speed = 0.f;
	float TotalPositionDelta = 0.f;
	float TotalPositionSamples = 0.f;
	float TimeTracked = 0.f;
	float TimeStartedTracking = 0.f;
	float AcceptableDistanceDelta = 0.f;
	
	float DistanceAlongSpline = 0.f;
	
	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY()
	AActor* SolutionActor;

	UPROPERTY()
	USplineComponent* SplineComponent;
};
