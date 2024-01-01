// Copyright (c) 2023-2024 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "Definitions/HyperlinkLevelActor.h"

#include "HyperlinkUtility.h"
#include "JsonObjectConverter.h"
#include "LevelEditor.h"
#include "Log.h"
#include "Selection.h"

#define LOCTEXT_NAMESPACE "HyperlinkLevelActor"

FHyperlinkLevelActorCommands::FHyperlinkLevelActorCommands()
	: TCommands<FHyperlinkLevelActorCommands>(
		TEXT("HyperlinkLevelActor"),
		NSLOCTEXT("Contexts", "HyperlinkLevelActor", "Hyperlink LevelActor"),
		NAME_None,
		FAppStyle::GetAppStyleSetName())
{
}

void FHyperlinkLevelActorCommands::RegisterCommands()
{
	UI_COMMAND(CopyLevelActorLink, "Copy Actor Link", "Copy a link to focus the selected actor in the level editor",
		EUserInterfaceActionType::Button, FInputChord(EModifierKey::Alt | EModifierKey::Shift, EKeys::C));
}

#undef LOCTEXT_NAMESPACE

void UHyperlinkLevelActor::Initialize()
{
	FHyperlinkLevelActorCommands::Register();
	LevelActorCommands = MakeShared<FUICommandList>();
	LevelActorCommands->MapAction(
		FHyperlinkLevelActorCommands::Get().CopyLevelActorLink,
		FExecuteAction::CreateUObject(this, &UHyperlinkDefinition::CopyLink)
	);

	const FLevelEditorModule& LevelEditor{ FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor")) };
	LevelEditor.GetGlobalLevelEditorActions()->Append(LevelActorCommands.ToSharedRef());

	// Just add the menu entry to avoid duplicate sub menus in world outliner
	FHyperlinkUtility::AddHyperlinkSubMenuAndEntry(TEXT("LevelEditor.ActorContextMenu"), TEXT("ActorOptions"),
	LevelActorCommands, FHyperlinkLevelActorCommands::Get().CopyLevelActorLink);

	FHyperlinkUtility::AddHyperlinkMenuEntry(TEXT("LevelEditor.LevelEditorSceneOutliner.ContextMenu"),
	LevelActorCommands, FHyperlinkLevelActorCommands::Get().CopyLevelActorLink);
}

void UHyperlinkLevelActor::Deinitialize()
{
	FHyperlinkLevelActorCommands::Unregister();
}

TSharedPtr<FJsonObject> UHyperlinkLevelActor::GeneratePayload(const TArray<FString>& Args) const
{
	TSharedPtr<FJsonObject> Payload{ nullptr };
	
	if (const USelection* const Selection{ GEditor->GetSelectedActors() })
	{
		if (Selection->Num() > 0)
		{
			const FHyperlinkLevelActorPayload PayloadStruct
			{
				GEditor->GetEditorWorldContext().World()->PersistentLevel->GetPackage()->GetFName(),
				Selection->GetSelectedObject(0)->GetFName()
			};
			Payload = FJsonObjectConverter::UStructToJsonObject(PayloadStruct);
		}
		else
		{
			UE_LOG(LogHyperlinkEditor, Error, TEXT("Could not generate actor link: no actor selected"));
		}
	}
	
	return Payload;
}

void UHyperlinkLevelActor::ExecutePayload(const TSharedRef<FJsonObject>& InPayload)
{
	FHyperlinkLevelActorPayload PayloadStruct{};
	if (FJsonObjectConverter::JsonObjectToUStruct(InPayload, &PayloadStruct))
	{
		const FName& LevelPackageName{ PayloadStruct.LevelPackageName };
		const FName& ActorName{ PayloadStruct.ActorName };

		FHyperlinkUtility::OpenEditorForAsset(LevelPackageName);
		if (AActor* const ActorToSelect{ GEditor->SelectNamedActor(*ActorName.ToString()) })
		{
			GEditor->SelectNone(true, true);
			GEditor->SelectActor(ActorToSelect, true, true);
			GEditor->MoveViewportCamerasToActor(*ActorToSelect, true);
		}
		else
		{
			UE_LOG(LogHyperlinkEditor, Error, TEXT("Could not find actor named %s"), *ActorName.ToString());
		}
	}
}
