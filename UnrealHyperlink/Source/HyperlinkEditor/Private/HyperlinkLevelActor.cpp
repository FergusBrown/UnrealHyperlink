// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkLevelActor.h"

#include "HyperlinkFormat.h"
#include "HyperlinkUtils.h"
#include "Log.h"
#include "Selection.h"

UHyperlinkLevelActor::UHyperlinkLevelActor()
{
	DefinitionIdentifier = TEXT("LevelActor");

	BodyPattern = FString::Printf(TEXT("(.*)%s(.*)"), &FHyperlinkFormat::ArgSeparator);
}

void UHyperlinkLevelActor::Initialize()
{

}

void UHyperlinkLevelActor::Deinitialize()
{

}

bool UHyperlinkLevelActor::GenerateLink(FString& OutLink) const
{
	bool bSuccess{ false };
	
	const FString& LevelPackageName{ GEditor->GetEditorWorldContext().World()->PersistentLevel->GetPackage()->GetName() };
	if (const USelection* const Selection{ GEditor->GetSelectedActors() })
	{
		bSuccess = Selection->Num() > 0;
		if (bSuccess)
		{
			const FString& ObjectName{ Selection->GetSelectedObject(0)->GetName() };
			OutLink = GetHyperlinkBase() / LevelPackageName + FHyperlinkFormat::ArgSeparator + ObjectName;
		}
	}
	UE_CLOG(!bSuccess, LogHyperlinkEditor, Error, TEXT("Could not generate actor link: no actor selected"));
	
	return bSuccess;
}

void UHyperlinkLevelActor::ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments)
{
	const FString& LevelPackageName{ LinkArguments[1] };
	const FString& ActorName{ LinkArguments[2] };

	FHyperlinkUtils::OpenEditorForAsset(LevelPackageName);
	if (AActor* const ActorToSelect{ GEditor->SelectNamedActor(*ActorName) })
	{
		GEditor->SelectActor(ActorToSelect, true, true);
		GEditor->MoveViewportCamerasToActor(*ActorToSelect, true);
	}
	else
	{
		UE_LOG(LogHyperlinkEditor, Error, TEXT("Could not find actor named %s"), *ActorName);
	}
}
