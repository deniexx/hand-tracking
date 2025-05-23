// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HTFeedbackSubsystem.generated.h"

class USerialCom;


/**
 * The subsystem handling hand haptics
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

	/**
	 * Toggles whether the hand haptics are enabled
	 * @param bEnabled TRUE - enabled, FALSE - disabled
	 */
	UFUNCTION(BlueprintCallable, Category = "HTFeedbackSubsystem")
	void ToggleHandHaptics(bool bEnabled);

private:

	/** Writes the data to the specified serial com */
	void SendFeedback(USerialCom* Com, const FHandFeedbackConfig& Config) const;
	
private:

	bool bHapticsEnabled = false;
	
	UPROPERTY()
	TMap<ETargetHandLocation, double> FingerToTimeToSendRight;

	UPROPERTY()
	TMap<ETargetHandLocation, double> FingerToTimeToSendLeft;

	UPROPERTY()
	TObjectPtr<USerialCom> RightHandSerialCom;

	UPROPERTY()
	TObjectPtr<USerialCom> LeftHandSerialCom;
};
