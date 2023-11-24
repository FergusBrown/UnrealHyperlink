// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkSettings.h"

#include "HyperlinkFormat.h"
#if WITH_EDITOR
#include "HyperlinkClassEntry.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkSubsystem.h"
#endif //WITH_EDITOR


void UHyperlinkSettings::PostInitProperties()
{
	Super::PostInitProperties();
	// Fill project identifier if it's empty
	if (ProjectIdentifier.IsEmpty())
	{
		ProjectIdentifier = FApp::GetProjectName();
		const FString DefaultConfigFile{ GetDefaultConfigFilename() };
		SaveConfig(CPF_Config, *DefaultConfigFile);
	}

#if WITH_EDITOR
	if (GIsEditor)
	{
		FCoreDelegates::OnAllModuleLoadingPhasesComplete.AddUObject(this, &UHyperlinkSettings::OnAllModulesLoaded);
	}
#endif //WITH_EDITOR
}

#if WITH_EDITOR
FName UHyperlinkSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

void UHyperlinkSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
		
	GEngine->GetEngineSubsystem<UHyperlinkSubsystem>()->RefreshDefinitions();
}

#endif //WITH_EDITOR

TConstArrayView<FHyperlinkClassEntry> UHyperlinkSettings::GetRegisteredDefinitions() const
{
	return TConstArrayView<FHyperlinkClassEntry>(RegisteredDefinitions);
}

FString UHyperlinkSettings::GetLinkGenerationBase() const
{
	const FString LinkBase
	{
		LinkHandlingMethod == EHyperlinkHandlingMethod::Local
			? FHyperlinkFormat::ApplicationBase
			: LinkHandlerAddress
	};

	return LinkBase / ProjectIdentifier;
}

FString UHyperlinkSettings::GetLinkRegexBase() const
{
	const FString EscapedLocalHandler{ RegexEscapeString(FHyperlinkFormat::ApplicationBase) };
	const FString EscapedWebHandler{ RegexEscapeString(LinkHandlerAddress) };
	return FString::Format(TEXT(R"((?:{0}|{1}\/){2})"),
		{ EscapedLocalHandler, EscapedWebHandler, ProjectIdentifier });
}

FString UHyperlinkSettings::RegexEscapeString(const FString& InString)
{
	// TODO: These characters are probably enough but might want to escape some more characters
	static const TArray<TCHAR> CharsToEscape{ '.', '/' };

	FString Ret{ InString };
	const FString Escape{ TEXT("\\") };
	for (const TCHAR Char : CharsToEscape)
	{
		Ret.ReplaceInline(&Char, *(Escape + Char));
	}

	return Ret;
}

#if WITH_EDITOR
void UHyperlinkSettings::OnAllModulesLoaded()
{
	// Remove null classes
	RegisteredDefinitions.RemoveAll([](const FHyperlinkClassEntry& Entry){ return Entry.Class == nullptr; });

	// Populate the list
	for(TObjectIterator<UClass> It; It; ++It)
	{
		if(It->IsChildOf(UHyperlinkDefinition::StaticClass()) && !It->HasAnyClassFlags(CLASS_Abstract))
		{
			UClass* Class{ *It };
			if (!RegisteredDefinitions.FindByPredicate([=](const FHyperlinkClassEntry& Entry)
				{ return Entry.Class == Class; }))
			{
				FHyperlinkClassEntry NewEntry{};
				NewEntry.Class = Class;
				NewEntry.Identifier = GetDefault<UHyperlinkDefinition>(*It)->GetIdentifier();
				RegisteredDefinitions.Add(NewEntry);
			}
		}
	}
		
	RegisteredDefinitions.Sort([](const FHyperlinkClassEntry& Lhs, const FHyperlinkClassEntry& Rhs)
		{ return Lhs.Class->GetName() < Rhs.Class->GetName(); });

	// Ensure subsystem is up to date
	GEngine->GetEngineSubsystem<UHyperlinkSubsystem>()->RefreshDefinitions();
}
#endif //WITH_EDITOR
