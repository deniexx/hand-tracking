// Fill out your copyright notice in the Description page of Project Settings.


#include "System/HTResultWriterSubsystem.h"

#include "HandTracking/HandTracking.h"

void UHTResultWriterSubsystem::BeginWritingTest(const FString& Configuration)
{
	DataToWriteToCSV.Empty();

	Config = Configuration;
	DataToWriteToCSV.Add(Configuration + '\n');
}

void UHTResultWriterSubsystem::BeginWriteTestTask(const FString& TestTask)
{
	DataToWriteToCSV.Add("Task:, " + TestTask + LINE_TERMINATOR);
	DataToWriteToCSV.Add(FString("Results:") + LINE_TERMINATOR);
}

void UHTResultWriterSubsystem::WriteTestResult(const FString& Result)
{
	DataToWriteToCSV.Add(Result + LINE_TERMINATOR);
}

void UHTResultWriterSubsystem::EndWriteTestTask()
{
	DataToWriteToCSV.Add(FString("Finished results on task!") + LINE_TERMINATOR);
}

void UHTResultWriterSubsystem::EndWritingTest()
{
	// Find Results folder, if it does not exists create it
	const FString Path = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectSavedDir(), FString("Results")));
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*Path))
	{
		PlatformFile.CreateDirectory(*Path);
	}

	// Find the correct participant number, based on the number of files in the directory
	TArray<FString> Results;
	PlatformFile.FindFiles(Results, *Path, TEXT(".csv"));
	int32 ParticipantNumber = 0;
	
	if (Results.Num() > 0)
	{
		ParticipantNumber = FMath::Floor((float)(Results.Num()) / 3.f);
	}

	// Prepare and write to file
	const FString FileName = FString::Printf(TEXT("Participant%d_%s.csv"), ParticipantNumber, *Config);
	const FString FullPath = FPaths::Combine(Path, FileName);

	UE_LOG(LogHandTracking, Warning, TEXT("DataToWriteToCSV Num: %d"), DataToWriteToCSV.Num());
	
	FFileHelper::SaveStringArrayToFile(DataToWriteToCSV, *FullPath);
}

bool UHTResultWriterSubsystem::IsInMotionControllerConfig()
{
	return Config == "MotionControllers";
}
