// Fill out your copyright notice in the Description page of Project Settings.


#include "System/HTBlueprintFunctionLibrary.h"

#include "System/HTFeedbackSubsystem.h"

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
