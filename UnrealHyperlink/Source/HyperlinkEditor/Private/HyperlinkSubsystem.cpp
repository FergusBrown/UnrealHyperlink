// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkSubsystem.h"

#include "HyperlinkPipeServer.h"
#include "Log.h"

void UHyperlinkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	PipeServer = MakeUnique<FHyperlinkPipeServer>();

	// Register core link types
}

void UHyperlinkSubsystem::Deinitialize()
{
	LinkExecutorMap.Empty();
}

bool UHyperlinkSubsystem::ExecuteLink(const FString& Link) const
{
	static const TCHAR* BaseFormat{ TEXT(R"(unreal://{0}/{1}/{2})") };
	const FRegexPattern TypePattern{ FString::Format(BaseFormat, { FApp::GetProjectName(), TEXT(R"((\w+))"), TEXT(R"((.*))") }) };
	FRegexMatcher Matcher{ TypePattern, Link };
	if (!Matcher.FindNext())
	{
		UE_LOG(LogHyperlinkEditor, Error, TEXT("Failed to extract executor ID from %s. Ensure link is in the format %s"), *Link, *FString::Format(BaseFormat, { TEXT("PROJECT_NAME"), TEXT("EXECUTOR_ID"), TEXT("BODY") }));
		return false;
	}

	const FString ExecutorID{ Matcher.GetCaptureGroup(1) };
	
	if (FHyperlinkExecutor* Executor{ LinkExecutorMap.Find(FName(ExecutorID)) })
	{
		const FString LinkBody{ Matcher.GetCaptureGroup(2) };
		UE_LOG(LogHyperlinkEditor, Log, TEXT("Executing %s link with body %s"), *ExecutorID, *LinkBody);
		
		(*Executor)()
	}
	else
	{
		UE_LOG(LogHyperlinkEditor, Error, TEXT("Could not find link of type %s"));
		
	}
}

FString UHyperlinkSubsystem::GetLinkBase()
{
	return FString::Format(TEXT("unreal://"))
}

void UHyperlinkSubsystem::RegisterHyperlinkExecutor(const FName& ExecutorID, FHyperlinkExecutor Executor)
{
	LinkExecutorMap.Emplace(ExecutorID, Executor);
}


