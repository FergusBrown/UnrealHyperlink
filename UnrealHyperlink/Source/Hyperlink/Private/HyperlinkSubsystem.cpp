// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkSubsystem.h"

#include "HyperlinkClassEntry.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkSettings.h"
#include "Log.h"

#if WITH_EDITOR
#include "Internationalization/Regex.h"
#include "HyperlinkExecutePayload.h"
#include "HyperlinkPythonBridge.h"
#include "Interfaces/IMainFrameModule.h"
#include "HyperlinkUtility.h"
#include "JsonObjectConverter.h"
#endif //WITH_EDITOR

void UHyperlinkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	/*
	 * Here we only want to initialise the definitions if we are in a game environment. If we are in an editor
	 * environment then we want to let UHyperlinkSettings call initialise when all the editor classes are ready
	 * TODO: splitting the initialisation like this between the two classes is messy, there must be a better way
	 */
#if WITH_EDITOR
	if (!GIsEditor)
#endif //WITH_EDITOR
	{
		InitDefinitions();
	}

	// Register console commands
	CopyConsoleCommand = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("uhl.CopyLink"),
		TEXT(R"(Copy a link of the specified type. For example: "uhl.CopyLink Edit")"),
		FConsoleCommandWithArgsDelegate::CreateUObject(this, &UHyperlinkSubsystem::CopyLinkConsole));

#if WITH_EDITOR
	ExecuteConsoleCommand = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("uhl.ExecuteLink"),
		*FString::Printf(TEXT(R"(Execute a hyperlink in the format "%s". Note the link must be surrounded in quotes.)"),
			*FHyperlinkUtility::GetLinkStructureHint()),
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

UHyperlinkDefinition* UHyperlinkSubsystem::GetDefinition(
	const TSubclassOf<UHyperlinkDefinition> DefinitionClass) const
{
	UHyperlinkDefinition* Ret{ nullptr };
	for (const TPair<FString, TObjectPtr<UHyperlinkDefinition>>& Pair : Definitions)
	{
		if (Pair.Value->IsA(DefinitionClass))
		{
			Ret = Pair.Value;
			break;
		}
	}
	return Ret;
}

void UHyperlinkSubsystem::InitDefinitions()
{
	// Create object for each of the project definitions
	for (const FHyperlinkClassEntry& ClassEntry : GetDefault<UHyperlinkSettings>()->GetRegisteredDefinitions())
	{
		/*
		 * Note: How does the TSoftObjectPtr of ClassEntry.Class resolve when use it in the boolean expression below?
		 * The TSoftObjectPtr is resolved by calling FSoftObjectPath::ResolveObjectInternal which uses FindObject to
		 * try and load the class via its path. If it can't be loaded via path (e.g. python module not loaded or
		 * asset registry hasn't discovered a Blutility) then it returns nullptr and this block will be skipped
		 */
		if (ClassEntry.bEnabled && ClassEntry.Class)
		{
			if (!Definitions.Contains(ClassEntry.Identifier))
			{
				/*
				 * Note: It's fine to use Get() instead of LoadSynchronous() since the class will always be loaded by 
				 * resolve discussed above
				 */
				TObjectPtr<UHyperlinkDefinition> NewDefinition
					{ NewObject<UHyperlinkDefinition>(this, ClassEntry.Class.Get()) };
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
void UHyperlinkSubsystem::StaticExecuteLink(const FHyperlinkExecutePayload& ExecutePayload)
{
	if (UHyperlinkSubsystem* const HyperlinkSubsystem{ GEngine->GetEngineSubsystem<UHyperlinkSubsystem>() })
	{
		HyperlinkSubsystem->ExecuteLink(ExecutePayload);
	}
	else
	{
		UE_LOG(LogHyperlink, Warning, TEXT("Cannot execute link: UHyperlinkSubsystem not yet initialised."));
	}
}

void UHyperlinkSubsystem::ExecuteLink(const FHyperlinkExecutePayload& ExecutePayload)
{
	// Proceed if we're not already executing a link
	if (!PostEditorTickHandle.IsValid())
	{
		// Need to defer this to after editor tick is complete to ensure we avoid any crashes
		// This is particularly important when the link handles opening a level
		PostEditorTickHandle = GEngine->OnPostEditorTick().AddWeakLambda(this, [=, this](float DeltaTime)
		{
			ExecuteLinkDeferred(ExecutePayload);
			// Clear delegate
			GEngine->OnPostEditorTick().Remove(PostEditorTickHandle);
			PostEditorTickHandle.Reset();
		});
	}
}

void UHyperlinkSubsystem::ExecuteLink(const FString& InString)
{
	// Replace any escaped characters in the input string
	const FString ParsedString{ UHyperlinkPythonBridge::GetChecked().ParseUrlString(InString) };
	
	FHyperlinkExecutePayload Payload{};
	if (TryGetPayloadFromString(ParsedString, Payload))
	{
		ExecuteLink(Payload);
	}
	else
	{
		UE_LOG(LogHyperlink, Error, TEXT("Failed to deserialize link payload: %s"), *InString);
	}
}

void UHyperlinkSubsystem::ExecuteLinkConsole(const TArray<FString>& Args)
{
	if (Args.Num() != 1)
	{
		UE_LOG(LogHyperlink, Display, TEXT(R"(Invalid argument, requires 1 argument in the format "%s")"),
			*FHyperlinkUtility::GetLinkStructureHint());
	}
	else
	{
		ExecuteLink(Args[0].TrimQuotes());
	}
}

// NOLINTNEXTLINE(performance-unnecessary-value-param) : when passed by ref passed variable goes out of scope
void UHyperlinkSubsystem::ExecuteLinkDeferred(const FHyperlinkExecutePayload ExecutePayload) const
{
	if (ExecutePayload.Class && ExecutePayload.DefinitionPayload.JsonObject.IsValid())
	{
		if (UHyperlinkDefinition* const Definition{ GetDefinition(ExecutePayload.Class) })
		{
			Definition->ExecutePayload(ExecutePayload.DefinitionPayload.JsonObject.ToSharedRef());

			// Focus the editor window
			const IMainFrameModule& MainFrameModule = IMainFrameModule::Get();

			if (const TSharedPtr<SWindow> Window{ MainFrameModule.GetParentWindow() })
			{
				Window->HACK_ForceToFront();
			}
			
		}
		else
		{
			UE_LOG(LogHyperlink, Error, TEXT("Could not find registered definition with class %s"),
				*ExecutePayload.Class->GetDisplayNameText().ToString()	);
		}
	}
	else
	{
		UE_LOG(LogHyperlink, Warning, TEXT("Invalid payload, link execution aborted."));
		
	}
}

/*static*/bool UHyperlinkSubsystem::TryGetPayloadFromString(const FString& InString, FHyperlinkExecutePayload& OutPayload)
{
	bool bResult{ false };
	
	FRegexMatcher Matcher{ FRegexPattern(TEXT(R"(\{.*\}$)")), InString };
	if (Matcher.FindNext())
	{
		bResult = FJsonObjectConverter::JsonObjectStringToUStruct(Matcher.GetCaptureGroup(0), &OutPayload);
	}

	return bResult;
}
#endif //WITH_EDITOR
