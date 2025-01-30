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

	/** @NOTE (Denis): This introduces left hand bias, which has some issues described below:
	 * If you are holding an object with the right hand and then try to take it with the left, that will work
	 * But if you are holding an object with the left and try to take it with the right, it will fail, as the Left hand is checked after the right
	 * Either we flip this around, depending on the dominant hand of the user, or introduce a way to track previously grabbed objects, that way we can
	 * filter for those and update grabbing properly, which is not ideal, maybe adding a small delay would help, but unsure on how to fix this issue currently
	 * that is consistent and feels good, other than switching over to pinching gesturees to grab objects, or multiple different gestures to grab objects
	 */
	TraceAndGrabFromHand(OculusXRHandRight, MotionControllerRight, FingerCollisionRight, HeldActorsRightHand);
	TraceAndGrabFromHand(OculusXRHandLeft, MotionControllerLeft, FingerCollisionLeft, HeldActorsLeftHand);

	UHTBlueprintFunctionLibrary::LowPassFilter_RollingAverage(RightControllerVelocityRunningAverage,
		MotionControllerRight->GetComponentVelocity(), RightControllerVelocityRunningAverage, VelocitySamples);

	UHTBlueprintFunctionLibrary::LowPassFilter_RollingAverage(LeftControllerVelocityRunningAverage,
		MotionControllerLeft->GetComponentVelocity(), LeftControllerVelocityRunningAverage, VelocitySamples);
}

void AHTHandsPawn::TraceAndGrabFromHand(UOculusXRHandComponent* HandComponent,
	UMotionControllerComponent* ControllerComponent, FFingerCollisionData& FingerCollision, TArray<AActor*>& HeldActors)
{
	TArray<AActor*> IgnoredActors = { this };
	const bool bDrawDebug = GDebugFingerTrace > 0;

	if (HandComponent->bSkeletalMeshInitialized)
	{
		FingerCollision.Reset();
		FHitResult HitResult;
		if (TraceFinger(HandComponent, "Thumb Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			TryGrabItem(HandComponent, ControllerComponent, FingerCollision, ETargetHandLocation::Thumb, HitResult, HeldActors);
			DoFeedback(ETargetHand::Right, ETargetHandLocation::Thumb, 0.5f);
		}
		else
		{
			TryReleaseItem(HandComponent, ControllerComponent, FingerCollision, HeldActors);
		}

		if (TraceFinger(HandComponent, "Index Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			TryGrabItem(HandComponent, ControllerComponent, FingerCollision, ETargetHandLocation::Index, HitResult, HeldActors);
			DoFeedback(ETargetHand::Right, ETargetHandLocation::Index, 0.5f);
		}
		else
		{
			TryReleaseItem(HandComponent, ControllerComponent, FingerCollision, HeldActors);
		}

		if (TraceFinger(HandComponent, "Middle Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			TryGrabItem(HandComponent, ControllerComponent, FingerCollision, ETargetHandLocation::Middle, HitResult, HeldActors);
			DoFeedback(ETargetHand::Right, ETargetHandLocation::Middle, 0.5f);
		}
		else
		{
			TryReleaseItem(HandComponent, ControllerComponent, FingerCollision, HeldActors);
		}

		if (TraceFinger(HandComponent, "Ring Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			TryGrabItem(HandComponent, ControllerComponent, FingerCollision, ETargetHandLocation::Ring, HitResult, HeldActors);
			DoFeedback(ETargetHand::Right, ETargetHandLocation::Ring, 0.5f);
		}
		else
		{
			TryReleaseItem(HandComponent, ControllerComponent, FingerCollision, HeldActors);
		}

		if (TraceFinger(HandComponent, "Pinky Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			TryGrabItem(HandComponent, ControllerComponent, FingerCollision, ETargetHandLocation::Pinky, HitResult, HeldActors);
			DoFeedback(ETargetHand::Right, ETargetHandLocation::Pinky, 0.5f);
		}
		else
		{
			TryReleaseItem(HandComponent, ControllerComponent, FingerCollision, HeldActors);
		}
	}
}

// Called to bind functionality to input
void AHTHandsPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
}

void AHTHandsPawn::GetControllerVelocitiesAveraged(FVector& OutLeftControllerVelocity,
	FVector& OutRightControllerVelocity)
{
	OutLeftControllerVelocity = LeftControllerVelocityRunningAverage;
	OutRightControllerVelocity = RightControllerVelocityRunningAverage;
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

void AHTHandsPawn::TryGrabItem(UOculusXRHandComponent* HandComponent, UMotionControllerComponent* ControllerComponent,
                               FFingerCollisionData& FingerCollision, ETargetHandLocation Location, const FHitResult& HitResult, TArray<AActor*>& HeldActors)
{
	AActor* HitActor = HitResult.GetActor();
	if (HitActor == nullptr)
	{
		return;
	}

	/** @NOTE (Denis): This might need to be looked into, as currently we are just overriding the hit actor,
	 * which could cause issues, by overriding the hit actor, probably a better solution would be store multiple FingerCollisionDatas
	 * and use that to determine which actor should/should not be grabbed
	 */
	FingerCollision.HitActor = HitActor;
	SET_BIT(FingerCollision.FingersColliding, Location);
	
	if (AreGrabRequirementsMet(FingerCollision))
	{
		/** @TODO (Denis): Check if the other hand is already grabbing the object and if it is, remove it from the array */
		/** @NOTE (Denis): Currently fixed by a hack... */
		TArray<AActor*> OtherHeldActors = HandComponent->MeshType == EOculusXRHandType::HandRight ? HeldActorsRightHand : HeldActorsLeftHand;
		if (OtherHeldActors.Num() > 0)
		{
			const int32 OtherHeldActor = OtherHeldActors.Find(FingerCollision.HitActor);
			if (OtherHeldActor != INDEX_NONE)
			{
				OtherHeldActors[OtherHeldActor]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				OtherHeldActors.RemoveAt(OtherHeldActor);
			}
		}
		
		IHTGrabbable::Execute_Grab(HitResult.GetActor(), this, ControllerComponent);
		HitResult.GetActor()->AttachToComponent(HandComponent, FAttachmentTransformRules::KeepWorldTransform);
		HeldActors.Add(HitResult.GetActor());
	}
}

void AHTHandsPawn::TryReleaseItem(UOculusXRHandComponent* HandComponent,  UMotionControllerComponent* ControllerComponent,
	FFingerCollisionData& FingerCollision, TArray<AActor*>& HeldActors)
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
		HeldActors.Empty();
	}
}

