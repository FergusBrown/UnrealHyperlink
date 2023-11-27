// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkDefinition.h"

#include "HyperlinkUtility.h"
#include "Log.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#if WITH_EDITOR
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#endif //WITH_EDITOR

void UHyperlinkDefinition::CopyLink() const
{
	if (const TSharedPtr<FJsonObject> Payload{ GeneratePayload() })
	{
		CopyLink(Payload.ToSharedRef());
	}
	else
	{
		UE_LOG(LogHyperlink, Error, TEXT("Failed to generate and copy %s link"),
			*GetClass()->GetDefaultObjectName().ToString());
	}
}

void UHyperlinkDefinition::PrintLink() const
{
	if (const TSharedPtr<FJsonObject> Payload{ GeneratePayload() })
	{
		UE_LOG(LogHyperlink, Display, TEXT("%s"),
			*FHyperlinkUtility::CreateLinkFromPayload(GetClass(), Payload.ToSharedRef()));
	}
	else
	{
		UE_LOG(LogHyperlink, Error, TEXT("Failed to generate and print %s link"),
			*GetClass()->GetDefaultObjectName().ToString());
	}
}

void UHyperlinkDefinition::CopyLink(const TSharedRef<FJsonObject>& Payload) const
{
	const FString LinkString
		{ FHyperlinkUtility::CreateLinkFromPayload(GetClass(), Payload) };
	UE_LOG(LogHyperlink, Display, TEXT("Copied: %s"), *LinkString);
	FPlatformApplicationMisc::ClipboardCopy(*LinkString);

#if WITH_EDITOR
	FNotificationInfo Info{ FText::FromString(TEXT("Link Copied")) };
	Info.ExpireDuration = 2.0f;
	FSlateNotificationManager::Get().AddNotification(Info);
#endif //WITH_EDITOR
}
