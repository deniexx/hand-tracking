// Fill out your copyright notice in the Description page of Project Settings.


#include "Async/CAsyncAction_TweenValue.h"

UCAsyncAction_TweenValue* UCAsyncAction_TweenValue::Execute(UObject* WorldContextObject, AActor* TargetActor, AActor* OptionalActor,
	float StartValue, float EndValue, float Speed)
{
	UCAsyncAction_TweenValue* Action = NewObject<UCAsyncAction_TweenValue>(WorldContextObject, StaticClass());
	Action->RegisterWithGameInstance(WorldContextObject);

	Action->TargetActor = TargetActor;
	Action->OptionalActor = OptionalActor;
	Action->CurrentValue = StartValue;
	Action->EndingValue = EndValue;
	Action->TweenSpeed = Speed;


	return Action;
}

void UCAsyncAction_TweenValue::Tick(float DeltaTime)
{
	if (LastFrameNumberWeTicked == GFrameCounter || bTweenFinished || !IsValid(TargetActor))
	{
		if (bTweenFinished || !IsValid(TargetActor))
		{
			SetReadyToDestroy();
		}
		return;
	}

	LastFrameNumberWeTicked = GFrameCounter;
	CurrentValue = FMath::FInterpConstantTo(CurrentValue, EndingValue, DeltaTime, TweenSpeed);
	OnTweenUpdate.Broadcast(TargetActor, OptionalActor, CurrentValue);

	if (FMath::IsNearlyEqual(CurrentValue, EndingValue))
	{
		bTweenFinished = true;
		OnTweenFinished.Broadcast(TargetActor);
		SetReadyToDestroy();
	}
}
