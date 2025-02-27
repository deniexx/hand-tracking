// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/HTTaskObjective.h"
#include "CubeStacking.generated.h"

/**
 * 
 */
UCLASS()
class HANDTRACKING_API UCubeStacking : public UHTTaskObjective
{
	GENERATED_BODY()

public:

	virtual void Activate_Implementation(UObject* InWorldContextManual) override;
	virtual void Complete_Implementation() override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Objective")
	bool bEnablesPhysicsOnCubes = false;

	UPROPERTY(EditDefaultsOnly, Category = "Objective")
	bool bEnablesPhysicsOnHands = false;

	UPROPERTY(EditDefaultsOnly, Category = "Objective")
	bool bSnapToGround = false;
	
private:
	
	UFUNCTION()
	void OnObjectiveActorDropped(AHTTaskActor* TaskActor);
	
	int32 CubesStackedCorrectly = 0;
	int32 CubesStackedIncorrectly = 0;
	float AverageXYDelta = 0.f;
};
