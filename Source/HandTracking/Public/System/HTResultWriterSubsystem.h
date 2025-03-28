// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HTResultWriterSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class HANDTRACKING_API UHTResultWriterSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	void BeginWritingTest(const FString& Configuration);
	
	void BeginWriteTestTask(const FString& TestTask);
	void WriteTestResult(const FString& Result);
	void EndWriteTestTask();

	void EndWritingTest();

	bool IsInMotionControllerConfig();
	
private:

	FString Config;

	UPROPERTY()
	TArray<FString> DataToWriteToCSV;
};
