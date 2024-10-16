// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/HTHandsPawn.h"

#include "OculusXRHandComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "SphereComponent.h"
#include "Camera/CameraComponent.h"

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

	LeftThumbTipSphere = CreateDefaultSubobject<USphereComponent>("LeftThumbTipSphere");
	LeftIndexTipSphere = CreateDefaultSubobject<USphereComponent>("LeftIndexTipSphere");
	LeftMiddleTipSphere = CreateDefaultSubobject<USphereComponent>("LeftMiddleTipSphere");
	LeftRingTipSphere = CreateDefaultSubobject<USphereComponent>("LeftRingTipSphere");
	LeftPinkyTipSphere = CreateDefaultSubobject<USphereComponent>("LeftPinkyTipSphere");

	RightThumbTipSphere = CreateDefaultSubobject<USphereComponent>("RightThumbTipSphere");
	RightIndexTipSphere = CreateDefaultSubobject<USphereComponent>("RightIndexTipSphere");
	RightMiddleTipSphere = CreateDefaultSubobject<USphereComponent>("RightMiddleTipSphere");
	RightRingTipSphere = CreateDefaultSubobject<USphereComponent>("RightRingTipSphere");
	RightPinkyTipSphere = CreateDefaultSubobject<USphereComponent>("RightPinkyTipSphere");
	
	LeftThumbTipSphere->SetupAttachment(OculusXRHandLeft, "Thumb Tip");
	LeftIndexTipSphere->SetupAttachment(OculusXRHandLeft, "Index Tip");
	LeftMiddleTipSphere->SetupAttachment(OculusXRHandLeft, "Middle Tip");
	LeftRingTipSphere->SetupAttachment(OculusXRHandLeft, "Ring Tip");
	LeftPinkyTipSphere->SetupAttachment(OculusXRHandLeft, "Pinky Tip");
	
	
	RightThumbTipSphere->SetupAttachment(OculusXRHandRight, "Thumb Tip");
	RightIndexTipSphere->SetupAttachment(OculusXRHandRight, "Index Tip");
	RightMiddleTipSphere->SetupAttachment(OculusXRHandRight, "Middle Tip");
	RightRingTipSphere->SetupAttachment(OculusXRHandRight, "Ring Tip");
	RightPinkyTipSphere->SetupAttachment(OculusXRHandRight, "Pinky Tip");

	LeftHandSpheres =
	{
		LeftThumbTipSphere,
		LeftIndexTipSphere,
		LeftMiddleTipSphere,
		LeftRingTipSphere,
		LeftPinkyTipSphere
	};

	RightHandSpheres =
	{
		RightThumbTipSphere,
		RightIndexTipSphere,
		RightMiddleTipSphere,
		RightRingTipSphere,
		RightPinkyTipSphere
	};

	LeftThumbTipSphere->SetSphereRadius(2.f);
	LeftIndexTipSphere->SetSphereRadius(2.f);
	LeftMiddleTipSphere->SetSphereRadius(2.f);
	LeftRingTipSphere->SetSphereRadius(2.f);
	LeftPinkyTipSphere->SetSphereRadius(2.f);
	
	RightThumbTipSphere->SetSphereRadius(2.f);
	RightIndexTipSphere->SetSphereRadius(2.f);
	RightMiddleTipSphere->SetSphereRadius(2.f);
	RightRingTipSphere->SetSphereRadius(2.f);
	RightPinkyTipSphere->SetSphereRadius(2.f);
}

// Called when the game starts or when spawned
void AHTHandsPawn::BeginPlay()
{
	Super::BeginPlay();
	
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Stage);

	for (auto Sphere : RightHandSpheres)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnRightHandSphereOverlapBegin);
		Sphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnRightHandSphereOverlapEnd);
	}

	for (auto Sphere : LeftHandSpheres)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnLeftHandSphereOverlapBegin);
		Sphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnLeftHandSphereOverlapEnd);
	}
}

// Called every frame
void AHTHandsPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHTHandsPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHTHandsPawn::OnRightHandSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this) return;
	if (FHandSphereList* Result = ActorsToPointOfContacts.Find(OtherActor))
	{
		if (Result->Spheres.Find(Cast<USphereComponent>(OverlappedComponent)) != INDEX_NONE)
		{
			// Sphere is already there idk how we got here
		}
		else
		{
			Result->Spheres.Add(Cast<USphereComponent>(OverlappedComponent));
		}

		if (Result->Spheres.Num() > 3)
		{
			OtherActor->AttachToComponent(MotionControllerRight, FAttachmentTransformRules::KeepWorldTransform);
		}
	}
	else
	{
		FHandSphereList List;
		List.Spheres.Add(Cast<USphereComponent>(OverlappedComponent));
		ActorsToPointOfContacts.Add(OtherActor, List);
	}
}

void AHTHandsPawn::OnRightHandSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	if (OtherActor == this) return;
	if (FHandSphereList* Result = ActorsToPointOfContacts.Find(OtherActor))
	{
		if (Result->Spheres.Remove(Cast<USphereComponent>(OverlappedComponent)) != INDEX_NONE)
		{
			if (Result->Spheres.Num() < 3)
			{
				OtherActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				for (auto Component : OtherActor->GetComponents())
				{
					if (UPrimitiveComponent* Comp = Cast<UPrimitiveComponent>(Component))
					{
						//Comp->SetSimulatePhysics(true);
					}
				}
			}
		}
	}
}

void AHTHandsPawn::OnLeftHandSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this) return;
	if (FHandSphereList* Result = ActorsToPointOfContacts.Find(OtherActor))
	{
		if (Result->Spheres.Find(Cast<USphereComponent>(OverlappedComponent)) != INDEX_NONE)
		{
			// Sphere is already there idk how we got here
		}
		else
		{
			Result->Spheres.Add(Cast<USphereComponent>(OverlappedComponent));
		}

		if (Result->Spheres.Num() > 3)
		{
			OtherActor->AttachToComponent(MotionControllerLeft, FAttachmentTransformRules::KeepWorldTransform);
		}
	}
	else
	{
		FHandSphereList List;
		List.Spheres.Add(Cast<USphereComponent>(OverlappedComponent));
		ActorsToPointOfContacts.Add(OtherActor, List);
	}
}

void AHTHandsPawn::OnLeftHandSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	if (OtherActor == this) return;
	if (FHandSphereList* Result = ActorsToPointOfContacts.Find(OtherActor))
	{
		if (Result->Spheres.Remove(Cast<USphereComponent>(OverlappedComponent)) != INDEX_NONE)
		{
			if (Result->Spheres.Num() < 3)
			{
				OtherActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				for (auto Component : OtherActor->GetComponents())
				{
					if (UPrimitiveComponent* Comp = Cast<UPrimitiveComponent>(Component))
					{
						//Comp->SetSimulatePhysics(true);
					}
				}
			}
		}
	}
}
