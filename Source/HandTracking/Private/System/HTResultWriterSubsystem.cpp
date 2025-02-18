// Fill out your copyright notice in the Description page of Project Settings.


#include "System/HTResultWriterSubsystem.h"

#include "HandTracking/HandTracking.h"

void UHTResultWriterSubsystem::BeginWritingTest(const FString& Configuration)
{
	DataToWriteToCSV.Empty();

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
	const FString Path = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectSavedDir(), FString("Results")));
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*Path))
	{
		PlatformFile.CreateDirectory(*Path);
	}

	TArray<FString> Results;
	PlatformFile.FindFiles(Results, *Path, TEXT(".csv"));
	
	const FString FileName = FString("Participant") + FString::FromInt(Results.Num()) + FString(".csv");
	const FString FullPath = FPaths::Combine(Path, FileName);

	UE_LOG(LogHandTracking, Warning, TEXT("DataToWriteToCSV Num: %d"), DataToWriteToCSV.Num());
	
	FFileHelper::SaveStringArrayToFile(DataToWriteToCSV, *FullPath);
}
