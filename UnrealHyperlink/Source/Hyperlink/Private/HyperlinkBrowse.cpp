// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkBrowse.h"

#if WITH_EDITOR
#include "AssetRegistry/IAssetRegistry.h"
#include "ContentBrowserModule.h"
#include "HyperlinkUtils.h"
#include "IContentBrowserSingleton.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "HyperlinkEdit"

FHyperlinkBrowseCommands::FHyperlinkBrowseCommands()
	: TCommands<FHyperlinkBrowseCommands>(
		TEXT("HyperlinkBrowse"),
		NSLOCTEXT("Contexts", "HyperlinkBrowse", "Hyperlink Browse"),
		NAME_None,
		FAppStyle::GetAppStyleSetName())
{
}

void FHyperlinkBrowseCommands::RegisterCommands()
{
	UI_COMMAND(CopyBrowseLink, "Copy Browse Link", "Copy a link to browse this asset", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CopyFolderLink, "Copy Browse Link", "Copy a link to browse this folder", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
#endif //WITH_EDITOR

UHyperlinkBrowse::UHyperlinkBrowse()
{
	DefinitionIdentifier = TEXT("Browse");
}

void UHyperlinkBrowse::Initialize()
{
#if WITH_EDITOR
	FHyperlinkBrowseCommands::Register();
	BrowseCommands = MakeShared<FUICommandList>();
	BrowseCommands->MapAction(
		FHyperlinkBrowseCommands::Get().CopyBrowseLink,
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
	
	BrowseCommands->MapAction(
	FHyperlinkBrowseCommands::Get().CopyFolderLink,
	FExecuteAction::CreateLambda([=]()
			{
				const FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };
				TArray<FString> SelectedFolders{};
				ContentBrowser.Get().GetSelectedFolders(SelectedFolders);
				if (SelectedFolders.Num() > 0)
				{
					// TODO: check where this /All prefix has come from
					SelectedFolders[0].RemoveFromStart(TEXT("/All")); // This does not remove the prefix
					FPlatformApplicationMisc::ClipboardCopy(*GenerateLink(SelectedFolders[0]));
				}
			}
		)
	);
	
	FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };
	
	FContentBrowserMenuExtender_SelectedAssets SelectedAssetsDelegate
	{
		FContentBrowserMenuExtender_SelectedAssets::CreateLambda([=](const TArray<FAssetData>&)
		{
			return FHyperlinkUtils::GetMenuExtender(TEXT("CommonAssetActions"), EExtensionHook::After, BrowseCommands, FHyperlinkBrowseCommands::Get().CopyBrowseLink, TEXT("CopyBrowseLink"));
		})
	};
	AssetContextMenuHandle = SelectedAssetsDelegate.GetHandle();
	ContentBrowser.GetAllAssetViewContextMenuExtenders().Emplace(MoveTemp(SelectedAssetsDelegate));

	FContentBrowserMenuExtender_SelectedPaths SelectedFoldersDelegate
	{
		FContentBrowserMenuExtender_SelectedPaths::CreateLambda([=](const TArray<FString>&)
		{
			return FHyperlinkUtils::GetMenuExtender(TEXT("PathViewFolderOptions"), EExtensionHook::After, BrowseCommands, FHyperlinkBrowseCommands::Get().CopyFolderLink, TEXT("CopyBrowseLink"));
		})
	};
	FolderContextMenuHandle = SelectedFoldersDelegate.GetHandle();
	ContentBrowser.GetAllPathViewContextMenuExtenders().Emplace(MoveTemp(SelectedFoldersDelegate));
#endif //WITH_EDITOR
}

void UHyperlinkBrowse::Deinitialize()
{
#if WITH_EDITOR
	FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };

	ContentBrowser.GetAllAssetViewContextMenuExtenders().RemoveAll(
		[=](const FContentBrowserMenuExtender_SelectedAssets& Delegate){ return Delegate.GetHandle() == AssetContextMenuHandle; });
	ContentBrowser.GetAllPathViewContextMenuExtenders().RemoveAll(
		[=](const FContentBrowserMenuExtender_SelectedPaths& Delegate){ return Delegate.GetHandle() == FolderContextMenuHandle; });
	FHyperlinkBrowseCommands::Unregister();
#endif //WITH_EDITOR
}

FString UHyperlinkBrowse::GenerateLink(const FString& PackageOrFolderName) const
{
	return GetHyperlinkBase() / PackageOrFolderName;
}

void UHyperlinkBrowse::ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments)
{
#if WITH_EDITOR
  	TArray<FAssetData> LinkAssetData{};
	IAssetRegistry::Get()->GetAssetsByPackageName(FName(LinkArguments[0]), LinkAssetData);

	const FContentBrowserModule& ContentBrowserModule{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };
	if (LinkAssetData.Num() > 0)
	{
		// Treat as asset
		ContentBrowserModule.Get().SyncBrowserToAssets(LinkAssetData);
	}
	else
	{
		// Treat as folder
		ContentBrowserModule.Get().SyncBrowserToFolders(LinkArguments);
	}
#endif //WITH_EDITOR
}
