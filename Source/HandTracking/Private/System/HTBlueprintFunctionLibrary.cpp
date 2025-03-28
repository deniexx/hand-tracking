// Fill out your copyright notice in the Description page of Project Settings.


#include "System/HTBlueprintFunctionLibrary.h"

#include "System/HTFeedbackSubsystem.h"
#include "System/HTResultWriterSubsystem.h"

UHTFeedbackSubsystem* UHTBlueprintFunctionLibrary::GetHTFeedbackSubsystem(const UObject* WorldContextObject)
{
	if (UWorld* World = WorldContextObject->GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UHTFeedbackSubsystem>();
		}
	}

	return nullptr;
}

void UHTBlueprintFunctionLibrary::ApplyHandFeedback(const UObject* WorldContextObject, FHandFeedbackConfig HandFeedbackConfig)
{
	if (UHTFeedbackSubsystem* FeedbackSubsystem = GetHTFeedbackSubsystem(WorldContextObject))
	{
		FeedbackSubsystem->ApplyFeedback(HandFeedbackConfig);
	}
}

void UHTBlueprintFunctionLibrary::ToggleHandFeedback(const UObject* WorldContextObject, bool bEnabled)
{
	if (UHTFeedbackSubsystem* FeedbackSubsystem = GetHTFeedbackSubsystem(WorldContextObject))
	{
		FeedbackSubsystem->ToggleHandHaptics(bEnabled);
	}	
}

bool UHTBlueprintFunctionLibrary::IsInMotionControllerConfig(const UObject* WorldContextObject)
{
	if (UHTResultWriterSubsystem* ResultWriter = WorldContextObject->GetWorld()->GetGameInstance()->GetSubsystem<UHTResultWriterSubsystem>())
	{
		return ResultWriter->IsInMotionControllerConfig();
	}

	return false;
}

FString UHTBlueprintFunctionLibrary::GetStringFromFinger(ETargetHandLocation Target)
{
	switch (Target)
	{
	case ETargetHandLocation::Index:
		return FString("Index");
	case ETargetHandLocation::Middle:
		return FString("Middle");
	case ETargetHandLocation::Ring:
		return FString("Ring");
	case ETargetHandLocation::Pinky:
		return FString("Pinky");
	case ETargetHandLocation::Thumb:
		return FString("Thumb");
	case ETargetHandLocation::Palm:
		return FString("Palm");
	default:
		return FString("Unknown");
	}
}

void UHTBlueprintFunctionLibrary::LowPassFilter_RollingAverage(const FVector& LastAverage, const FVector& NewSample,
	FVector& NewAverage, const int32 NumSamples)
{
	NewAverage = LastAverage;
	NewAverage -= NewAverage / NumSamples;
	NewAverage += NewSample / NumSamples;
}
