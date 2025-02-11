// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/Objectives/Sortation.h"

#include "Actor/HTTaskActor.h"
#include "Async/CAsyncAction_TweenValue.h"

void USortation::Activate_Implementation(UObject* InWorldContextManual)
{
	Super::Activate_Implementation(InWorldContextManual);

	int32 CountToHalf = 0;
	const int32 HalfObjects = NumberOfSortables / 2;

	UWorld* World = WorldContextManual->GetWorld();
	for (int32 Idx = 0; Idx < NumberOfSortables - 1; ++Idx)
	{
		AHTTaskActor* SpawnedActor = World->SpawnActorDeferred<AHTTaskActor>(ObjectiveActors[2].ActorTemplate,
			ObjectiveActors[2].SpawnTransforms[0], nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
		SpawnedActor->FinishSpawning(ObjectiveActors[2].SpawnTransforms[0]);
		SpawnedActors.Add(SpawnedActor);
	}

	TArray<FVector> SortableLocations = GetLocationsInCircleAroundCenter();
	
	for (auto TaskActor : SpawnedActors)
	{
		/** Is a target */
		if (TargetTags.HasTagExact(TaskActor->TaskActorTag))
		{
			TargetActors.Add(TaskActor);
		}
		else /** Is not a target */
		{
			const int32 Index = CountToHalf <= HalfObjects ? 0 : 1;
			CountToHalf += CountToHalf <= HalfObjects ? 1 : 0;
			
			TaskActor->SetActorLocation(SortableLocations[LastIndex]);
			TaskActor->TaskActorTag = SortableActors[Index].SortableTag;
			TaskActor->OptionalTag = SortableActors[Index].TargetTag;
			SetActorMaterial(TaskActor, SortableActors[Index].Material);
			TaskActor->OnDropped.AddDynamic(this, &ThisClass::OnSortableDropped);
			
			++LastIndex;
		}
	}
}

void USortation::Complete_Implementation()
{
	if (ObjectsSortedCorrectly + ObjectsSortedIncorrectly < NumberOfSortables)
	{
		// Do not allow user to continue, unless all balls have been sorted
		return;
	}
	
	Super::Complete_Implementation();
}

void USortation::SetActorMaterial(AHTTaskActor* TaskActor, UMaterialInterface* Material)
{
	UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(TaskActor->GetRootComponent());
	PrimitiveComponent->SetMaterial(0, Material);
}

TArray<FVector> USortation::GetLocationsInCircleAroundCenter()
{
	const float AngleDelta = 360 / NumberOfSortables;
	const FVector Center = ObjectiveActors[2].SpawnTransforms[0].GetLocation();
	TArray<FVector> Locations;

	FVector Radius = FVector(DistanceFromCenter, 0.0f, 0.0f);
	
	for (int32 Idx = 1; Idx <= NumberOfSortables; ++Idx)
	{
		const FVector Direction = Radius.RotateAngleAxis(AngleDelta * Idx, FVector::UpVector);
		Locations.Add(Center + (Direction));
	}

	// Shuffle the array
	Algo::Sort(Locations, [this](const FVector& A, const FVector& B)
	{
		return FMath::RandBool();
	});
	
	return Locations;
}

void USortation::ScaleDownActor(AActor* TargetActor, AActor* Optional, float CurrentValue)
{
	TargetActor->SetActorLocation(FMath::Lerp(TargetActor->GetActorLocation(), Optional->GetActorLocation(), CurrentValue));
	TargetActor->SetActorScale3D(FMath::Lerp(FVector(0.1f, 0.1f, 0.1f), FVector(0.f, 0.f, 0.f), CurrentValue));
}

void USortation::OnTweenFinished(AActor* TargetActor)
{
	TargetActor->Destroy();
}

void USortation::OnSortableDropped(AHTTaskActor* TaskActor)
{
	const FVector TaskActorLocation = TaskActor->GetActorLocation();
	float ClosestTarget = 50000000.f;
	AHTTaskActor* ClosestTargetActor = nullptr;
	
	for (auto Target : TargetActors)
	{
		const FVector TargetLocation = Target->GetActorLocation();
		const float CurrentDistance = FVector::Dist(TaskActorLocation, TargetLocation);
		if (CurrentDistance < ClosestTarget)
		{
			ClosestTargetActor = Target;
			ClosestTarget = CurrentDistance;
		}
	}

	if (IsValid(ClosestTargetActor) && ClosestTarget < SortationAcceptanceRange)
	{
		if (ClosestTargetActor->TaskActorTag.MatchesTagExact(ClosestTargetActor->OptionalTag))
		{
			++ObjectsSortedCorrectly;
		}
		else
		{
			++ObjectsSortedIncorrectly;
		}

		TaskActor->SetCanBeGrabbed(false);
		UCAsyncAction_TweenValue* TweenValue = UCAsyncAction_TweenValue::Execute(WorldContextManual, TaskActor,
			ClosestTargetActor, 0.f, 1.f, 1.f);
		
		TweenValue->OnTweenUpdate.AddDynamic(this, &ThisClass::ScaleDownActor);
		TweenValue->OnTweenFinished.AddDynamic(this, &ThisClass::OnTweenFinished);
	}
}
