// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HTTaskObjective.generated.h"


class AHTTaskActor;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveStatusUpdated, UHTTaskObjective*, Objective);

UENUM(BlueprintType)
enum class EObjectiveState : uint8
{
	WaitingToStart,
	InProgress,
	Completed
};

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class HANDTRACKING_API UHTTaskObjective : public UObject
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintNativeEvent, Category = "HTTaskObjective")
	void Activate(UObject* InWorldContextManual);

	UFUNCTION(BlueprintCallable, Category = "HTTaskObjective")
	void Complete();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HTTaskObjective")
	EObjectiveState GetObjectiveState() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HTTaskObjective")
	const FString& GetTrackedData() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HTTaskObjective")
	bool IsComplete() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HTTaskObjective")
	bool IsInProgress() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HTTaskObjective")
	bool IsWaitingToStart() const;

	UPROPERTY(BlueprintAssignable, Category = "HTTaskObjective")
	FOnObjectiveStatusUpdated OnObjectiveStarted;

	UPROPERTY(BlueprintAssignable, Category = "HTTaskObjective")
	FOnObjectiveStatusUpdated OnObjectiveCompleted;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HTTaskObjective")
	TArray<AHTTaskActor*> GatherTaskActors() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HTTaskObjective")
	FGameplayTagContainer TaskActorTags;

protected:

	UPROPERTY()
	FString TrackedData = FString();
	
	EObjectiveState State = EObjectiveState::WaitingToStart;

	/** Manually set world context object */
	UPROPERTY(BlueprintReadOnly, Category = "HTTaskObjective")
	UObject* WorldContextManual = nullptr;
};
