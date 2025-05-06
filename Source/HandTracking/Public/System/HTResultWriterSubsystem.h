// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HTResultWriterSubsystem.generated.h"

/**
 * A simple subsystem used to write test results to a file
 */
UCLASS()
class HANDTRACKING_API UHTResultWriterSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Begins writing tests for a given configuration
	 * @param Configuration The input configuration to start writing tests for
	 */
	void BeginWritingTest(const FString& Configuration);

	/**
	 * Begins writing test results for a given task
	 * @param TestTask The task to start writing test results for
	 */
	void BeginWriteTestTask(const FString& TestTask);

	/**
	 * Writes a test result for the currently started task
	 * @param Result The results to write
	 */
	void WriteTestResult(const FString& Result);

	/**
	 * Ends writing test results for the active task
	 */
	void EndWriteTestTask();

	/**
	 * Ends writing the given test for an input configuration, this includes saving all results to a file
	 */
	void EndWritingTest();

	/**
	 * Checks if the player is in the motion controller input configuration
	 * @return TRUE - the player is using motion controllers, FALSE - the player is not using them 
	 */
	bool IsInMotionControllerConfig();
	
private:

	FString Config;

	UPROPERTY()
	TArray<FString> DataToWriteToCSV;
};
