// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "Definitions/HyperlinkLevelActor.h"

#include "HyperlinkFormat.h"
#include "HyperlinkUtils.h"
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

UHyperlinkLevelActor::UHyperlinkLevelActor()
{
	DefinitionIdentifier = TEXT("LevelActor");

	BodyPattern = FString::Printf(TEXT("(.*)%s(.*)"), &FHyperlinkFormat::ArgSeparator);
}

void UHyperlinkLevelActor::Initialize()
{
	FHyperlinkLevelActorCommands::Register();
	LevelActorCommands = MakeShared<FUICommandList>();
	LevelActorCommands->MapAction(
		FHyperlinkLevelActorCommands::Get().CopyLevelActorLink,
		FExecuteAction::CreateUObject(this, &UHyperlinkDefinition::CopyLink)
	);
	
	FLevelEditorModule& LevelEditor{ FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor")) };
	LevelEditor.GetGlobalLevelEditorActions()->Append(LevelActorCommands.ToSharedRef());
	FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors SelectedActorsDelegate
	{
		FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors::CreateLambda([=](const TSharedRef<FUICommandList>, const TArray<AActor*>)
		{
			return FHyperlinkUtils::GetMenuExtender(TEXT("ActorOptions"), EExtensionHook::After,
				LevelActorCommands, FHyperlinkLevelActorCommands::Get().CopyLevelActorLink, TEXT("CopyLevelActorLink"));
		})
	};

	ActorContextMenuHandle = SelectedActorsDelegate.GetHandle();
	LevelEditor.GetAllLevelViewportContextMenuExtenders().Emplace(MoveTemp(SelectedActorsDelegate));
}

void UHyperlinkLevelActor::Deinitialize()
{
	FLevelEditorModule& LevelEditor{ FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor")) };
	LevelEditor.GetAllLevelViewportContextMenuExtenders().RemoveAll(
		[=](const FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors& Delegate){ return Delegate.GetHandle() == ActorContextMenuHandle; });
	
	FHyperlinkLevelActorCommands::Unregister();
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
		GEditor->SelectNone(true, true);
		GEditor->SelectActor(ActorToSelect, true, true);
		GEditor->MoveViewportCamerasToActor(*ActorToSelect, true);
	}
	else
	{
		UE_LOG(LogHyperlinkEditor, Error, TEXT("Could not find actor named %s"), *ActorName);
	}
}
