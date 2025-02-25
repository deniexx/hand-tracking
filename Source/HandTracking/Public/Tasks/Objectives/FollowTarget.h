// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/HTTaskObjective.h"
#include "FollowTarget.generated.h"

class USplineComponent;
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
	FGameplayTag SplineActorTag;

	UPROPERTY(EditAnywhere, Category = "Objective")
	float AcceptableDistanceDelta = 10.f;

	UPROPERTY(EditAnywhere, Category = "Objective")
	FLinearColor GoodColor;

	UPROPERTY(EditAnywhere, Category = "Objective")
	FLinearColor BadColor;

protected:

	UFUNCTION()
	void OnSolutionObjectHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnSolutionObjectGrabbbed(AHTTaskActor* TaskActor);

	UFUNCTION()
	void OnSolutionObjectDropped(AHTTaskActor* TaskActor);

	UFUNCTION()
	void OnLocationCloseToEnd();
	
private:

	int32 NumberOfHits = 0;
	float TimeStarted = 0.f;

	UPROPERTY()
	AActor* SplineActor;

	UPROPERTY()
	USplineComponent* SplineComponent;

	UPROPERTY()
	AHTTaskActor* SolutionObject;

	UPROPERTY()
	UCAsyncAction_FollowTargetTask* FollowTargetTask;
};
