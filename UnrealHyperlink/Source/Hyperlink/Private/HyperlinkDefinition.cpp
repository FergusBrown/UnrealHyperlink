// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkDefinition.h"

#include "HyperlinkUtility.h"
#include "JsonObjectConverter.h"
#include "Log.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

void UHyperlinkDefinition::CopyLink() const
{
	if (const TSharedPtr<FJsonObject> Payload{ GeneratePayload() })
	{
		const FString ExecutePayloadString
			{ UHyperlinkUtility::CreateLinkFromPayload(GetClass(), Payload.ToSharedRef()) };
		CopyLink(ExecutePayloadString);
	}
	else
	{
		UE_LOG(LogHyperlink, Error, TEXT("Failed to generate and copy %s link"),
			*UHyperlinkUtility::CreateClassDisplayString(GetClass()));
	}
}

void UHyperlinkDefinition::PrintLink() const
{
	if (const TSharedPtr<FJsonObject> Payload{ GeneratePayload() })
	{
		UE_LOG(LogHyperlink, Display, TEXT("%s"),
			*UHyperlinkUtility::CreateLinkFromPayload(GetClass(), Payload.ToSharedRef()));
	}
	else
	{
		UE_LOG(LogHyperlink, Error, TEXT("Failed to generate and print %s link"),
			*UHyperlinkUtility::CreateClassDisplayString(GetClass()));
	}
}

void UHyperlinkDefinition::CopyLink(const FString& InLink)
{
	UE_LOG(LogHyperlink, Display, TEXT("Copied: %s"), *InLink);
	FPlatformApplicationMisc::ClipboardCopy(*InLink);
}
