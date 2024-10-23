// Fill out your copyright notice in the Description page of Project Settings.


#include "System/HTFeedbackSubsystem.h"

#include "SerialCom.h"
#include "HandTracking/HandTracking.h"

void UHTFeedbackSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bool bSuccess = false; 
	RightHandSerialCom = USerialCom::OpenComPort(bSuccess, 3, 9600);
	if (!RightHandSerialCom || !bSuccess)
	{
		UE_LOG(LogHandTracking, Error, TEXT("RightHandSerialCom has failed to construct"));
	}
}

void UHTFeedbackSubsystem::Deinitialize()
{
	if (RightHandSerialCom)
	{
		RightHandSerialCom->Close();
	}

	Super::Deinitialize();
}

void UHTFeedbackSubsystem::ApplyFeedback(const FHandFeedbackConfig& Config)
{
	const float ClampedStrength = FMath::Clamp(Config.NormalizedStrength, 0.f, 1.f); // Make sure strength is between 0 and 1
	
	if (Config.Hand == ETargetHand::Left || Config.Hand == ETargetHand::Both)
	{
		// @TODO (Denis): Add Left hand implementation
	}

	if (Config.Hand == ETargetHand::Right || Config.Hand == ETargetHand::Both)
	{
		if (!RightHandSerialCom)
		{
			UE_LOG(LogHandTracking, Warning, TEXT("Attempting to apply feedback to Right Hand, but the serial com is nullptr"));
			return;
		}
		uint8 Hand = (uint8)Config.Hand;
		uint8 Strength = (uint8)(255 * ClampedStrength);
		RightHandSerialCom->WriteBytes( { Hand, Strength } );
	}
}
