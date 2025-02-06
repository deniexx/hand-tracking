// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Interactable.h"
#include "HTTaskActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrabUpdated, AHTTaskActor*, TaskActor);

/**
 * 
 */
UCLASS()
class HANDTRACKING_API AHTTaskActor : public AInteractable
{
	GENERATED_BODY()

public:

	/** Tag used to allow for this actor to be found by the Objectives */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HTTaskActor")
	FGameplayTag TaskActorTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HTTaskActor")
	FGameplayTag OptionalTag;
	
	/** Picked up */
	virtual void Interaction1_Implementation() override;
	virtual void Interaction2_Implementation() override;
	
	/** Dropped */
	virtual void Interaction3_Implementation() override;
	
	UPROPERTY(BlueprintAssignable, Category="HTTaskActor")
	FOnGrabUpdated OnGrabbed;

	UPROPERTY(BlueprintAssignable, Category="HTTaskActor")
	FOnGrabUpdated OnDropped;

private:

	bool HasAnyGrabPoses() const;
};
