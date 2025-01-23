#pragma once

#include "CoreMinimal.h"

#include "HTTypes.generated.h"

#define HAS_REQUIRED_BITS(RequiredBitmask, CheckBitmask) ((RequiredBitmask & CheckBitmask) == RequiredBitmask)
#define TEST_BIT(Bitmask, Bit) (Bitmask & static_cast<uint8>(Bit) > 0)
#define SET_BIT(Bitmask, Bit) (Bitmask |= static_cast<uint8>(Bit))
#define BIT(Bit) 1 << Bit

UENUM()
enum class ETargetHand : uint8
{
	Left,
	Right,
	Both
};

UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ETargetHandLocation : uint8
{
	Thumb = BIT(0),
	Index = BIT(1),
	Middle = BIT(2),
	Ring = BIT(3),
	Pinky = BIT(4),
	Palm = BIT(5)
};
ENUM_CLASS_FLAGS(ETargetHandLocation);

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