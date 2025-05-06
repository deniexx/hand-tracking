// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "CTickableAsyncAction.generated.h"

/**
 * A base class for all tickable async actions
 */
UCLASS()
class HANDTRACKING_API UCTickableAsyncAction : public UCancellableAsyncAction, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	
	virtual void Tick(float DeltaTime) override;
	virtual UWorld* GetTickableGameObjectWorld() const override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableWhenPaused() const override;

protected:
	
	uint32 LastFrameNumberWeTicked = INDEX_NONE;
};
