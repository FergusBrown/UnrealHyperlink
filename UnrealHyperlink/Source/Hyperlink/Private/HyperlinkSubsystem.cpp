// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkSubsystem.h"

#include "HyperlinkClassEntry.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkFormat.h"
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
		*FString::Format(TEXT(R"(Execute a hyperlink in the format "{0}". Note the link must be surrounded in quotes.)"), { FHyperlinkFormat::StructureHint }),
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

void UHyperlinkSubsystem::RefreshDefinitions()
{
	DeinitDefinitions();
	InitDefinitions();
}

void UHyperlinkSubsystem::InitDefinitions()
{
	// Create object for each of the project definitions
	for (const FHyperlinkClassEntry& ClassEntry : GetDefault<UHyperlinkSettings>()->GetRegisteredDefinitions())
	{
		if (ClassEntry.bEnabled && ClassEntry.Class)
		{
			if (!Definitions.Contains(ClassEntry.Identifier))
			{
				TObjectPtr<UHyperlinkDefinition> NewDefinition{ NewObject<UHyperlinkDefinition>(this, ClassEntry.Class) };
				NewDefinition->SetIdentifier(ClassEntry.Identifier);
				NewDefinition->Initialize();
				Definitions.Emplace(ClassEntry.Identifier, NewDefinition);
			}
			else
			{
				UE_LOG(LogHyperlink, Warning, TEXT("Cannot register %s: a class is already using the identifier \"%s\""),
					*ClassEntry.Class->GetName(), *ClassEntry.Identifier);
			}
		}
	}
}

void UHyperlinkSubsystem::DeinitDefinitions()
{
	for (const TPair<FString, TObjectPtr<UHyperlinkDefinition>>& Pair : Definitions)
	{
		if (Pair.Value)
		{
			Pair.Value->Deinitialize();
		}
	}
	Definitions.Empty();
}

void UHyperlinkSubsystem::CopyLinkConsole(const TArray<FString>& Args)
{
	if (Args.Num() != 1)
	{
		UE_LOG(LogHyperlink, Display, TEXT("Invalid arguments, must have only 1 argument"));
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
	if (Args.Num() != 1)
	{
		UE_LOG(LogHyperlink, Display, TEXT(R"(Invalid argument, requires 1 argument in the format "%s")"), FHyperlinkFormat::StructureHint);
	}
	else
	{
		ExecuteLink(Args[0].TrimQuotes());
	}
}

void UHyperlinkSubsystem::ExecuteLinkDeferred(const FString Link) const
{
	const FRegexPattern TypePattern{ GetDefault<UHyperlinkSettings>()->GetLinkRegexBase() +
		TEXT(R"(\/(\w+)(\/\S+))") };
	
	FRegexMatcher Matcher{ TypePattern, Link };
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
			   FHyperlinkFormat::StructureHint);
	}
}
#endif //WITH_EDITOR
