// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkSubsystem.h"

#include "HyperlinkDefinition.h"
#include "HyperlinkSettings.h"
#include "Log.h"

void UHyperlinkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
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
	
#if WITH_EDITOR
	// Register console commands
	ExecuteConsoleCommand = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("uhl.ExecuteLink"),
		*FString::Format(TEXT(R"(Execute a hyperlink in the format "{0}". Note the link must be surrounded in quotes.)"), { GetLinkFormatHint() }),
		FConsoleCommandWithArgsDelegate::CreateUObject(this, &UHyperlinkSubsystem::ExecuteLinkConsole));
#endif //WITH_EDITOR
}

void UHyperlinkSubsystem::Deinitialize()
{
	// Deinitialize definitions
	// TODO: can we make this a set or just make it an array?
	for (const TPair<FName, TObjectPtr<UHyperlinkDefinition>>& Pair : Definitions)
	{
		if (Pair.Value)
		{
			Pair.Value->Deinitialize();
		}
	}
	Definitions.Empty();
	
#if WITH_EDITOR
	IConsoleManager::Get().UnregisterConsoleObject(ExecuteConsoleCommand);
#endif //WITH_EDITOR
}

void UHyperlinkSubsystem::ExecuteLink(const FString& Link) const
{
	const FRegexPattern TypePattern{ GetLinkBase() + TEXT(R"((\w+)(/.*))") };
	FRegexMatcher Matcher{ TypePattern, Link };
	if (Matcher.FindNext())
	{
		const FString DefName{ Matcher.GetCaptureGroup(1) };
		if (const TObjectPtr<UHyperlinkDefinition>* Def{ Definitions.Find(DefName) })
		{
			const FString LinkBody{ Matcher.GetCaptureGroup(2) };
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
		UE_LOG(LogHyperlink, Error, TEXT("Failed to extract definition name from %s. Ensure link is in the format %s"), *Link, *GetLinkFormatHint());
	}
}

FString UHyperlinkSubsystem::GetLinkBase()
{
	return FString::Format(TEXT("unreal://{0}/"), { FApp::GetProjectName() });
}

FString UHyperlinkSubsystem::GetLinkFormatHint()
{
	return GetLinkBase() + TEXT("DEFINITION/BODY");
}

#if WITH_EDITOR
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
#endif //WITH_EDITOR
