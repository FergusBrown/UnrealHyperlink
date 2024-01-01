// Copyright (c) 2023-2024 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkDefinition.h"

#include "HyperlinkUtility.h"
#include "LogHyperlink.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#if WITH_EDITOR
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#endif //WITH_EDITOR

void UHyperlinkDefinition::CopyLink() const
{
	CopyLink(TArray<FString>());
}

void UHyperlinkDefinition::CopyLink(const TArray<FString>& Args) const
{
	// Editor only code used to display notification
#if WITH_EDITOR
	FNotificationInfo Info{ FText::GetEmpty() };
	Info.ExpireDuration = 2.0f;
	Info.bUseSuccessFailIcons = true;
	SNotificationItem::ECompletionState CompletionState{ SNotificationItem::CS_None };
#endif //WITH_EDITOR

	if (const TSharedPtr<FJsonObject> Payload{ GeneratePayload(Args) })
	{
		CopyLink(Payload.ToSharedRef());
		
#if WITH_EDITOR
		Info.Text = FText::FromString(TEXT("Link Copied"));
		CompletionState = SNotificationItem::CS_Success;
#endif //WITH_EDITOR
	}
	else
	{
		UE_LOG(LogHyperlink, Error, TEXT("Failed to generate and copy %s link"),
			*GetClass()->GetDefaultObjectName().ToString());
		
#if WITH_EDITOR
		Info.Text = FText::FromString(TEXT("Link Copy Failed"));
		CompletionState = SNotificationItem::CS_Fail;
#endif //WITH_EDITOR
	}

#if WITH_EDITOR
	FSlateNotificationManager::Get().AddNotification(Info)->SetCompletionState(CompletionState);
#endif //WITH_EDITOR
}

void UHyperlinkDefinition::PrintLink(const TArray<FString>& Args) const
{
	if (const TSharedPtr<FJsonObject> Payload{ GeneratePayload(Args) })
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
}
