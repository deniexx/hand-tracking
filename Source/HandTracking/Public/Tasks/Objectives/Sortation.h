// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/HTTaskObjective.h"
#include "Sortation.generated.h"

USTRUCT(BlueprintType)
struct FSortableActorData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FGameplayTag SortableTag;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;

	UPROPERTY(EditAnywhere)
	FGameplayTag TargetTag;
};

/**
 * 
 */
UCLASS()
class HANDTRACKING_API USortation : public UHTTaskObjective
{
	GENERATED_BODY()

public:
	virtual void Activate_Implementation(UObject* InWorldContextManual) override;

	virtual void Complete_Implementation() override;
	
	int32 ObjectsSortedCorrectly = 0;
	int32 ObjectsSortedIncorrectly = 0;
	
	UPROPERTY(EditAnywhere, Category = "Objective")
	FGameplayTagContainer TargetTags;

	UPROPERTY(EditAnywhere, Category = "Objective")
	TArray<FSortableActorData> SortableActors;

	UPROPERTY(EditAnywhere, Category = "Objective")
	float SortationAcceptanceRange = 10.f;

	UPROPERTY(EditAnywhere, Category = "Objective")
	int32 NumberOfSortables = 12;

	UPROPERTY(EditAnywhere, Category = "Objective")
	float DistanceFromCenter = 15.f;
	
	UPROPERTY()
	TArray<AHTTaskActor*> TargetActors;

	void SetActorMaterial(AHTTaskActor* TaskActor, UMaterialInterface* Material);

	int32 LastIndex = 0;
	TArray<FVector> GetLocationsInCircleAroundCenter();
	
	UFUNCTION()
	void ScaleDownActor(AActor* TargetActor, AActor* Optional, float CurrentValue);

	UFUNCTION()
	void OnTweenFinished(AActor* TargetActor);

	UFUNCTION()
	void OnSortableDropped(AHTTaskActor* TaskActor);
};
