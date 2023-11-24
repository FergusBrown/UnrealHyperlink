// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "Definitions/HyperlinkEdit.h"

#include "ContentBrowserModule.h"
#include "HyperlinkCommonPayload.h"
#include "HyperlinkUtility.h"
#include "IContentBrowserSingleton.h"
#include "JsonObjectConverter.h"
#include "Log.h"
#include "Dom/JsonObject.h"

#define LOCTEXT_NAMESPACE "HyperlinkEdit"

FHyperlinkEditCommands::FHyperlinkEditCommands()
	: TCommands<FHyperlinkEditCommands>(
		TEXT("HyperlinkEdit"),
		NSLOCTEXT("Contexts", "HyperlinkEdit", "Hyperlink Edit"),
		NAME_None,
		FAppStyle::GetAppStyleSetName())
{
}

void FHyperlinkEditCommands::RegisterCommands()
{
	UI_COMMAND(CopyEditLink, "Copy Edit Link", "Copy a link to edit the selected asset",
		EUserInterfaceActionType::Button, FInputChord(EModifierKey::Alt | EModifierKey::Shift, EKeys::Z));
}

#undef LOCTEXT_NAMESPACE

void UHyperlinkEdit::Initialize()
{
	FHyperlinkEditCommands::Register();
	EditCommands = MakeShared<FUICommandList>();
	EditCommands->MapAction(
		FHyperlinkEditCommands::Get().CopyEditLink,
		FExecuteAction::CreateUObject(this, &UHyperlinkDefinition::CopyLink)
	);

	// Assets context menu
	UHyperlinkUtility::ExtendToolMenuSection(TEXT("ContentBrowser.AssetContextMenu"), TEXT("CommonAssetActions"),
		EditCommands, FHyperlinkEditCommands::Get().CopyEditLink);
	
	// Keyboard shortcut command
	// Note that the keyboard shortcut will only be registered if applied on startup because of the way content
	// browser commands work
	FContentBrowserCommandExtender CommandExtender
	{
		FContentBrowserCommandExtender::CreateLambda(
			[this](TSharedRef<FUICommandList> CommandList, FOnContentBrowserGetSelection GetSelectionDelegate) // NOLINT (performance-unnecessary-value-param) Delegate signature
			{
				CommandList->Append(EditCommands.ToSharedRef());
			}
		)
	};
	KeyboardShortcutHandle = CommandExtender.GetHandle();
	
	FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };
	ContentBrowser.GetAllContentBrowserCommandExtenders().Emplace(MoveTemp(CommandExtender));
}

void UHyperlinkEdit::Deinitialize()
{
	FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };
	ContentBrowser.GetAllContentBrowserCommandExtenders().RemoveAll(
		[=](const FContentBrowserCommandExtender& Delegate){ return Delegate.GetHandle() == KeyboardShortcutHandle; });
	
	FHyperlinkEditCommands::Unregister();
}

TSharedPtr<FJsonObject> UHyperlinkEdit::GeneratePayload() const
{
	TSharedPtr<FJsonObject> Payload{ nullptr };
	
	const FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };
	TArray<FAssetData> SelectedAssets{};
	ContentBrowser.Get().GetSelectedAssets(SelectedAssets);
	
	if (SelectedAssets.Num() > 0)
	{
		Payload = GeneratePayloadFromPackageName(SelectedAssets[0].PackageName);
	}
	else
	{
		UE_LOG(LogHyperlinkEditor, Display, TEXT("Cannot generate Edit link with no assets selected in Content Browser"));
	}
	
	return Payload;
}

TSharedPtr<FJsonObject> UHyperlinkEdit::GeneratePayloadFromPackageName(const FName& PackageName) const
{
	const FHyperlinkNamePayload PayloadStruct{ PackageName };
	return FJsonObjectConverter::UStructToJsonObject(PayloadStruct);
}

void UHyperlinkEdit::ExecutePayload(const TSharedRef<FJsonObject>& InPayload)
{
	FHyperlinkNamePayload PayloadStruct{};
	if (FJsonObjectConverter::JsonObjectToUStruct(InPayload, &PayloadStruct))
	{
		UHyperlinkUtility::OpenEditorForAsset(PayloadStruct.Name);
	}
}
