// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/HTTaskObjective.h"

#include "EngineUtils.h"
#include "Actor/HTTaskActor.h"
#include "System/HTResultWriterSubsystem.h"

void UHTTaskObjective::Activate_Implementation(UObject* InWorldContextManual)
{
	if (!IsWaitingToStart())
	{
		return;
	}

	WorldContextManual = InWorldContextManual;
	TrackedData = FString();
	State = EObjectiveState::InProgress;
	GetResultWriterSubsystem()->BeginWriteTestTask(GetFName().ToString());
	
	SpawnTaskActors();
	OnObjectiveStarted.Broadcast(this);
}

void UHTTaskObjective::Complete_Implementation()
{
	if (!IsInProgress())
	{
		return;
	}

	SendResultsToSubsystem();
	GetResultWriterSubsystem()->EndWriteTestTask();
	State = EObjectiveState::Completed;
	OnObjectiveCompleted.Broadcast(this);
}

EObjectiveState UHTTaskObjective::GetObjectiveState() const
{
	return State;
}

const FString& UHTTaskObjective::GetTrackedData() const
{
	return TrackedData;
}

bool UHTTaskObjective::IsComplete() const
{
	return State == EObjectiveState::Completed;
}

bool UHTTaskObjective::IsInProgress() const
{
	return State == EObjectiveState::InProgress;
}

bool UHTTaskObjective::IsWaitingToStart() const
{
	return State == EObjectiveState::WaitingToStart;
}

void UHTTaskObjective::CleanUp()
{
	for (auto& SpawnedActor : SpawnedActors)
	{
		if (IsValid(SpawnedActor))
		{
			SpawnedActor->Destroy();
		}
	}

	SpawnedActors.Empty();
}

void UHTTaskObjective::SetMaterialColor(const AHTTaskActor* TaskActor, const FVector& MaterialColor)
{
	UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(TaskActor->GetRootComponent());
	UMaterialInstanceDynamic* Material = Cast<UMaterialInstanceDynamic>(PrimitiveComponent->GetMaterial(0));
	Material->SetVectorParameterValue("HologramColor", MaterialColor);
}

#if WITH_EDITOR
void UHTTaskObjective::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(FObjectiveActor, InstanceCount))
	{
		/** Validate all entries */
		for (auto& ObjectiveActor : ObjectiveActors)
		{
			if (!ObjectiveActor.bUseArrayCountAsInstanceCount)
			{
				ObjectiveActor.InstanceCount = FMath::Min(ObjectiveActor.InstanceCount, ObjectiveActor.SpawnTransforms.Num());
			}
		}
	}
}
#endif

TArray<AHTTaskActor*> UHTTaskObjective::GatherTaskActors() const
{
	TArray<AHTTaskActor*> RelativeTaskActors;
	UWorld* World = WorldContextManual->GetWorld();
	
	/** Actually not that slow, as it uses HashBuckets for different actor classes */
	for (TActorIterator<AHTTaskActor> It(World, AHTTaskActor::StaticClass()); It; ++It)
	{
		AHTTaskActor* Actor = *It;
		if (IsValid(Actor) && TaskActorTags.HasTagExact(Actor->TaskActorTag))
		{
			RelativeTaskActors.Add(Actor);
		}
	}

	return RelativeTaskActors;
}

void UHTTaskObjective::SpawnTaskActors()
{
	if (ObjectiveActors.Num() == 0)
	{
		/** No actors to spawn so no need to run anything else */
		return;
	}
	
	UWorld* World = WorldContextManual->GetWorld();
	ensureAlways(World);
	
	for (const auto& ObjectiveActor : ObjectiveActors)
	{
		TArray<int32> UsedIndices;
		const int32 FinalIndex = ObjectiveActor.bUseArrayCountAsInstanceCount ?
			ObjectiveActor.SpawnTransforms.Num() : ObjectiveActor.InstanceCount;
		
		for (int32 Idx = 0; Idx < FinalIndex; ++Idx)
		{
			FTransform SpawnTransform = ObjectiveActor.SpawnTransforms[Idx];

			if (!ObjectiveActor.bUseArrayCountAsInstanceCount)
			{
				int32 UnusedIndex = GetUnusedIndex(UsedIndices, ObjectiveActor.InstanceCount);
				SpawnTransform = ObjectiveActor.SpawnTransforms[UnusedIndex];
			}

			if (ObjectiveActor.bRandomSpawnRotation)
			{
				FVector RandomVector = FMath::VRand();
				SpawnTransform.SetRotation(RandomVector.ToOrientationQuat());
			}
			
			AHTTaskActor* SpawnedActor = World->SpawnActorDeferred<AHTTaskActor>(ObjectiveActor.ActorTemplate,
				SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			SpawnedActor->FinishSpawning(SpawnTransform);
			SpawnedActors.Add(SpawnedActor);
		}
	}
}

int32 UHTTaskObjective::GetUnusedIndex(TArray<int32>& UsedIndices, int32 MaxNumber)
{
	int32 RandomIndex = FMath::RandRange(0, MaxNumber - 1);
	if (UsedIndices.Find(RandomIndex) == INDEX_NONE)
	{
		UsedIndices.Add(RandomIndex);
		return RandomIndex;
	}

	return GetUnusedIndex(UsedIndices, MaxNumber);
}

UHTResultWriterSubsystem* UHTTaskObjective::GetResultWriterSubsystem() const
{
	if (WorldContextManual)
	{
		return WorldContextManual->GetWorld()->GetGameInstance()->GetSubsystem<UHTResultWriterSubsystem>();
	}

	return nullptr;
}

void UHTTaskObjective::SendResultsToSubsystem()
{
	GetResultWriterSubsystem()->WriteTestResult(TrackedData);
}
