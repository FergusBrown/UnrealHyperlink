// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "Definitions/HyperlinkEdit.h"

#include "ContentBrowserModule.h"
#include "HyperlinkCommonPayload.h"
#include "HyperlinkUtility.h"
#include "IContentBrowserSingleton.h"
#include "JsonObjectConverter.h"
#include "Log.h"

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
	UI_COMMAND(CopyContentBrowserLink, "Copy Edit Link", "Copy a link to edit the selected asset",
		EUserInterfaceActionType::Button, FInputChord(EModifierKey::Alt | EModifierKey::Shift, EKeys::E));
	UI_COMMAND(CopyAssetEditorLink, "Copy Edit Link", "Copy a link to edit this asset",
	EUserInterfaceActionType::Button, FInputChord(EModifierKey::Alt | EModifierKey::Shift, EKeys::E));
}

#undef LOCTEXT_NAMESPACE

void UHyperlinkEdit::Initialize()
{
	FHyperlinkEditCommands::Register();
	EditCommands = MakeShared<FUICommandList>();
	EditCommands->MapAction(
		FHyperlinkEditCommands::Get().CopyContentBrowserLink,
		FExecuteAction::CreateUObject(this, &UHyperlinkDefinition::CopyLink)
	);
	EditCommands->MapAction(
		FHyperlinkEditCommands::Get().CopyAssetEditorLink,
		FExecuteAction::CreateWeakLambda(this, [this]()
		{
			if(const TSharedPtr<FJsonObject> Payload{ GeneratePayloadFromAssetEditor() })
			{
				CopyLink(Payload.ToSharedRef());
			}
		})
	);

	// Content Browser asset context menu
	UHyperlinkUtility::AddHyperlinkSubMenuAndEntry(TEXT("ContentBrowser.AssetContextMenu"), TEXT("CommonAssetActions"),
		EditCommands, FHyperlinkEditCommands::Get().CopyContentBrowserLink);
	
	// Asset Editor asset menu
	// Note because of the way asset editor drop down menus work we can't (easily) add this entry in a sub menu
	UHyperlinkUtility::AddHyperlinkMenuEntry(TEXT("MainFrame.MainMenu.Asset"), EditCommands,
		FHyperlinkEditCommands::Get().CopyContentBrowserLink, false);
	
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

	// Asset editor extension delegate for recording edited asset
	TArray<FAssetEditorExtender>& AssetEditorMenuExtenderDelegates
		{ FAssetEditorToolkit::GetSharedMenuExtensibilityManager()->GetExtenderDelegates() };
	FAssetEditorExtender AssetEditorExtender{ FAssetEditorExtender::CreateUObject(this, &UHyperlinkEdit::OnExtendAssetEditor) };
	AssetEditorExtensionHandle = AssetEditorExtender.GetHandle();
	AssetEditorMenuExtenderDelegates.Emplace(MoveTemp(AssetEditorExtender));
}

void UHyperlinkEdit::Deinitialize()
{
	TArray<FAssetEditorExtender>& AssetEditorMenuExtenderDelegates
		{ FAssetEditorToolkit::GetSharedMenuExtensibilityManager()->GetExtenderDelegates() };
	AssetEditorMenuExtenderDelegates.RemoveAll(
		[=](const FAssetEditorExtender& Delegate){ return Delegate.GetHandle() == AssetEditorExtensionHandle; });
	
	FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };
	ContentBrowser.GetAllContentBrowserCommandExtenders().RemoveAll(
		[=](const FContentBrowserCommandExtender& Delegate){ return Delegate.GetHandle() == KeyboardShortcutHandle; });
	
	FHyperlinkEditCommands::Unregister();
}

TSharedPtr<FJsonObject> UHyperlinkEdit::GeneratePayload() const
{
	return GeneratePayloadFromContentBrowser();
}

TSharedPtr<FJsonObject> UHyperlinkEdit::GeneratePayloadFromContentBrowser()
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

TSharedPtr<FJsonObject> UHyperlinkEdit::GeneratePayloadFromAssetEditor() const
{
	return GeneratePayloadFromPackageName(AssetEditorPackageName);
}

TSharedPtr<FJsonObject> UHyperlinkEdit::GeneratePayloadFromPackageName(const FName& PackageName)
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

// NOLINTNEXTLINE (performance-unnecessary-value-param) Delegate signature
TSharedRef<FExtender> UHyperlinkEdit::OnExtendAssetEditor(const TSharedRef<FUICommandList> CommandList, const TArray<UObject*> ContextSensitiveObjects)
{
	const UObject* const* const AssetPtr
		{ ContextSensitiveObjects.FindByPredicate([](const UObject* const Obj){ return Obj->IsAsset(); }) };
	if (AssetPtr)
	{
		AssetEditorPackageName = (*AssetPtr)->GetPackage()->GetFName();
	}

	return MakeShared<FExtender>();
}
