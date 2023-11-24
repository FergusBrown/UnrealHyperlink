// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkDefinition.h"

#include "HyperlinkSettings.h"
#include "Log.h"

FString UHyperlinkDefinition::GetDefinitionIdentifier() const
{
	return DefinitionIdentifier;
}

void UHyperlinkDefinition::ExecuteLinkBody(const FString& LinkBody)
{
	const FRegexPattern LinkPattern{ GetBodyPattern() };
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
		UE_LOG(LogHyperlink, Warning, TEXT("Link did not match %s pattern %s"), *DefinitionIdentifier, *GetBodyPattern());
		
	}
}

FString UHyperlinkDefinition::GetBodyPattern() const
{
	return TEXT(R"(.*)");
}

FString UHyperlinkDefinition::GetHyperlinkBase() const
{
	return GetMutableDefault<UHyperlinkSettings>()->GetLinkGenerationBase() / DefinitionIdentifier;
}
