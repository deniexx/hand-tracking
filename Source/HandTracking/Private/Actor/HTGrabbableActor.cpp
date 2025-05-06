// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/HTGrabbableActor.h"

#include "HTTypes.h"

// Sets default values
ADEPRECATED_HTGrabbableActor::ADEPRECATED_HTGrabbableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SET_BIT(RequiredFingers, ETargetHandLocation::Thumb);

}

void ADEPRECATED_HTGrabbableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADEPRECATED_HTGrabbableActor::Grab_Implementation(APawn* GrabInstigator, UMotionControllerComponent* GrabbingMotionController)
{
	
}

void ADEPRECATED_HTGrabbableActor::Drop_Implementation(APawn* DropInstigator, UMotionControllerComponent* DroppingMotionController)
{
	
}

uint8 ADEPRECATED_HTGrabbableActor::GetRequiredPointsOfContact_Implementation() const
{
	return RequiredPointsOfContact;
}

uint8 ADEPRECATED_HTGrabbableActor::GetRequiredFingers_Implementation() const
{
	return RequiredFingers;
}
