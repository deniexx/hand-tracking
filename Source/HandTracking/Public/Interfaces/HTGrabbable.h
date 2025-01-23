// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MotionControllerComponent.h"
#include "UObject/Interface.h"
#include "HTGrabbable.generated.h"

class UMotionControllerComponent;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHTGrabbable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HANDTRACKING_API IHTGrabbable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTGrabbable")
	void Grab(APawn* GrabInstigator, UMotionControllerComponent* GrabbingMotionController);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTGrabbable")
	void Drop(APawn* DropInstigator, UMotionControllerComponent* DroppingMotionController);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTGrabbable")
	uint8 GetRequiredPointsOfContact() const;

	/** @return A bitmask field, that specifies the required fingers */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTGrabbable")
	uint8 GetRequiredFingers() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTGrabbable")
	void OnGrabbed(APawn* GrabInstigator, UMotionControllerComponent* GrabbingMotionController);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTGrabbable")
	void OnDropped(APawn* DropInstigator, UMotionControllerComponent* DroppingMotionController);
};
