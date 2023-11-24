// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkSubsystem.h"

#include "HyperlinkDefinition.h"
#include "HyperlinkSettings.h"
#include "Internationalization/Regex.h"
#include "Log.h"

void UHyperlinkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	InitDefinitions();

	// Register console commands
	CopyConsoleCommand = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("uhl.CopyLink"),
		TEXT(R"(Copy a link of the specified type. For example: "uhl.CopyLink Edit")"),
		FConsoleCommandWithArgsDelegate::CreateUObject(this, &UHyperlinkSubsystem::CopyLinkConsole));
#if WITH_EDITOR
	ExecuteConsoleCommand = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("uhl.ExecuteLink"),
		*FString::Format(TEXT(R"(Execute a hyperlink in the format "{0}". Note the link must be surrounded in quotes.)"), { GetLinkFormatHint() }),
		FConsoleCommandWithArgsDelegate::CreateUObject(this, &UHyperlinkSubsystem::ExecuteLinkConsole));
#endif //WITH_EDITOR
}

void UHyperlinkSubsystem::Deinitialize()
{
	DeinitDefinitions();
	IConsoleManager::Get().UnregisterConsoleObject(CopyConsoleCommand);
#if WITH_EDITOR
	IConsoleManager::Get().UnregisterConsoleObject(ExecuteConsoleCommand);
#endif //WITH_EDITOR
}

FString UHyperlinkSubsystem::GetLinkBase()
{
	return FString::Format(TEXT("unreal://{0}/"), { FApp::GetProjectName() });
}

FString UHyperlinkSubsystem::GetLinkFormatHint()
{
	return GetLinkBase() + TEXT("DEFINITION/BODY");
}

void UHyperlinkSubsystem::CopyLinkConsole(const TArray<FString>& Args)
{
	if (Args.Num() != 1)
	{
		UE_LOG(LogHyperlink, Display, TEXT(R"(Invalid arguments, must have only 1 argument)"), *GetLinkFormatHint());
	}
	else
	{
		if (const TObjectPtr<UHyperlinkDefinition>* Def{ Definitions.Find(Args[0]) })
		{
			(*Def)->CopyLink();
		}
		else
		{
			UE_LOG(LogHyperlink, Error, TEXT("No registered definited with the identifier %s"), *Args[0]);
		}
	}
}

void UHyperlinkSubsystem::RefreshDefinitions()
{
	DeinitDefinitions();
	InitDefinitions();
}

void UHyperlinkSubsystem::InitDefinitions()
{
	// Create object for each of the project definitions
	for (const TSubclassOf<UHyperlinkDefinition> DefClass : GetDefault<UHyperlinkSettings>()->GetRegisteredDefinitions())
	{
		if (DefClass)
		{
			TObjectPtr<UHyperlinkDefinition> NewDefinition{ NewObject<UHyperlinkDefinition>(this, DefClass) };
			NewDefinition->Initialize();
			Definitions.Emplace(NewDefinition->GetDefinitionIdentifier(), NewDefinition);
		}
	}
}

void UHyperlinkSubsystem::DeinitDefinitions()
{
	// TODO: can we make this a set or just make it an array?
	for (const TPair<FName, TObjectPtr<UHyperlinkDefinition>>& Pair : Definitions)
	{
		if (Pair.Value)
		{
			Pair.Value->Deinitialize();
		}
	}
	Definitions.Empty();
}

#if WITH_EDITOR
void UHyperlinkSubsystem::ExecuteLink(const FString& Link)
{
	if (!PostEditorTickHandle.IsValid())
	{
		// Need to defer this to after editor tick is complete to ensure we avoid any crashes
		// This is particularly important when the link handles opening a level
		PostEditorTickHandle = GEngine->OnPostEditorTick().AddWeakLambda(this, [this, Link](float DeltaTime)
		{
			ExecuteLinkDeferred(Link);
			// Clear delegate
			GEngine->OnPostEditorTick().Remove(PostEditorTickHandle);
			PostEditorTickHandle.Reset();
		});
	}
}

void UHyperlinkSubsystem::ExecuteLinkConsole(const TArray<FString>& Args)
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

void UHyperlinkSubsystem::ExecuteLinkDeferred(const FString Link) const
{
	const FRegexPattern TypePattern{GetLinkBase() + TEXT(R"((\w+)(/.*))")};
	FRegexMatcher Matcher{TypePattern, Link};
	if (Matcher.FindNext())
	{
		const FString DefName{Matcher.GetCaptureGroup(1)};
		if (const TObjectPtr<UHyperlinkDefinition>* Def{Definitions.Find(DefName)})
		{
			const FString LinkBody{Matcher.GetCaptureGroup(2)};
			UE_LOG(LogHyperlink, Display, TEXT("Executing %s link with body %s"), *DefName, *LinkBody);

			(*Def)->ExecuteLinkBody(LinkBody);
		}
		else
		{
			UE_LOG(LogHyperlink, Error, TEXT("Could not find definition with name %s"), *DefName);
		}
	}
	else
	{
		UE_LOG(LogHyperlink, Error,
			   TEXT("Failed to extract definition name from %s. Ensure link is in the format %s"), *Link,
			   *GetLinkFormatHint());
	}
}
#endif //WITH_EDITOR
