// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/HTHandsPawn.h"

#include <bit>

#include "OculusXRHandComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "Interfaces/HTGrabbable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "System/HTBlueprintFunctionLibrary.h"

int32 GDebugFingerTrace = 0;
static FAutoConsoleVariableRef CVarShowDebugFingerTrace(
	TEXT("ShowDebug.FingerTrace"),
	GDebugFingerTrace,
	TEXT("Draws debug info for Finger Traces")
	TEXT("0 - Do not draw debug info")
	TEXT("1 - Draw debug info"),
	ECVF_Cheat
);

// Sets default values
AHTHandsPawn::AHTHandsPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(GetRootComponent());

	MotionControllerLeft = CreateDefaultSubobject<UMotionControllerComponent>("MotionControllerLeft");
	MotionControllerRight = CreateDefaultSubobject<UMotionControllerComponent>("MotionControllerRight");

	OculusXRHandLeft = CreateDefaultSubobject<UOculusXRHandComponent>("OculusXRHandLeft");
	OculusXRHandRight = CreateDefaultSubobject<UOculusXRHandComponent>("OculusXRHandRight");

	MotionControllerLeft->SetupAttachment(GetRootComponent());
	MotionControllerRight->SetupAttachment(GetRootComponent());

	OculusXRHandLeft->SetupAttachment(MotionControllerLeft);
	OculusXRHandRight->SetupAttachment(MotionControllerRight);

	MotionControllerLeft->SetTrackingMotionSource("Left");
	MotionControllerRight->SetTrackingMotionSource("Right");
	
	OculusXRHandLeft->SkeletonType = EOculusXRHandType::HandLeft;
	OculusXRHandRight->SkeletonType = EOculusXRHandType::HandRight;

	OculusXRHandLeft->MeshType = EOculusXRHandType::HandLeft;
	OculusXRHandRight->MeshType = EOculusXRHandType::HandRight;
}

// Called when the game starts or when spawned
void AHTHandsPawn::BeginPlay()
{
	Super::BeginPlay();
	
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Stage);
}

// Called every frame
void AHTHandsPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TArray<AActor*> IgnoredActors = { this };
	const bool bDrawDebug = GDebugFingerTrace > 0;

	if (OculusXRHandRight->bSkeletalMeshInitialized)
	{
		FingerCollisionRight.Reset();
		FHitResult HitResult;
		if (TraceFinger(OculusXRHandRight, "Thumb Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			TryGrabItem(FingerCollisionRight, ETargetHandLocation::Thumb, HitResult, HeldActorsRightHand);
			DoFeedback(ETargetHand::Right, ETargetHandLocation::Thumb, 0.5f);
		}
		else
		{
			TryReleaseItem(FingerCollisionRight, HeldActorsRightHand);
		}

		if (TraceFinger(OculusXRHandRight, "Index Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			TryGrabItem(FingerCollisionRight, ETargetHandLocation::Index, HitResult, HeldActorsRightHand);
			DoFeedback(ETargetHand::Right, ETargetHandLocation::Index, 0.5f);
		}
		else
		{
			TryReleaseItem(FingerCollisionRight, HeldActorsRightHand);
		}

		if (TraceFinger(OculusXRHandRight, "Middle Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			TryGrabItem(FingerCollisionRight, ETargetHandLocation::Middle, HitResult, HeldActorsRightHand);
			DoFeedback(ETargetHand::Right, ETargetHandLocation::Middle, 0.5f);
		}
		else
		{
			TryReleaseItem(FingerCollisionRight, HeldActorsRightHand);
		}

		if (TraceFinger(OculusXRHandRight, "Ring Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			TryGrabItem(FingerCollisionRight, ETargetHandLocation::Ring, HitResult, HeldActorsRightHand);
			DoFeedback(ETargetHand::Right, ETargetHandLocation::Ring, 0.5f);
		}
		else
		{
			TryReleaseItem(FingerCollisionRight, HeldActorsRightHand);
		}

		if (TraceFinger(OculusXRHandRight, "Pinky Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			TryGrabItem(FingerCollisionRight, ETargetHandLocation::Pinky, HitResult, HeldActorsRightHand);
			DoFeedback(ETargetHand::Right, ETargetHandLocation::Pinky, 0.5f);
		}
		else
		{
			TryReleaseItem(FingerCollisionRight, HeldActorsRightHand);
		}
	}
}

// Called to bind functionality to input
void AHTHandsPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
}

bool AHTHandsPawn::TraceFinger(UOculusXRHandComponent* HandComponent, FName SocketName, const TArray<AActor*>& IgnoredActors, bool bDrawDebug, FHitResult& OutResult) const
{
	const EDrawDebugTrace::Type DrawDebug = bDrawDebug ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
	const FVector TraceStart = HandComponent->GetSocketLocation(SocketName);
	return UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, TraceStart, TraceSphereRadius, TraceChannels, false, IgnoredActors, DrawDebug, OutResult, true);
}

void AHTHandsPawn::DoFeedback(ETargetHand Hand, ETargetHandLocation Location, float Duration) const
{
	FHandFeedbackConfig Config;
	Config.Hand = Hand;
	Config.NormalizedStrength = 1.f;
	Config.Location = Location;
	Config.Duration = Duration;
	UHTBlueprintFunctionLibrary::ApplyHandFeedback(this, Config);
}

bool AHTHandsPawn::AreGrabRequirementsMet(const FFingerCollisionData& FingerCollision)
{
	int32 RequiredPointsOfContact = 3;
	uint8 RequiredFingers = 0;
	
	if (FingerCollision.HitActor && FingerCollision.HitActor->Implements<UHTGrabbable>())
	{
		RequiredPointsOfContact = IHTGrabbable::Execute_GetRequiredPointsOfContact(FingerCollision.HitActor);
		RequiredFingers = IHTGrabbable::Execute_GetRequiredFingers(FingerCollision.HitActor);
	}
	
	/** @NOTE (Denis): Maybe wrap this in a function? */
	const int32 NumFingers = std::popcount(FingerCollision.FingersColliding);
	return NumFingers >= RequiredPointsOfContact && HAS_REQUIRED_BITS(RequiredFingers, FingerCollision.FingersColliding);
}

void AHTHandsPawn::TryGrabItem(FFingerCollisionData& FingerCollision, ETargetHandLocation Location, const FHitResult& HitResult, TArray<AActor*>& HeldActors) const
{
	if (HitResult.GetActor() == nullptr)
	{
		return;
	}

	FingerCollision.HitActor = HitResult.GetActor();
	SET_BIT(FingerCollision.FingersColliding, Location);
	
	if (AreGrabRequirementsMet(FingerCollision))
	{
		// @TODO (Denis): This needs to be replaced by the Grab Interface function and force the object to maintain it's position, etc... relative to the hand rather than using parenting
		HitResult.GetActor()->AttachToComponent(OculusXRHandRight, FAttachmentTransformRules::KeepWorldTransform);
		HeldActors.Add(HitResult.GetActor());
	}
}

void AHTHandsPawn::TryReleaseItem(FFingerCollisionData& FingerCollision, TArray<AActor*>& HeldActors)
{
	// If grab requirements are not met, drop the items
	if (!AreGrabRequirementsMet(FingerCollision))
	{
		for (auto& Actor : HeldActors)
		{
			// @TODO (Denis): Replace this with the Drop Interface function
			Actor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}

		FingerCollision.Reset();
	}
}

