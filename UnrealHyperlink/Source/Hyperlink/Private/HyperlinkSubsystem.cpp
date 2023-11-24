// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkSubsystem.h"

#include "AssetRegistry/IAssetRegistry.h"
#include "AssetViewUtils.h"
#include "ContentBrowserModule.h"
#include "HyperlinkUtils.h"
#include "IContentBrowserSingleton.h"
#include "Log.h"

void UHyperlinkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// Register console commands
	ExecuteConsoleCommand = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("uhl.ExecuteLink"),
		*FString::Format(TEXT(R"(Execute a hyperlink in the format "{0}". Note the link must be surrounded in quotes.)"), { GetLinkFormatHint() }),
		FConsoleCommandWithArgsDelegate::CreateUObject(this, &UHyperlinkSubsystem::ExecuteLinkConsole));
}

void UHyperlinkSubsystem::Deinitialize()
{
	IConsoleManager::Get().UnregisterConsoleObject(ExecuteConsoleCommand);
}

bool UHyperlinkSubsystem::ExecuteLink(const FString& Link) const
{
	bool bSuccess{ false };
	
	const FRegexPattern TypePattern{ GetLinkBase() + TEXT(R"((\w+)(/.*))") };
	FRegexMatcher Matcher{ TypePattern, Link };
	if (Matcher.FindNext())
	{
		const FString ExecutorID{ Matcher.GetCaptureGroup(1) };
		// if (FHyperlinkExecutor* Executor{ LinkExecutorMap.Find(FName(ExecutorID)) })
		// {
		// 	const FString LinkBody{ Matcher.GetCaptureGroup(2) };
		// 	UE_LOG(LogHyperlink, Display, TEXT("Executing %s link with body %s"), *ExecutorID, *LinkBody);
		//
		// 	(*Executor)(LinkBody);
		// 	bSuccess = true;
		// }
		// else
		// {
		// 	UE_LOG(LogHyperlink, Error, TEXT("Could not find executor with ID %s"), *ExecutorID);
		// }
	}
	else
	{
		UE_LOG(LogHyperlink, Error, TEXT("Failed to extract executor ID from %s. Ensure link is in the format %s"), *Link, *GetLinkFormatHint());
	}

	return bSuccess;
}

void UHyperlinkSubsystem::ExecuteBrowse(const FString& LinkBody)
{
	TArray<FAssetData> LinkAssetData{};
	IAssetRegistry::Get()->GetAssetsByPackageName(FName(LinkBody), LinkAssetData);

	const FContentBrowserModule& ContentBrowserModule{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };
	if (LinkAssetData.Num() > 0)
	{
		// Treat as asset
		ContentBrowserModule.Get().SyncBrowserToAssets(LinkAssetData);
	}
	else
	{
		// Treat as folder
		ContentBrowserModule.Get().SyncBrowserToFolders({ LinkBody });
	}
}

void UHyperlinkSubsystem::ExecuteEdit(const FString& LinkBody)
{
	if (UObject* const Object{ FHyperlinkUtils::LoadObjectFromPackageName(LinkBody) })
	{
		AssetViewUtils::OpenEditorForAsset(Object);
	}
	else
	{
		UE_LOG(LogHyperlink, Warning, TEXT("Failed to load %s"), *LinkBody);
	}
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
		UE_LOG(LogHyperlink, Display, TEXT(R"(Invalid argument, requires 1 argument in the format "%s")"), *GetLinkFormatHint());
	}
	else
	{
		ExecuteLink(Args[0].TrimQuotes());
	}
}