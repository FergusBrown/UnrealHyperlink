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

void UHyperlinkDefinition::ExecuteLinkBody(const FString& LinkBody)
{
	const FRegexPattern LinkPattern{ BodyPattern };
	FRegexMatcher LinkMatcher{ LinkPattern, LinkBody };

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
	FString Link{}; 
	if (GenerateLink(Link))
	{
		FPlatformApplicationMisc::ClipboardCopy(*Link);
	}
	else
	{
		UE_LOG(LogHyperlink, Error, TEXT("Failed to generated %s link"), *DefinitionIdentifier);
	}
}
