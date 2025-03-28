// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/Objectives/CubeStacking.h"

#include "PoseableMeshComponent.h"
#include "SphereComponent.h"
#include "Actor/HTTaskActor.h"
#include "Kismet/GameplayStatics.h"
#include "System/HTBlueprintFunctionLibrary.h"

void UCubeStacking::Activate_Implementation(UObject* InWorldContextManual)
{
	Super::Activate_Implementation(InWorldContextManual);

	TArray<AHTTaskActor*> Actors = GatherTaskActors();

	for (AHTTaskActor* Actor : Actors)
	{
		Actor->OnDropped.AddDynamic(this, &ThisClass::OnObjectiveActorDropped);
		if (bEnablesPhysicsOnCubes)
		{
			Cast<UPrimitiveComponent>(Actor->GetRootComponent())->SetSimulatePhysics(true);
		}
	}

	if (bEnablesPhysicsOnHands && !UHTBlueprintFunctionLibrary::IsInMotionControllerConfig(WorldContextManual))
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(WorldContextManual, 0);
		if (PlayerPawn != nullptr)
		{
			TArray<UPoseableMeshComponent*> PoseableMeshComponents;
			PlayerPawn->GetComponents<UPoseableMeshComponent>(PoseableMeshComponents);

			for (auto PoseableMeshComponent : PoseableMeshComponents)
			{
				PoseableMeshComponent->SetCollisionProfileName("PhysicsActor");
				PoseableMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			}

			TArray<USphereComponent*> SphereComponents;
			PlayerPawn->GetComponents<USphereComponent>(SphereComponents);

			for (auto SphereComponent : SphereComponents)
			{
				if (SphereComponent->GetFName().ToString().Contains("Fist"))
				{
					SphereComponent->SetCollisionProfileName("BlockAll");
					SphereComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
				}
			}
		}
	}
}

void UCubeStacking::Complete_Implementation()
{
	if (CubesStackedCorrectly + CubesStackedIncorrectly < SpawnedActors.Num())
	{
		return;
	}
	
	TrackedData.Empty();

	TrackedData = FString::Printf(TEXT("SnapToGround: %s"), bSnapToGround ? TEXT("true") : TEXT("false")) + LINE_TERMINATOR;
	TrackedData += FString::Printf(TEXT("CubePhysics: %s"), bEnablesPhysicsOnCubes ? TEXT("true") : TEXT("false")) + LINE_TERMINATOR;
	TrackedData += FString::Printf(TEXT("HandPhysics: %s"), bEnablesPhysicsOnHands ? TEXT("true") : TEXT("false")) + LINE_TERMINATOR;
	TrackedData += FString::Printf(TEXT("CubesStackedCorrectly:,%d"), CubesStackedCorrectly) + LINE_TERMINATOR;
	TrackedData += FString::Printf(TEXT("CubesStackedIncorrectly:,%d"), CubesStackedIncorrectly) + LINE_TERMINATOR;
	TrackedData += FString::Printf(TEXT("AverageXYDelta:,%.2f"), AverageXYDelta) + LINE_TERMINATOR;

	if (bEnablesPhysicsOnHands)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(WorldContextManual, 0);
		if (PlayerPawn != nullptr)
		{
			TArray<UPoseableMeshComponent*> PoseableMeshComponents;
			PlayerPawn->GetComponents<UPoseableMeshComponent>(PoseableMeshComponents);

			for (auto PoseableMeshComponent : PoseableMeshComponents)
			{
				PoseableMeshComponent->SetCollisionProfileName("NoCollision");
				PoseableMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}

			TArray<USphereComponent*> SphereComponents;
			PlayerPawn->GetComponents<USphereComponent>(SphereComponents);

			for (auto SphereComponent : SphereComponents)
			{
				if (SphereComponent->GetFName().ToString().Contains("Fist"))
				{
					SphereComponent->SetCollisionProfileName("NoCollision");
					SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				}
			}
		}
	}
	
	Super::Complete_Implementation();
}

void UCubeStacking::OnObjectiveActorDropped(AHTTaskActor* TaskActor)
{
	const UWorld* World = WorldContextManual->GetWorld();

	FVector Origin, BoxExtent;
	TaskActor->GetActorBounds(false, Origin, BoxExtent);
	FCollisionShape BoxShape = FCollisionShape::MakeBox(BoxExtent / 2.f);
	
	if (bSnapToGround)
	{
		FHitResult Hit;
		const FVector Start = TaskActor->GetActorLocation();
		const FVector End = Start - (FVector::UpVector * 10.f);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(TaskActor);
		if (World->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Visibility, BoxShape, Params))
		{
			if (AHTTaskActor* HitActor = Cast<AHTTaskActor>(Hit.GetActor()))
			{
				FVector TargetLocation = Start;
				TargetLocation.Z = Hit.ImpactPoint.Z;
				TargetLocation.Z += BoxExtent.Z;
				TaskActor->SetActorLocation(TargetLocation);
			}
		}
	}
	
	TArray<AHTTaskActor*> Actors = GatherTaskActors();

	int32 Samples = 0;
	float TotalDelta = 0.f;
	CubesStackedCorrectly = 0;
	CubesStackedIncorrectly = 0;
	for (AHTTaskActor* Actor : Actors)
	{
		float ZMultiplier = 1.f;
		if (Actor->OptionalTag == FGameplayTag::EmptyTag)
		{
			ZMultiplier = -1.f;
		}

		const FVector Start = Actor->GetActorLocation();
		const FVector End = Actor->GetActorLocation() + ((FVector::UpVector * 10.f) * ZMultiplier);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Actor);
		FHitResult Hit;
		if (World->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Visibility, BoxShape, Params))
		{
			AHTTaskActor* HitActor = Cast<AHTTaskActor>(Hit.GetActor());
			if (HitActor)
			{
				const FGameplayTag& TagToCheck = ZMultiplier > 0.f ? HitActor->TaskActorTag : HitActor->OptionalTag;
				const FGameplayTag& TagToCheckWith = ZMultiplier > 0.f ? Actor->OptionalTag : Actor->TaskActorTag;
				/** Have we correctly sorted the cube */
				if (TagToCheck.MatchesTagExact(TagToCheckWith))
				{
					++CubesStackedCorrectly;
				}
				else
				{
					++CubesStackedIncorrectly;
				}

				++Samples;
				TotalDelta += FVector::Dist2D(Actor->GetActorLocation(), HitActor->GetActorLocation());; 
			}
		}
	}

	AverageXYDelta = TotalDelta / Samples;
	const bool bCanBeCompleted = CubesStackedCorrectly + CubesStackedIncorrectly == SpawnedActors.Num();
	OnObjectiveReadyToBeCompleted.Broadcast(bCanBeCompleted);
}
