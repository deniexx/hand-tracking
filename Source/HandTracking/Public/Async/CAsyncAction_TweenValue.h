// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CTickableAsyncAction.h"
#include "CAsyncAction_TweenValue.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTweenUpdate, AActor*, TargetActor, AActor*, Optional, float, CurrentValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTweenFinished, AActor*, TargetActor);

/**
 * 
 */
UCLASS()
class HANDTRACKING_API UCAsyncAction_TweenValue : public UCTickableAsyncAction
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Async Task", meta = (DisplayName = "TweenValue", WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static UCAsyncAction_TweenValue* Execute(UObject* WorldContextObject, AActor* TargetActor, AActor* OptionalActor, float StartValue, float EndValue, float Speed);

	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(BlueprintAssignable, Category = "Async Task")
	FOnTweenUpdate OnTweenUpdate;
	
	UPROPERTY(BlueprintAssignable, Category = "Async Task")
	FOnTweenFinished OnTweenFinished;

private:

	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY()
	AActor* OptionalActor;
	
	float CurrentValue = 0.f;
	float EndingValue = 0.0f;
	float TweenSpeed = 0.0f;
	
	bool bTweenFinished = false;
};
