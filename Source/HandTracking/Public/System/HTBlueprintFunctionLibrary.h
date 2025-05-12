// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HTBlueprintFunctionLibrary.generated.h"

class UHTFeedbackSubsystem;

/**
 * Blueprint function library with utility functions, mostly focused on the haptics
 */
UCLASS()
class HANDTRACKING_API UHTBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	/**
	 * Gets the hand tracking feedback subsystem 
	 * @param WorldContextObject The object used to get the world from 
	 * @return The hand tracking feedback system, can be null
	 */
	UFUNCTION(BlueprintCallable, Category = "HTBlueprintFunctionLibrary", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static UHTFeedbackSubsystem* GetHTFeedbackSubsystem(const UObject* WorldContextObject);

	/**
	 * Applies hand feedback with a specified configuration 
	 * @param WorldContextObject The object used to get the world from
	 * @param HandFeedbackConfig The feedback configuration to be applied
	 */
	UFUNCTION(BlueprintCallable, Category = "HTBlueprintFunctionLibrary", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static void ApplyHandFeedback(const UObject* WorldContextObject, FHandFeedbackConfig HandFeedbackConfig);

	/**
	 * Toggles whether haptic feedback is applied to the hands
	 * @param WorldContextObject The object used to get the world from
	 * @param bEnabled Whether the haptic feedback should be enabled
	 */
	UFUNCTION(BlueprintCallable, Category = "HTBlueprintFunctionLibrary", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static void ToggleHandFeedback(const UObject* WorldContextObject, bool bEnabled);

	/**
	 * Checks if the game is in the motion controller configuration
	 * @param WorldContextObject The object used to get the world from
	 * @return Whether the player is in the motion controller configuration
	 */
	UFUNCTION(BlueprintCallable, Category = "HTBlueprintFunctionLibrary", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static bool IsInMotionControllerConfig(const UObject* WorldContextObject);

	/**
	 * Converts the ETargetHandLocation to a string
	 * @param Target The enum value to be converted to a string
	 * @return The enum value as a string
	 */
	UFUNCTION(BlueprintCallable, Category = "HTBlueprintFunctionLibrary")
	static FString GetStringFromFinger(ETargetHandLocation Target);

	/**
	 * Runs a low pass filter rolling average on a vector
	 * @param LastAverage The last average
	 * @param NewSample The current sample to be added
	 * @param NewAverage The new calculated average
	 * @param NumSamples The number of samples added
	 */
	UFUNCTION(BlueprintCallable, Category = "HTBlueprintFunctionLibrary")
	static void LowPassFilter_RollingAverage(const FVector& LastAverage, const FVector& NewSample, FVector& NewAverage, const int32 NumSamples = 10);

	UFUNCTION(BlueprintCallable, Category = "HTBlueprintFunctionLibrary")
	static void GenerateRandomNumbersArray(TArray<int32>& OutNumbers, int32 Amount = 4);

	UFUNCTION(BlueprintCallable, Category = "HTBlueprintFunctionLibrary", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static void SendFeedbackBasedOnArray(const UObject* WorldContextObject, int32 Number, float Duration = 1.f);
};
