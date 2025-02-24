// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HTTask.generated.h"

class UHTResultWriterSubsystem;
class UHTTaskObjective;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTaskCompleted, UHTTask*, Task);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTaskObjectiveCompleted, UHTTaskObjective*, Objective, int32, ObjectiveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTaskObjectiveReadyToBeCompleted, bool, bReady);

/**
 *
 */
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class HANDTRACKING_API UHTTask : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HTTask", meta = (DefaultToSelf = "WorldContext"))
	void StartTask(UObject* WorldContext, FString Configuration);

	UFUNCTION(BlueprintCallable, Category = "Task", meta = (DefaultToSelf = "WorldContext"))
	void BeginNextObjective(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "Task", meta = (DefaultToSelf = "WorldContext"))
	void CompleteOrBeginNextObjective(UObject* WorldContext);
	
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
