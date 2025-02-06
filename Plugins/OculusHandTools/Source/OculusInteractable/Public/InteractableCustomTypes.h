// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "InteractableCustomTypes.generated.h"

UENUM(BlueprintType)
enum class EGrabHand : uint8
{
	None,
	Left,
	Right
};