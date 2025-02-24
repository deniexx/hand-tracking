// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/Objectives/CubeStacking.h"

#include "PoseableMeshComponent.h"
#include "Actor/HTTaskActor.h"
#include "HandTracking/HandTracking.h"
#include "Kismet/GameplayStatics.h"

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

	if (bEnablesPhysicsOnHands)
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
		}
	}
}

void UCubeStacking::Complete_Implementation()
{
	if (CubesStackedCorrectly + CubesStackedIncorrectly < SpawnedActors.Num() - 1)
	{
		return;
	}

	UE_LOG(LogHandTracking, Display, TEXT("UCubeStacking::Complete_Implementation"));
	
	TrackedData.Empty();
	TrackedData = FString("CubesStackedCorrectly:,") + FString::FromInt(CubesStackedCorrectly) + LINE_TERMINATOR;
	TrackedData += FString("CubesStackedIncorrectly:,") + FString::FromInt(CubesStackedIncorrectly) + LINE_TERMINATOR;
	
	Super::Complete_Implementation();
}

void UCubeStacking::OnObjectiveActorDropped(AHTTaskActor* TaskActor)
{
	const UWorld* World = WorldContextManual->GetWorld();

	if (bSnapToGround)
	{
		FHitResult Hit;
		const FVector Start = TaskActor->GetActorLocation();
		const FVector End = Start - (FVector::UpVector * 10.f);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(TaskActor);
		if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
		{
			if (AHTTaskActor* HitActor = Cast<AHTTaskActor>(Hit.GetActor()))
			{
				FVector Origin, BoxExtent;
				TaskActor->GetActorBounds(false, Origin, BoxExtent);
				
				FVector TargetLocation = Hit.ImpactPoint;
				TargetLocation.Z += BoxExtent.Z;
				TaskActor->SetActorLocation(TargetLocation);
			}
		}
	}
	
	TArray<AHTTaskActor*> Actors = GatherTaskActors();

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
		if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
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
			}
		}
	}
}
