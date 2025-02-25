// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Async/CTickableAsyncAction.h"
#include "CAsyncAction_FollowTargetTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLocationCloseToEnd);
class USplineComponent;

/**
 * 
 */
UCLASS()
class HANDTRACKING_API UCAsyncAction_FollowTargetTask : public UCTickableAsyncAction
{
	GENERATED_BODY()

public:
	
	static UCAsyncAction_FollowTargetTask* Execute(AActor* SolutionActor, USplineComponent* SplineComponent,
		float AcceptableDistanceDelta, const FLinearColor& GoodColor, const FLinearColor& BadColor);

	virtual void Tick(float DeltaTime) override;

	bool IsObjectCloseToEnd() const;
	void SetIsTracking(bool bNewValue);
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
