// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkSubsystem.h"

#include "HyperlinkPipeServer.h"
#include "Log.h"

void UHyperlinkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	PipeServer = MakeUnique<FHyperlinkPipeServer>();

	// Register core link types
	RegisterHyperlinkExecutor(TEXT("browse"), UHyperlinkSubsystem::ExecuteBrowse);
	RegisterHyperlinkExecutor(TEXT("edit"), UHyperlinkSubsystem::ExecuteEdit);

	// Register console commands
	// TODO: unregister on shutdown
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("uhl.ExecuteLink"),
		*FString::Format(TEXT(R"(Execute a hyperlink in the format "{0}". Note the link must be surrounded in quotes.)"), { GetLinkFormatHint() }),
		FConsoleCommandWithArgsDelegate::CreateUObject(this, &UHyperlinkSubsystem::ExecuteLinkConsole));
}

void UHyperlinkSubsystem::Deinitialize()
{
	LinkExecutorMap.Empty();
}

bool UHyperlinkSubsystem::ExecuteLink(const FString& Link) const
{
	bool bResult{ false };
	
	const FRegexPattern TypePattern{ GetLinkBase() + TEXT(R"((\w+)/(.*))") };
	FRegexMatcher Matcher{ TypePattern, Link };
	if (Matcher.FindNext())
	{
		const FString ExecutorID{ Matcher.GetCaptureGroup(1) };
		if (FHyperlinkExecutor* Executor{ LinkExecutorMap.Find(FName(ExecutorID)) })
		{
			const FString LinkBody{ Matcher.GetCaptureGroup(2) };
			UE_LOG(LogHyperlinkEditor, Display, TEXT("Executing %s link with body %s"), *ExecutorID, *LinkBody);
		
			bResult = (*Executor)(Link);

			UE_CLOG(!bResult, LogHyperlinkEditor, Warning, TEXT("Failed to execute link %s"), *Link);
		}
		else
		{
			UE_LOG(LogHyperlinkEditor, Error, TEXT("Could not find executor with ID %s"), *ExecutorID);
		}
	}
	else
	{
		UE_LOG(LogHyperlinkEditor, Error, TEXT("Failed to extract executor ID from %s. Ensure link is in the format %s"), *Link, *GetLinkFormatHint());
	}

	return bResult;
}

void UHyperlinkSubsystem::RegisterHyperlinkExecutor(const FName& ExecutorID, FHyperlinkExecutor Executor)
{
	LinkExecutorMap.Emplace(ExecutorID, Executor);
}

bool UHyperlinkSubsystem::ExecuteBrowse(const FString& Link)
{
	UE_LOG(LogHyperlinkEditor, Display, TEXT("Executing browse link (temp log)"))
	return true;
}

bool UHyperlinkSubsystem::ExecuteEdit(const FString& Link)
{
	UE_LOG(LogHyperlinkEditor, Display, TEXT("Executing edit link (temp log)"))
	return true;
}

FString UHyperlinkSubsystem::GetLinkBase()
{
	return FString::Format(TEXT("unreal://{0}/"), { FApp::GetProjectName() });
}

FString UHyperlinkSubsystem::GetLinkFormatHint()
{
	return GetLinkBase() + TEXT("EXECUTOR_ID/BODY");
}

void UHyperlinkSubsystem::ExecuteLinkConsole(const TArray<FString>& Args) const
{
	if (Args.Num() != 1) // TODO: check link arg is valid
	{
		UE_LOG(LogHyperlinkEditor, Display, TEXT(R"(Invalid argument, requires 1 argument in the format "%s")"), *GetLinkFormatHint());
	}
	else
	{
		ExecuteLink(Args[0].TrimQuotes());
	}
}
