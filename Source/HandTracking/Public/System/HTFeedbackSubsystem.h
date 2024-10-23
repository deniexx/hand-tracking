// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HTFeedbackSubsystem.generated.h"

class USerialCom;
/**
 * 
 */
UCLASS()
class HANDTRACKING_API UHTFeedbackSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	/**
	 * Applies feedback to the player's hands with the specified configuration 
	 * @param Config The configuration used to determine the applied feedback
	 */
	UFUNCTION(BlueprintCallable, Category = "HTFeedbackSubsystem")
	void ApplyFeedback(const FHandFeedbackConfig& Config);
	
private:

	TObjectPtr<USerialCom> RightHandSerialCom;
};
