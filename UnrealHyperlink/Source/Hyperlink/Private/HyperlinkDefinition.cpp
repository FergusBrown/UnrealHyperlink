// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkDefinition.h"

#include "HyperlinkFormat.h"
#include "HyperlinkSettings.h"

FString UHyperlinkDefinition::GetBodyPattern() const
{
	return TEXT(R"(.*)");
}

FString UHyperlinkDefinition::GetHyperlinkBase() const
{
	// TODO: support for web links
	return FString::Format(TEXT("{0}{1}/{2}/"), { FHyperlinkFormat::Base, GetMutableDefault<UHyperlinkSettings>()->ProjectIdentifier,  GetDefinitionName()});
}
