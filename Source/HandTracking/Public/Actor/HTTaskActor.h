// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Interactable.h"
#include "HTTaskActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrabUpdated, AHTTaskActor*, TaskActor);

/**
 * Default class all Task Actors should inherit from.
 * Provides delegates for when an object has been grabbed or dropped,
 * Includes tags to identify the actor and an optional tag in case it is needed
 */
UCLASS()
class HANDTRACKING_API AHTTaskActor : public AInteractable
{
	GENERATED_BODY()

public:

	/** Tag used to allow for this actor to be found by the Objectives */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HTTaskActor")
	FGameplayTag TaskActorTag;

	/** An option tag, usually signifying a target actor to be used in any way required by the Objective */
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

	/** Checks for if a grab pose is available for the grabbing hand */
	bool HasAnyGrabPoses() const;
};
