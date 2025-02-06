// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/HTTaskObjective.h"
#include "Sortation.generated.h"

/**
 * 
 */
UCLASS()
class HANDTRACKING_API USortation : public UHTTaskObjective
{
	GENERATED_BODY()

public:
	
	virtual void Activate_Implementation(UObject* InWorldContextManual) override;

	UFUNCTION()
	void OnSortableSorted(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
