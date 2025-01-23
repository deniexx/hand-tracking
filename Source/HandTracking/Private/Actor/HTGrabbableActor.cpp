// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/HTGrabbableActor.h"

#include "HTTypes.h"

// Sets default values
AHTGrabbableActor::AHTGrabbableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SET_BIT(RequiredFingers, ETargetHandLocation::Thumb);

}

void AHTGrabbableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHTGrabbableActor::Grab_Implementation(APawn* GrabInstigator, UMotionControllerComponent* GrabbingMotionController)
{
	
}

void AHTGrabbableActor::Drop_Implementation(APawn* DropInstigator, UMotionControllerComponent* DroppingMotionController)
{
	
}

uint8 AHTGrabbableActor::GetRequiredPointsOfContact_Implementation() const
{
	return RequiredPointsOfContact;
}

uint8 AHTGrabbableActor::GetRequiredFingers_Implementation() const
{
	return RequiredFingers;
}
