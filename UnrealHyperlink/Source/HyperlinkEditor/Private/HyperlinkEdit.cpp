// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkEdit.h"

#include "ContentBrowserModule.h"
#include "HyperlinkUtils.h"
#include "IContentBrowserSingleton.h"
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
	UI_COMMAND(CopyEditLink, "Copy Edit Link", "Copy a link to edit the selected asset", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Alt | EModifierKey::Shift, EKeys::X));
}

#undef LOCTEXT_NAMESPACE

UHyperlinkEdit::UHyperlinkEdit()
{
	DefinitionIdentifier = TEXT("Edit");
}

void UHyperlinkEdit::Initialize()
{
	FHyperlinkEditCommands::Register();
	EditCommands = MakeShared<FUICommandList>();
	EditCommands->MapAction(
		FHyperlinkEditCommands::Get().CopyEditLink,
		FExecuteAction::CreateUObject(this, &UHyperlinkDefinition::CopyLink)
	);
	
	// Assets context menu
	FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };
	if (bEnableInAssetContextMenu)
	{
		FContentBrowserMenuExtender_SelectedAssets SelectedAssetsDelegate
		{
			FContentBrowserMenuExtender_SelectedAssets::CreateLambda([=](const TArray<FAssetData>&)
			{
				return FHyperlinkUtils::GetMenuExtender(TEXT("CommonAssetActions"), EExtensionHook::After, EditCommands, FHyperlinkEditCommands::Get().CopyEditLink, TEXT("CopyEditLink"));
			})
		};
		AssetContextMenuHandle = SelectedAssetsDelegate.GetHandle();
		ContentBrowser.GetAllAssetViewContextMenuExtenders().Emplace(MoveTemp(SelectedAssetsDelegate));
	}

	// Keyboard shortcut command
	// Note that the keyboard shortcut will only be registered if applied on startup because of the way content
	// browser commands work
	FContentBrowserCommandExtender CommandExtender
	{
		FContentBrowserCommandExtender::CreateLambda(
			[this](TSharedRef<FUICommandList> CommandList, FOnContentBrowserGetSelection GetSelectionDelegate)
			{
				CommandList->Append(EditCommands.ToSharedRef());
			}
		)
	};
	KeyboardShortcutHandle = CommandExtender.GetHandle();
	ContentBrowser.GetAllContentBrowserCommandExtenders().Emplace(MoveTemp(CommandExtender));
}

void UHyperlinkEdit::Deinitialize()
{
	FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };

	ContentBrowser.GetAllAssetViewContextMenuExtenders().RemoveAll(
		[=](const FContentBrowserMenuExtender_SelectedAssets& Delegate){ return Delegate.GetHandle() == AssetContextMenuHandle; });
	ContentBrowser.GetAllContentBrowserCommandExtenders().RemoveAll(
		[=](const FContentBrowserCommandExtender& Delegate){ return Delegate.GetHandle() == KeyboardShortcutHandle; });
	FHyperlinkEditCommands::Unregister();
}

bool UHyperlinkEdit::GenerateLink(FString& OutLink) const
{
	const FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };
	TArray<FAssetData> SelectedAssets{};
	ContentBrowser.Get().GetSelectedAssets(SelectedAssets);
	
	const bool bSuccess{ SelectedAssets.Num() > 0 };
	if (bSuccess)
	{
		OutLink = GenerateLinkFromPackageName(SelectedAssets[0].PackageName.ToString());
	}
	else
	{
		UE_LOG(LogHyperlinkEditor, Display, TEXT("Cannot generate Edit link with no assets selected in Content Browser"));
	}
	
	return bSuccess;
}

FString UHyperlinkEdit::GenerateLinkFromPackageName(const FString& PackageName) const
{
	return GetHyperlinkBase() / PackageName;
}

void UHyperlinkEdit::ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments)
{
	FHyperlinkUtils::OpenEditorForAsset(LinkArguments[0]);
}
