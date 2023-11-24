// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkEdit.h"

#if WITH_EDITOR
#include "AssetViewUtils.h"
#include "ContentBrowserModule.h"
#include "HyperlinkUtils.h"
#include "IContentBrowserSingleton.h"
#include "Styling/StarshipCoreStyle.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

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
	UI_COMMAND(GenerateEditLink, "Copy Edit Link", "Copy a link to edit this asset", EUserInterfaceActionType::Button, FInputChord());
}

#endif //WITH_EDITOR

FString UHyperlinkEdit::GetDefinitionName() const
{
	return TEXT("edit");
}

void UHyperlinkEdit::Initialize()
{
#if WITH_EDITOR
	FHyperlinkEditCommands::Register();
	EditCommands = MakeShared<FUICommandList>();
	EditCommands->MapAction(
		FHyperlinkEditCommands::Get().GenerateEditLink,
		FExecuteAction::CreateLambda([=]()
			{
				const FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };
				TArray<FAssetData> SelectedAssets{};
				ContentBrowser.Get().GetSelectedAssets(SelectedAssets);
				if (SelectedAssets.Num() > 0)
				{
					FPlatformApplicationMisc::ClipboardCopy(*GenerateLink(SelectedAssets[0].PackageName.ToString()));
				}
			}
		)
	);
	
	// Assets context menu
	FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };
	FContentBrowserMenuExtender_SelectedAssets SelectedAssetsDelegate
	{
		FContentBrowserMenuExtender_SelectedAssets::CreateUObject(this, &UHyperlinkEdit::OnExtendAssetContextMenu)
	};
	AssetContextMenuHandle = SelectedAssetsDelegate.GetHandle();
	ContentBrowser.GetAllAssetViewContextMenuExtenders().Emplace(MoveTemp(SelectedAssetsDelegate));
#endif //WITH_EDITOR
}

void UHyperlinkEdit::Deinitialize()
{
#if WITH_EDITOR
	FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };

	ContentBrowser.GetAllAssetViewContextMenuExtenders().RemoveAll(
		[=](const FContentBrowserMenuExtender_SelectedAssets& Delegate){ return Delegate.GetHandle() == AssetContextMenuHandle; });
	FHyperlinkEditCommands::Unregister();
#endif //WITH_EDITOR
}

FString UHyperlinkEdit::GenerateLink(const FString& PackageName) const
{
	return GetHyperlinkBase() / PackageName;
}

void UHyperlinkEdit::ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments)
{
#if WITH_EDITOR
  	if (UObject* const Object{ FHyperlinkUtils::LoadObjectFromPackageName(LinkArguments[0]) })
	{
		AssetViewUtils::OpenEditorForAsset(Object);
	}
#endif //WITH_EDITOR
}

#if WITH_EDITOR
TSharedRef<FExtender> UHyperlinkEdit::OnExtendAssetContextMenu(const TArray<FAssetData>& SelectedAssets) const
{
	TSharedRef<FExtender> Extender{ MakeShared<FExtender>() };
	Extender->AddMenuExtension(
		TEXT("CommonAssetActions"),
		EExtensionHook::After,
		EditCommands,
		FMenuExtensionDelegate::CreateLambda(
			[=](FMenuBuilder& MenuBuilder)
			{
				MenuBuilder.AddMenuEntry(
					FHyperlinkEditCommands::Get().GenerateEditLink,
					TEXT("CopyEditLink"),
					TAttribute<FText>(),
					TAttribute<FText>(),
					FSlateIcon(FStarshipCoreStyle::GetCoreStyle().GetStyleSetName(), TEXT("Icons.Link"))
				);
			}
		)
	);
	
	return Extender;
}

#undef LOCTEXT_NAMESPACE
#endif //WITH_EDITOR
