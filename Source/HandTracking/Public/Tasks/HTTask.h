// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTTask.generated.h"

class UHTResultWriterSubsystem;
class UHTTaskObjective;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTaskCompleted, UHTTask*, Task);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTaskObjectiveCompleted, UHTTaskObjective*, Objective, int32, ObjectiveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTaskObjectiveReadyToBeCompleted, bool, bReady);

/**
 * A task featuring a simple task, ideally this should handle any task
 */
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class HANDTRACKING_API UHTTask : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Starts the task, tell the result writing subsystem that a task has been started with a given input configuration
	 *
	 * NOTE: Begin Next Objective must be called manually
	 * @param WorldContext The object to get the world from
	 * @param Configuration The input configuration to be used
	 */
	UFUNCTION(BlueprintCallable, Category = "HTTask", meta = (DefaultToSelf = "WorldContext"))
	void StartTask(UObject* WorldContext, FString Configuration);

	/**
	 * Starts the next objective in the task
	 * @param WorldContext 
	 */
	UFUNCTION(BlueprintCallable, Category = "Task", meta = (DefaultToSelf = "WorldContext"))
	void BeginNextObjective(UObject* WorldContext);

	/**
	 * If an objective is not active, it will start it, otherwise it will attempt to complete the currently active objective
	 * @param WorldContext The object to get the world from
	 */
	UFUNCTION(BlueprintCallable, Category = "Task", meta = (DefaultToSelf = "WorldContext"))
	void CompleteOrBeginNextObjective(UObject* WorldContext);

	/**
	 * Completes the current objective, if that is possible
	 */
	UFUNCTION(BlueprintCallable, Category = "Task")
	void CompleteCurrentObjective();
	
	UPROPERTY(BlueprintAssignable, Category = "Task")
	FOnTaskCompleted OnCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Task")
	FOnTaskObjectiveCompleted OnTaskObjectiveCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Task")
	FOnTaskObjectiveCompleted OnObjectiveStarted;

	UPROPERTY(BlueprintAssignable, Category = "Task")
	FOnTaskObjectiveReadyToBeCompleted OnObjectiveReadyToBeCompleted;
	
protected:

	UFUNCTION(BlueprintCallable)
	void OnObjectiveReadyToBeCompletedFunc(bool bReady);

	UHTResultWriterSubsystem* GetResultWriterSubsystem() const;
	
	UPROPERTY()
	UObject* WorldContextManual;

	UPROPERTY(EditAnywhere, Category = "Task")
	TArray<UHTTaskObjective*> Objectives;

	UPROPERTY(EditAnywhere, Category = "Task")
	int32 CurrentObjective;
	
private:

	UFUNCTION()
	void OnObjectiveCompleted(UHTTaskObjective* Objective);
};
