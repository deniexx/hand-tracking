// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/HTHandsPawn.h"

#include <bit>

#include "HandGestureRecognizer.h"
#include "OculusXRHandComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "HandInput/CameraHandInput.h"
#include "HandTracking/HandTracking.h"
#include "Interfaces/HTGrabbable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "System/HTBlueprintFunctionLibrary.h"

int32 GDebugFingerTrace = 0;
static FAutoConsoleVariableRef CVarShowDebugFingerTrace(
	TEXT("ShowDebug.FingerTrace"),
	GDebugFingerTrace,
	TEXT("Draws debug info for Finger Traces")
	TEXT("	0 - Do not draw debug info")
	TEXT("	1 - Draw debug info"),
	ECVF_Cheat
);

int32 GProcessHandLogInput = 0;
static FAutoConsoleVariableRef CVarProcessHandLogInput(
	TEXT("Debug.ProcessHandLog"),
	GProcessHandLogInput,
	TEXT("Processes the input for logging hand poses")
	TEXT("	0 - Do not process")
	TEXT("	1 - Process"),
	ECVF_Cheat
);

// Sets default values
ADEPRECATED_HTHandsPawn::ADEPRECATED_HTHandsPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(GetRootComponent());

	MotionControllerLeft = CreateDefaultSubobject<UMotionControllerComponent>("MotionControllerLeft");
	MotionControllerRight = CreateDefaultSubobject<UMotionControllerComponent>("MotionControllerRight");

	OculusXRHandLeft = CreateDefaultSubobject<UOculusXRHandComponent>("OculusXRHandLeft");
	OculusXRHandRight = CreateDefaultSubobject<UOculusXRHandComponent>("OculusXRHandRight");

	HandPoseRecognizerLeft = CreateDefaultSubobject<UHandPoseRecognizer>("HandPoseRecognizerLeft");
	HandPoseRecognizerRight = CreateDefaultSubobject<UHandPoseRecognizer>("HandPoseRecognizerRight");

	HandGestureRecognizerLeft = CreateDefaultSubobject<UHandGestureRecognizer>("HandGestureRecognizerLeft");
	HandGestureRecognizerRight = CreateDefaultSubobject<UHandGestureRecognizer>("HandGestureRecognizerRight");

	CameraHandInputLeft = CreateDefaultSubobject<UCameraHandInput>("CameraHandInputLeft");
	CameraHandInputRight = CreateDefaultSubobject<UCameraHandInput>("CameraHandInputRight");

	MotionControllerLeft->SetupAttachment(GetRootComponent());
	MotionControllerRight->SetupAttachment(GetRootComponent());

	CameraHandInputLeft->SetHand(EControllerHand::Left);
	CameraHandInputRight->SetHand(EControllerHand::Right);
	
	OculusXRHandLeft->SetupAttachment(MotionControllerLeft);
	OculusXRHandRight->SetupAttachment(MotionControllerRight);

	MotionControllerLeft->SetTrackingMotionSource("Left");
	MotionControllerRight->SetTrackingMotionSource("Right");

	HandPoseRecognizerLeft->Side = EOculusXRHandType::HandLeft;
	HandPoseRecognizerRight->Side = EOculusXRHandType::HandRight;
	
	HandPoseRecognizerLeft->SetupAttachment(MotionControllerLeft);
	HandPoseRecognizerRight->SetupAttachment(MotionControllerRight);

	HandGestureRecognizerLeft->SetupAttachment(HandPoseRecognizerLeft);
	HandGestureRecognizerRight->SetupAttachment(HandPoseRecognizerRight);
	
	OculusXRHandLeft->SkeletonType = EOculusXRHandType::HandLeft;
	OculusXRHandRight->SkeletonType = EOculusXRHandType::HandRight;

	OculusXRHandLeft->MeshType = EOculusXRHandType::HandLeft;
	OculusXRHandRight->MeshType = EOculusXRHandType::HandRight;
}

// Called when the game starts or when spawned
void ADEPRECATED_HTHandsPawn::BeginPlay()
{
	Super::BeginPlay();
	
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Stage);
}

// Called every frame
void ADEPRECATED_HTHandsPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GProcessHandLogInput)
	{
		ProcessHandLogInput();
	}

	/** @NOTE (Denis): This introduces left hand bias, which has some issues described below:
	 * If you are holding an object with the right hand and then try to take it with the left, that will work
	 * But if you are holding an object with the left and try to take it with the right, it will fail, as the Left hand is checked after the right
	 * Either we flip this around, depending on the dominant hand of the user, or introduce a way to track previously grabbed objects, that way we can
	 * filter for those and update grabbing properly, which is not ideal, maybe adding a small delay would help, but unsure on how to fix this issue currently
	 * that is consistent and feels good, other than switching over to pinching gesturees to grab objects, or multiple different gestures to grab objects
	 
	TraceAndGrabFromHand(OculusXRHandRight, MotionControllerRight, FingerCollisionRight, HeldActorsRightHand);
	TraceAndGrabFromHand(OculusXRHandLeft, MotionControllerLeft, FingerCollisionLeft, HeldActorsLeftHand);
	*/

	TryGrabWithGesture(OculusXRHandRight, MotionControllerRight, HeldActorsRightHand);
	TryGrabWithGesture(OculusXRHandLeft, MotionControllerLeft, HeldActorsLeftHand);
	
	UHTBlueprintFunctionLibrary::LowPassFilter_RollingAverage(RightControllerVelocityRunningAverage,
		MotionControllerRight->GetComponentVelocity(), RightControllerVelocityRunningAverage, VelocitySamples);

	UHTBlueprintFunctionLibrary::LowPassFilter_RollingAverage(LeftControllerVelocityRunningAverage,
		MotionControllerLeft->GetComponentVelocity(), LeftControllerVelocityRunningAverage, VelocitySamples);
}

void ADEPRECATED_HTHandsPawn::TraceAndGrabFromHand(UOculusXRHandComponent* HandComponent,
	UMotionControllerComponent* ControllerComponent, FFingerCollisionData& FingerCollision, TArray<AActor*>& HeldActors)
{
	TArray<AActor*> IgnoredActors = { this };
	const bool bDrawDebug = GDebugFingerTrace > 0;
	ETargetHand TargetHand = HandComponent->MeshType == EOculusXRHandType::HandLeft ? ETargetHand::Left : ETargetHand::Right;

	if (HandComponent->bSkeletalMeshInitialized)
	{
		FingerCollision.Reset();
		FHitResult HitResult;
		if (TraceFinger(HandComponent, "Thumb Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			TryGrabItem(HandComponent, ControllerComponent, FingerCollision, ETargetHandLocation::Thumb, HitResult, HeldActors);
			DoFeedback(TargetHand, ETargetHandLocation::Thumb, 0.5f);
		}
		else
		{
			TryReleaseItem(HandComponent, ControllerComponent, FingerCollision, HeldActors);
		}

		if (TraceFinger(HandComponent, "Index Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			TryGrabItem(HandComponent, ControllerComponent, FingerCollision, ETargetHandLocation::Index, HitResult, HeldActors);
			DoFeedback(TargetHand, ETargetHandLocation::Index, 0.5f);
		}
		else
		{
			TryReleaseItem(HandComponent, ControllerComponent, FingerCollision, HeldActors);
		}

		if (TraceFinger(HandComponent, "Middle Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			TryGrabItem(HandComponent, ControllerComponent, FingerCollision, ETargetHandLocation::Middle, HitResult, HeldActors);
			DoFeedback(TargetHand, ETargetHandLocation::Middle, 0.5f);
		}
		else
		{
			TryReleaseItem(HandComponent, ControllerComponent, FingerCollision, HeldActors);
		}

		if (TraceFinger(HandComponent, "Ring Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			TryGrabItem(HandComponent, ControllerComponent, FingerCollision, ETargetHandLocation::Ring, HitResult, HeldActors);
			DoFeedback(TargetHand, ETargetHandLocation::Ring, 0.5f);
		}
		else
		{
			TryReleaseItem(HandComponent, ControllerComponent, FingerCollision, HeldActors);
		}

		if (TraceFinger(HandComponent, "Pinky Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			TryGrabItem(HandComponent, ControllerComponent, FingerCollision, ETargetHandLocation::Pinky, HitResult, HeldActors);
			DoFeedback(TargetHand, ETargetHandLocation::Pinky, 0.5f);
		}
		else
		{
			TryReleaseItem(HandComponent, ControllerComponent, FingerCollision, HeldActors);
		}
	}
}

// Called to bind functionality to input
void ADEPRECATED_HTHandsPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
}

void ADEPRECATED_HTHandsPawn::GetControllerVelocitiesAveraged(FVector& OutLeftControllerVelocity,
	FVector& OutRightControllerVelocity)
{
	OutLeftControllerVelocity = LeftControllerVelocityRunningAverage;
	OutRightControllerVelocity = RightControllerVelocityRunningAverage;
}

bool ADEPRECATED_HTHandsPawn::TraceFinger(UOculusXRHandComponent* HandComponent, FName SocketName, const TArray<AActor*>& IgnoredActors, bool bDrawDebug, FHitResult& OutResult) const
{
	const EDrawDebugTrace::Type DrawDebug = bDrawDebug ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
	const FVector TraceStart = HandComponent->GetSocketLocation(SocketName);
	return UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, TraceStart, TraceSphereRadius, TraceChannels, false, IgnoredActors, DrawDebug, OutResult, true);
}

void ADEPRECATED_HTHandsPawn::DoFeedback(ETargetHand Hand, ETargetHandLocation Location, float Duration) const
{
	FHandFeedbackConfig Config;
	Config.Hand = Hand;
	Config.NormalizedStrength = 1.f;
	Config.Location = Location;
	Config.Duration = Duration;
	UHTBlueprintFunctionLibrary::ApplyHandFeedback(this, Config);
}

bool ADEPRECATED_HTHandsPawn::AreGrabRequirementsMet(const FFingerCollisionData& FingerCollision)
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

void ADEPRECATED_HTHandsPawn::TryGrabItem(UOculusXRHandComponent* HandComponent, UMotionControllerComponent* ControllerComponent,
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

void ADEPRECATED_HTHandsPawn::TryGrabItemFromPose(UMotionControllerComponent* ControllerComponent, UOculusXRHandComponent* HandComponent, TArray<AActor*>& HeldActors)
{
	const bool bDrawDebug = GDebugFingerTrace > 0;
	const EDrawDebugTrace::Type DrawDebug = bDrawDebug ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;

	const FVector TraceStart = HandComponent->GetSocketLocation(FName("Palm"));

	TArray<AActor*> IgnoredActors = { this };
	FHitResult OutResult;
	
	if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, TraceStart, 12,
		TraceChannels, false, IgnoredActors, DrawDebug, OutResult, true))
	{
		AActor* HitActor = OutResult.GetActor();
		if (HitActor != nullptr)
		{
			HitActor->AttachToComponent(HandComponent, FAttachmentTransformRules::KeepWorldTransform);
		}
	}
}

void ADEPRECATED_HTHandsPawn::TryReleaseItem(UOculusXRHandComponent* HandComponent,  UMotionControllerComponent* ControllerComponent,
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

void ADEPRECATED_HTHandsPawn::ProcessHandLogInput()
{
	/** Check if the appropriate key is pressed and log the hand pose in the editor with a label of which hand */
	APlayerController* PlayerController = GetController<APlayerController>();

	if (PlayerController->IsInputKeyDown(LogLeftHandPoseKey))
	{
		UE_LOG(LogHandTracking, Log, TEXT("=================================================================="));
		UE_LOG(LogHandTracking, Log, TEXT("Left Hand Pose:"));
		HandPoseRecognizerLeft->LogEncodedHandPose();
		UE_LOG(LogHandTracking, Log, TEXT("=================================================================="));
	}
	
	if (PlayerController->IsInputKeyDown(LogRightHandPoseKey))
	{
		UE_LOG(LogHandTracking, Log, TEXT("=================================================================="));
		UE_LOG(LogHandTracking, Log, TEXT("Right Hand Pose:"));
		HandPoseRecognizerRight->LogEncodedHandPose();
		UE_LOG(LogHandTracking, Log, TEXT("=================================================================="));
	}
	
}

void ADEPRECATED_HTHandsPawn::TryGrabWithGesture(UOculusXRHandComponent* HandComponent, UMotionControllerComponent* ControllerComponent, TArray<AActor*>& HeldActors)
{
	if (HandComponent->MeshType == EOculusXRHandType::HandRight)
	{
		if (CameraHandInputRight->IsPinching() || CameraHandInputRight->IsInGrabPose())
		{
			TryGrabItemFromPose(ControllerComponent, HandComponent, HeldActors);
		}
	}
	else
	{
		if (CameraHandInputLeft->IsPinching() || CameraHandInputLeft->IsInGrabPose())
		{
			TryGrabItemFromPose(ControllerComponent, HandComponent, HeldActors);
		}		
	}
}
