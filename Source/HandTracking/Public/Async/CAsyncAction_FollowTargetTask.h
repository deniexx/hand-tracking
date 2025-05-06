// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Async/CTickableAsyncAction.h"
#include "CAsyncAction_FollowTargetTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLocationCloseToEnd);
class USplineComponent;

/**
 * An async action that features the ticking behaviour for the follow target task,
 * like updating the color of the object, checking if it as the finish line,
 * and recording data to be used when writing data results
 */
UCLASS()
class HANDTRACKING_API UCAsyncAction_FollowTargetTask : public UCTickableAsyncAction
{
	GENERATED_BODY()

public:

	/** Creates and Activates the action */
	static UCAsyncAction_FollowTargetTask* Execute(AActor* SolutionActor, USplineComponent* SplineComponent,
		float AcceptableDistanceDelta, const FLinearColor& GoodColor, const FLinearColor& BadColor);
	
	virtual void Tick(float DeltaTime) override;

	/** Check if the object is within the given acceptable distance delta to the end point*/
	bool IsObjectCloseToEnd() const;

	/**
	 * Sets whether we should keep on tracking, disabled when the object is not grabbed, to save performance
	 * and not dirty the distance delta data
	 * @param bNewValue TRUE - tracking, FALSE - not tracking 
	 */
	void SetIsTracking(bool bNewValue);

	/** Fills the incoming data with the tracked data held in this object */
	void GetTrackedData(float& OutAveragePositionDelta, float& OutTotalPositionDelta, float& OutTotalPositionSamples,
		float& OutMaxDistanceDelta) const;

	FOnLocationCloseToEnd OnLocationCloseToEnd;

private:
	
	bool bTracking = false;

	float TotalPositionDelta = 0.f;
	float TotalPositionSamples = 0.f;
	float AcceptableDistanceDelta = 0.f;
	float MaxDistanceDelta = 0.f;

	FLinearColor GoodColor;
	FLinearColor BadColor;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;
	
	UPROPERTY()
	UPrimitiveComponent* PrimitiveComponent;
	
	UPROPERTY()
	AActor* SolutionActor;

	UPROPERTY()
	USplineComponent* SplineComponent;
};
