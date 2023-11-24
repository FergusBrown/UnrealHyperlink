// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkDefinition.h"

#include "HyperlinkFormat.h"
#include "HyperlinkSettings.h"
#include "Internationalization/Regex.h"
#include "Log.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

FString UHyperlinkDefinition::GetIdentifier() const
{
	return DefinitionIdentifier;
}

void UHyperlinkDefinition::SetIdentifier(const FString& InIdentifier)
{
	DefinitionIdentifier = InIdentifier;
}

void UHyperlinkDefinition::ExecuteLinkBody(const FString& InLinkBody)
{
	const FRegexPattern LinkPattern{ BodyPattern };
	FRegexMatcher LinkMatcher{ LinkPattern, InLinkBody };

	if (LinkMatcher.FindNext())
	{
		TArray<FString> LinkArgs{};
		int32 Idx{ 0 };
		while (!LinkMatcher.GetCaptureGroup(Idx).IsEmpty())
		{
			LinkArgs.Emplace(LinkMatcher.GetCaptureGroup(Idx));
			++Idx;
		}
		ExecuteExtractedArgs(LinkArgs);
	}
	else
	{
		UE_LOG(LogHyperlink, Warning, TEXT("Link did not match %s pattern %s"), *DefinitionIdentifier, *BodyPattern);
	}
}

FString UHyperlinkDefinition::GetHyperlinkBase() const
{
	return FHyperlinkFormat::GetLinkGenerationBase() / DefinitionIdentifier;
}

void UHyperlinkDefinition::CopyLink() const
{
	FString Link{}; 
	if (GenerateLink(Link))
	{
		CopyLink(*Link);
	}
	else
	{
		UE_LOG(LogHyperlink, Error, TEXT("Failed to generate and copy %s link"), *DefinitionIdentifier);
	}
}

void UHyperlinkDefinition::PrintLink() const
{
	FString Link{}; 
	if (GenerateLink(Link))
	{
		UE_LOG(LogHyperlink, Display, TEXT("%s"), *Link);
	}
	else
	{
		UE_LOG(LogHyperlink, Error, TEXT("Failed to generate and print %s link"), *DefinitionIdentifier);
	}
}

#if WITH_EDITOR
void UHyperlinkDefinition::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// Save any user properties for this definition
	SaveConfig(CPF_Config, *GetGlobalUserConfigFilename());

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif //WITH_EDITOR

void UHyperlinkDefinition::CopyLink(const FString& InLink)
{
	UE_LOG(LogHyperlink, Display, TEXT("Copied: %s"), *InLink);
	FPlatformApplicationMisc::ClipboardCopy(*InLink);
}
