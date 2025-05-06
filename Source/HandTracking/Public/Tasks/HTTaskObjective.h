// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HTTaskObjective.generated.h"

class UHTResultWriterSubsystem;
class AHTTaskActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveStatusUpdated, UHTTaskObjective*, Objective);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveReadyToBeCompleted, bool, bReady);

USTRUCT(BlueprintType)
struct FObjectiveActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AHTTaskActor> ActorTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FTransform> SpawnTransforms;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUseArrayCountAsInstanceCount = true;

	/** Should rotation be randomized on spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bRandomSpawnRotation = false;
	
	/** This is here so that we can either use a random transform from the array or spawn all the instances from the array */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditConditionHides = true, EditCondition = "bUseArrayCountAsInstanceCount == false"))
	int32 InstanceCount = 0;

	/** @NOTE (Denis): Could be expanded to add things like the FGameplayTags, Materials, and more */ 
};

UENUM(BlueprintType)
enum class EObjectiveState : uint8
{
	WaitingToStart,
	InProgress,
	Completed
};

/**
 * The base class for task objectives
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class HANDTRACKING_API UHTTaskObjective : public UObject
{
	GENERATED_BODY()
	
public:
	/**
	 * Actives the objective, starts writing a test task on the result writer subsystem,
	 * updates the state, spawns the task actors and broadcast the started event
	 * @param InWorldContextManual The object to get the world from
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Objective")
	void Activate(UObject* InWorldContextManual);

	/**
	 * Completes the objective, updating the state, ending writing test results,
	 * and broadcast an event, CleanUp MUST be called manually
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Objective")
	void Complete();

	/** Gets the current objective state */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Objective")
	EObjectiveState GetObjectiveState() const;

	/** Gets the current tracked data */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Objective")
	const FString& GetTrackedData() const;

	/** Checks if the objective is complete */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Objective")
	bool IsComplete() const;

	/** Checks if the objective is in progress */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Objective")
	bool IsInProgress() const;

	/** Checks if the objective is waiting to start, I.E, not complete and not in progress */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Objective")
	bool IsWaitingToStart() const;

	/** Cleans up after itself, by destroying spawned actors */
	UFUNCTION(BlueprintCallable, Category = "Objective")
	virtual void CleanUp();

	UPROPERTY(BlueprintAssignable, Category = "Objective")
	FOnObjectiveStatusUpdated OnObjectiveStarted;

	UPROPERTY(BlueprintAssignable, Category = "Objective")
	FOnObjectiveStatusUpdated OnObjectiveCompleted;

	FOnObjectiveReadyToBeCompleted OnObjectiveReadyToBeCompleted;
	
	static void SetMaterialColor(const AHTTaskActor* TaskActor, const FVector& MaterialColor);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
protected:

	/** Gets all actors that have a matching tag to any of the task actor tags */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Objective")
	TArray<AHTTaskActor*> GatherTaskActors() const;

	/** Spawns the task actors */
	UFUNCTION(BlueprintCallable, Category = "Objective")
	void SpawnTaskActors();

	/** Finds an unused index from an array */
	static int32 GetUnusedIndex(TArray<int32>& UsedIndices, int32 MaxNumber);

protected:
	
	UHTResultWriterSubsystem* GetResultWriterSubsystem() const;
	
	virtual void SendResultsToSubsystem();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	FGameplayTagContainer TaskActorTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	TArray<FObjectiveActor> ObjectiveActors;

	/** Whether or not to complete the objective, once all requirement have been met, or require user to complete it */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	bool bAutoComplete = true;

	UPROPERTY()
	TArray<AHTTaskActor*> SpawnedActors;

	UPROPERTY()
	FString TrackedData = FString();
	
	EObjectiveState State = EObjectiveState::WaitingToStart;

	/** Manually set world context object */
	UPROPERTY(BlueprintReadOnly, Category = "Objective")
	UObject* WorldContextManual = nullptr;
};
