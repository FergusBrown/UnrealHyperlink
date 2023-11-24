// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkBrowse.h"

#include "AssetRegistry/IAssetRegistry.h"
#include "ContentBrowserModule.h"
#include "Editor.h"
#include "HyperlinkUtils.h"
#include "IContentBrowserSingleton.h"
#include "Log.h"
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

UHyperlinkBrowse::UHyperlinkBrowse()
{
	DefinitionIdentifier = TEXT("Browse");
}

void UHyperlinkBrowse::Initialize()
{
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
					// The path will be an "virtual path" which is usually (always?) the internal path prefixed with "/All"
					// We need to convert this to the regular internal path
					const FString& VirtualPath{ SelectedFolders[0] };
					FString InternalPath;
					EContentBrowserPathType ConvertedType{ GEditor->GetEditorSubsystem<UContentBrowserDataSubsystem>()->TryConvertVirtualPath(VirtualPath, InternalPath) };
					if (ConvertedType == EContentBrowserPathType::Internal)
					{
						FPlatformApplicationMisc::ClipboardCopy(*GenerateLink(InternalPath));
					}
					else
					{
						UE_LOG(LogHyperlinkEditor, Display, TEXT("Failed to convert %s to an internal path, cannot create browse link."), *VirtualPath);
					}
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
}

void UHyperlinkBrowse::Deinitialize()
{
	FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };

	ContentBrowser.GetAllAssetViewContextMenuExtenders().RemoveAll(
		[=](const FContentBrowserMenuExtender_SelectedAssets& Delegate){ return Delegate.GetHandle() == AssetContextMenuHandle; });
	ContentBrowser.GetAllPathViewContextMenuExtenders().RemoveAll(
		[=](const FContentBrowserMenuExtender_SelectedPaths& Delegate){ return Delegate.GetHandle() == FolderContextMenuHandle; });
	FHyperlinkBrowseCommands::Unregister();
}

FString UHyperlinkBrowse::GenerateLink(const FString& PackageOrFolderName) const
{
	return GetHyperlinkBase() / PackageOrFolderName;
}

void UHyperlinkBrowse::ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments)
{
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
}
