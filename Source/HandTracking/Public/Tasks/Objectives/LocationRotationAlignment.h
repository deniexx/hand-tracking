// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/HTTaskObjective.h"
#include "LocationRotationAlignment.generated.h"

/**
 * 
 */
UCLASS()
class HANDTRACKING_API ULocationRotationAlignment : public UHTTaskObjective
{
	GENERATED_BODY()

public:

	ULocationRotationAlignment();
	
	virtual void Activate_Implementation(UObject* InWorldContextManual) override;

	virtual void Complete_Implementation() override;

	/** Objects that user should place into the targets */
	UPROPERTY(EditAnywhere, Category = "Objective")
	FGameplayTagContainer SolutionObjects;
	
	UPROPERTY(EditAnywhere, Category = "Objective")
	FGameplayTagContainer Targets;

	UPROPERTY(EditAnywhere, Category = "Objective")
	float MinimumAcceptableDistance = 10.f;
	
	TArray<TPair<AHTTaskActor*, AHTTaskActor*>> SolutionsToTargets;
	
protected:

	UFUNCTION()
	void OnSolutionActorDropped(AHTTaskActor* TaskActor);
};
