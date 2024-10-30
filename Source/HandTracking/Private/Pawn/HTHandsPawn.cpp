// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/HTHandsPawn.h"

#include "OculusXRHandComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "System/HTBlueprintFunctionLibrary.h"

static TAutoConsoleVariable<int32> CVarShowDebugFingerTrace(
	TEXT("ShowDebug.FingerTrace"),
	1,
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
	const bool bDrawDebug = CVarShowDebugFingerTrace.GetValueOnAnyThread() > 0;
		
	if (OculusXRHandRight->bSkeletalMeshInitialized)
	{
		FHitResult HitResult;
		if (TraceFinger(OculusXRHandRight, "Thumb Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			DoFeedback(ETargetHand::Right, ETargetHandLocation::Thumb, 0.5f);
		}

		if (TraceFinger(OculusXRHandRight, "Index Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			DoFeedback(ETargetHand::Right, ETargetHandLocation::Index, 0.5f);
		}

		if (TraceFinger(OculusXRHandRight, "Middle Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			DoFeedback(ETargetHand::Right, ETargetHandLocation::Middle, 0.5f);
		}

		if (TraceFinger(OculusXRHandRight, "Ring Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			DoFeedback(ETargetHand::Right, ETargetHandLocation::Ring, 0.5f);
		}

		if (TraceFinger(OculusXRHandRight, "Pinky Tip", IgnoredActors, bDrawDebug, HitResult))
		{
			DoFeedback(ETargetHand::Right, ETargetHandLocation::Pinky, 0.5f);
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
	FHitResult HitResult;
	const FVector TraceStart = HandComponent->GetSocketLocation(SocketName);
	return UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, TraceStart, TraceSphereRadius, TraceChannels, false, IgnoredActors, DrawDebug, HitResult, true);
}

void AHTHandsPawn::DoFeedback(ETargetHand Hand, ETargetHandLocation Location, float Duration)
{
	FHandFeedbackConfig Config;
	Config.Hand = Hand;
	Config.NormalizedStrength = 1.f;
	Config.Location = Location;
	Config.Duration = Duration;
	UHTBlueprintFunctionLibrary::ApplyHandFeedback(this, Config);
}
