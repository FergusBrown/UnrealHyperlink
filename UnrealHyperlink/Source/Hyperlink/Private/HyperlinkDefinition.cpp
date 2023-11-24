// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkDefinition.h"

#include "HyperlinkFormat.h"
#include "HyperlinkSettings.h"
#include "HyperlinkUtility.h"
#include "JsonObjectConverter.h"
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

void UHyperlinkDefinition::CopyLink() const
{
	if (const TSharedPtr<FJsonObject> Payload{ GeneratePayload() })
	{
		const FString ExecutePayloadString{ UHyperlinkUtility::CreateLinkFromPayload(this->GetClass(), Payload.ToSharedRef()) };
		CopyLink(ExecutePayloadString);
	}
	else
	{
		UE_LOG(LogHyperlink, Error, TEXT("Failed to generate and copy %s link"), *DefinitionIdentifier);
	}
}

void UHyperlinkDefinition::PrintLink() const
{
	if (const TSharedPtr<FJsonObject> Payload{ GeneratePayload() })
	{
		UE_LOG(LogHyperlink, Display, TEXT("%s"),
			*UHyperlinkUtility::CreateLinkFromPayload(this->GetClass(), Payload.ToSharedRef()));
	}
	else
	{
		UE_LOG(LogHyperlink, Error, TEXT("Failed to generate and print %s link"), *DefinitionIdentifier);
	}
}

void UHyperlinkDefinition::CopyLink(const FString& InLink)
{
	UE_LOG(LogHyperlink, Display, TEXT("Copied: %s"), *InLink);
	FPlatformApplicationMisc::ClipboardCopy(*InLink);
}
