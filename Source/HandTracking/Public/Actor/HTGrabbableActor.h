// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HTGrabbable.h"
#include "HTGrabbableActor.generated.h"

UCLASS()
class HANDTRACKING_API AHTGrabbableActor : public AActor, public IHTGrabbable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHTGrabbableActor();

	/** IHTGrabbale Interface Start */
	virtual void Grab_Implementation(APawn* GrabInstigator, UMotionControllerComponent* GrabbingMotionController) override;
	virtual void Drop_Implementation(APawn* DropInstigator, UMotionControllerComponent* DroppingMotionController) override;
	virtual uint8 GetRequiredPointsOfContact_Implementation() const override;
	virtual uint8 GetRequiredFingers_Implementation() const override;
	/** IHTGrabbale Interface End */

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HTGrabbable")
	uint8 RequiredPointsOfContact = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HTGrabbable", meta = (Bitmask, BitmaskEnum = ETargetHandLocation))
	uint8 RequiredFingers;
	
};
