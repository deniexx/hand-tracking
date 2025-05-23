// Fill out your copyright notice in the Description page of Project Settings.


#include "System/HTFeedbackSubsystem.h"

#include "SerialCom.h"
#include "HandTracking/HandTracking.h"
#include "System/HTBlueprintFunctionLibrary.h"

void UHTFeedbackSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bool bSuccess = false;
	
	// Create serial com ports and log if failed
	RightHandSerialCom = USerialCom::OpenComPort(bSuccess, 11, 9600);
	if (!RightHandSerialCom || !bSuccess)
	{
		UE_LOG(LogHandTracking, Error, TEXT("RightHandSerialCom has failed to construct"));
	}

	LeftHandSerialCom = USerialCom::OpenComPort(bSuccess, 6, 9600);
	if (!LeftHandSerialCom || !bSuccess)
	{
		UE_LOG(LogHandTracking, Error, TEXT("LeftHandSerialCom has failed to construct"));
	}

	// Initialize map of fingers to times
	const double TimeSeconds = GetWorld()->GetTimeSeconds();
	FingerToTimeToSendLeft.Add({ ETargetHandLocation::Thumb, TimeSeconds });
	FingerToTimeToSendLeft.Add({ ETargetHandLocation::Index, TimeSeconds });
	FingerToTimeToSendLeft.Add({ ETargetHandLocation::Middle, TimeSeconds });
	FingerToTimeToSendLeft.Add({ ETargetHandLocation::Ring, TimeSeconds });
	FingerToTimeToSendLeft.Add({ ETargetHandLocation::Pinky, TimeSeconds });

	FingerToTimeToSendRight.Add({ ETargetHandLocation::Thumb, TimeSeconds });
	FingerToTimeToSendRight.Add({ ETargetHandLocation::Index, TimeSeconds });
	FingerToTimeToSendRight.Add({ ETargetHandLocation::Middle, TimeSeconds });
	FingerToTimeToSendRight.Add({ ETargetHandLocation::Ring, TimeSeconds });
	FingerToTimeToSendRight.Add({ ETargetHandLocation::Pinky, TimeSeconds });
}

void UHTFeedbackSubsystem::Deinitialize()
{
	if (RightHandSerialCom)
	{
		RightHandSerialCom->Close();
	}

	if (LeftHandSerialCom)
	{
		LeftHandSerialCom->Close();
	}

	Super::Deinitialize();
}

void UHTFeedbackSubsystem::ApplyFeedback(const FHandFeedbackConfig& Config)
{
	if (!bHapticsEnabled)
	{
		return;
	}
	
	if (Config.Hand == ETargetHand::Left || Config.Hand == ETargetHand::Both)
	{
		if (!LeftHandSerialCom)
		{
			UE_LOG(LogHandTracking, Warning, TEXT("Attempting to apply feedback to Left Hand, but the serial com is nullptr"))
			return;
		}

		// Can we even send haptics, depending on the time we last send
		if (FingerToTimeToSendLeft[Config.Location] > GetWorld()->GetTimeSeconds()) // Maybe add a queue?
		{
			return;
		}

		FingerToTimeToSendLeft[Config.Location] = GetWorld()->GetTimeSeconds() + Config.Duration;
		SendFeedback(LeftHandSerialCom, Config);
	}

	if (Config.Hand == ETargetHand::Right || Config.Hand == ETargetHand::Both)
	{
		if (!RightHandSerialCom)
		{
			UE_LOG(LogHandTracking, Warning, TEXT("Attempting to apply feedback to Right Hand, but the serial com is nullptr"));
			return;
		}

		// Can we even send haptics, depending on the time we last send
		if (FingerToTimeToSendRight[Config.Location] > GetWorld()->GetTimeSeconds()) // Maybe add a queue?
		{
			return;
		}

		FingerToTimeToSendRight[Config.Location] = GetWorld()->GetTimeSeconds() + Config.Duration;
		SendFeedback(RightHandSerialCom, Config);
	}
}

void UHTFeedbackSubsystem::ToggleHandHaptics(bool bEnabled)
{
	bHapticsEnabled = bEnabled;
}

void UHTFeedbackSubsystem::SendFeedback(USerialCom* Com, const FHandFeedbackConfig& Config) const
{
	// Make sure strength is between 0 and 1
	const float ClampedStrength = FMath::Clamp(Config.NormalizedStrength, 0.f, 1.f); 

	const uint8 Location = (uint8)Config.Location;
	const uint8 Strength = (uint8)(255.f * ClampedStrength);
	const uint8 Padding1 = 0;
	const uint8 Padding2 = 0;
	
	TArray<uint8> RawData = { Location, Strength };

	// Taken from USerialCom::FloatToBytes, but inlined, as it removes 1 TArray allocation
	RawData.Append(reinterpret_cast<const uint8*>(&Config.Duration), 4);
	RawData.Append({ Padding1, Padding2 });

	const FString HandString = Config.Hand == ETargetHand::Left ? "Left" : "Right";

	if (!Com->WriteBytes(RawData))
	{
		UE_LOG(LogHandTracking, Error, TEXT("Failed to send feedback to %ls hand"), *HandString);
	}
	else
	{
		const FString FingerString = UHTBlueprintFunctionLibrary::GetStringFromFinger(Config.Location);
		UE_LOG(LogHandTracking, Log, TEXT("Sending feedback to %ls hand, %ls finger with %d index."),
			*HandString, *FingerString, Location);
	}
}
