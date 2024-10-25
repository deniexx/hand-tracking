#pragma once

#include "CoreMinimal.h"

#include "HTTypes.generated.h"

UENUM()
enum class ETargetHand : uint8
{
	Left,
	Right,
	Both
};

UENUM()
enum class ETargetHandLocation : uint8
{
	Thumb = 0,
	Index = 1,
	Middle = 2,
	Ring = 3,
	Pinky = 4,
	Palm = 5
};

USTRUCT(BlueprintType)
struct FHandFeedbackConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETargetHand Hand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETargetHandLocation Location;

	// Strength in 0-1
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NormalizedStrength = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 0.f;
};