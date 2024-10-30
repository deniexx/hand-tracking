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

	void SendFeedback(USerialCom* Com, const FHandFeedbackConfig& Config) const;
	
private:

	// @NOTE (Denis): Unsure if these are required, but they make sure that there is at least 20ms delay between feedback to hands, might need to change this to be per finger, rather than per hand 
	double LastFeedbackTimeLeft = 0;
	double LastFeedbackTimeRight = 0;
	static constexpr double MinDelayTime = 0.02;

	UPROPERTY()
	TMap<ETargetHandLocation, double> FingerToLastTimeFeedbackSentRight;

	UPROPERTY()
	TMap<ETargetHandLocation, double> FingerToLastTimeFeedbackSentLeft;

	UPROPERTY()
	TObjectPtr<USerialCom> RightHandSerialCom;

	UPROPERTY()
	TObjectPtr<USerialCom> LeftHandSerialCom;
};
