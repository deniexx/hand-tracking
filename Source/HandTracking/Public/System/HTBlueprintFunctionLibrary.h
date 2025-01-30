// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HTBlueprintFunctionLibrary.generated.h"

class UHTFeedbackSubsystem;
/**
 * 
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

	UFUNCTION(BlueprintCallable, Category = "HTBlueprintFunctionLibrary")
	static FString GetStringFromFinger(ETargetHandLocation Target);

	UFUNCTION(BlueprintCallable, Category = "HTBlueprintFunctionLibrary")
	static void LowPassFilter_RollingAverage(const FVector& LastAverage, const FVector& NewSample, FVector& NewAverage, const int32 NumSamples = 10);
};
