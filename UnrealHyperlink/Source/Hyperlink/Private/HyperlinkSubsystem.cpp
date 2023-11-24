// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkSubsystem.h"

#include "HyperlinkClassEntry.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkExecutePayload.h"
#include "HyperlinkPythonBridge.h"
#include "HyperlinkSettings.h"
#include "HyperlinkUtility.h"
#include "Interfaces/IMainFrameModule.h"
#include "Internationalization/Regex.h"
#include "JsonObjectConverter.h"
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
		*FString::Printf(TEXT(R"(Execute a hyperlink in the format "%s". Note the link must be surrounded in quotes.)"),
			*UHyperlinkUtility::GetLinkStructureHint()),
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

void UHyperlinkSubsystem::StaticExecuteLink(const FHyperlinkExecutePayload& ExecutePayload)
{
	if (UHyperlinkSubsystem* const HyperlinkSubsystem{ GEngine->GetEngineSubsystem<UHyperlinkSubsystem>() })
	{
		HyperlinkSubsystem->ExecuteLink(ExecutePayload);
		// if (Payload.JsonObject.IsValid())
		// {
		// 	//HyperlinkSubsystem->ExecuteLink(*Payload.JsonObject);
		// }
		// else
		// {
		// 	UE_LOG(LogHyperlink, Warning, TEXT("Failed to execute link: no valid JSON payload."));
		// }
	}
	else
	{
		UE_LOG(LogHyperlink, Warning, TEXT("Cannot execute link: UHyperlinkSubsystem not yet initialised."));
	}
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
		if (ClassEntry.bEnabled && ClassEntry.Class)
		{
			if (!Definitions.Contains(ClassEntry.Identifier))
			{
				TObjectPtr<UHyperlinkDefinition> NewDefinition{ NewObject<UHyperlinkDefinition>(this, ClassEntry.Class) };
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
void UHyperlinkSubsystem::ExecuteLink(const FHyperlinkExecutePayload& ExecutePayload)
{
	// Proceed if we're not already executing a link
	if (!PostEditorTickHandle.IsValid())
	{
		// Need to defer this to after editor tick is complete to ensure we avoid any crashes
		// This is particularly important when the link handles opening a level
		PostEditorTickHandle = GEngine->OnPostEditorTick().AddWeakLambda(this, [=](float DeltaTime)
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
			*UHyperlinkUtility::GetLinkStructureHint());
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
