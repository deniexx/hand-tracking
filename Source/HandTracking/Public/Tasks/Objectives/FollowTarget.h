// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/HTTaskObjective.h"
#include "FollowTarget.generated.h"

class UCAsyncAction_FollowTargetTask;
/**
 * 
 */
UCLASS()
class HANDTRACKING_API UFollowTarget : public UHTTaskObjective
{
	GENERATED_BODY()

public:

	virtual void Activate_Implementation(UObject* InWorldContextManual) override;

	virtual void Complete_Implementation() override;

	/** Async task to track and follow the objects */
	UPROPERTY(EditAnywhere, Category = "Objective")
	FGameplayTag SolutionObjectTag;	
	
	UPROPERTY(EditAnywhere, Category = "Objective")
	FGameplayTag TargetTag;

	UPROPERTY(EditAnywhere, Category = "Objective")
	FGameplayTag SplineActorTag;

	UPROPERTY(EditAnywhere, Category = "Objective")
	float MovementSpeed = 10.f;

	UPROPERTY(EditAnywhere, Category = "Objective")
	float AcceptableDistanceDelta = 10.f;

protected:
	
	UFUNCTION()
	void BeginTracking(AHTTaskActor* TaskActor);

	UFUNCTION()
	void StopTracking(AHTTaskActor* TaskActor);
	
private:

	UPROPERTY()
	UCAsyncAction_FollowTargetTask* FollowTargetTask;
};
