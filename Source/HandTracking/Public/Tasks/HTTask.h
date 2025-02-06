// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HTTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTaskCompleted, UHTTask*, Task);

class UHTTaskObjective;
/**
 *
 */
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class HANDTRACKING_API UHTTask : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Task")
	TArray<UHTTaskObjective*> Objectives;

	/** Calls begin on all objectives and subscribes to their delegates */
	UFUNCTION(BlueprintCallable, Category = "Task", meta = (DefaultToSelf = "WorldContext"))
	void BeginTask(UObject* WorldContext);

	UPROPERTY(BlueprintAssignable, Category = "Task")
	FOnTaskCompleted OnCompleted;
	
private:

	UFUNCTION()
	void OnObjectiveCompleted(UHTTaskObjective* Objective);
};
