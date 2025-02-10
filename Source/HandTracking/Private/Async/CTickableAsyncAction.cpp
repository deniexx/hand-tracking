// Fill out your copyright notice in the Description page of Project Settings.


#include "Async/CTickableAsyncAction.h"

void UCTickableAsyncAction::Tick(float DeltaTime)
{
	
}

UWorld* UCTickableAsyncAction::GetTickableGameObjectWorld() const
{
	return GetWorld();
}

TStatId UCTickableAsyncAction::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCTickableAsyncAction, STATGROUP_Tickables);
}

bool UCTickableAsyncAction::IsTickableWhenPaused() const
{
	return false;
}
