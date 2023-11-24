// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkDefinition.h"

#include "HyperlinkSettings.h"
#include "Internationalization/Regex.h"
#include "Log.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

FString UHyperlinkDefinition::GetDefinitionIdentifier() const
{
	return DefinitionIdentifier;
}

TSubclassOf<UHyperlinkDefinitionSettings> UHyperlinkDefinition::GetSettingsClass() const
{
	return SettingsClass;
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
		ExecuteLinkBodyInternal(LinkArgs);
	}
	else
	{
		UE_LOG(LogHyperlink, Warning, TEXT("Link did not match %s pattern %s"), *DefinitionIdentifier, *BodyPattern);
	}
}

FString UHyperlinkDefinition::GetHyperlinkBase() const
{
	return GetDefault<UHyperlinkSettings>()->GetLinkGenerationBase() / DefinitionIdentifier;
}

void UHyperlinkDefinition::CopyLink() const
{
	PrintLinkInternal(true);
}

void UHyperlinkDefinition::PrintLink() const
{
	PrintLinkInternal();
}

void UHyperlinkDefinition::CopyLink(const FString& InLink)
{
	UE_LOG(LogHyperlink, Display, TEXT("Copied: %s"), *InLink);
	FPlatformApplicationMisc::ClipboardCopy(*InLink);
}

void UHyperlinkDefinition::PrintLinkInternal(const bool bCopy) const
{
	FString Link{}; 
	if (GenerateLink(Link))
	{
		if (bCopy)
		{
			CopyLink(*Link);
		}
		else
		{
			UE_LOG(LogHyperlink, Display, TEXT("%s"), *Link);
		}
	}
	else
	{
		UE_LOG(LogHyperlink, Error, TEXT("Failed to generate %s link"), *DefinitionIdentifier);
	}
}
