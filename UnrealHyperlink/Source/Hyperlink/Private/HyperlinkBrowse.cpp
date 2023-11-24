// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkBrowse.h"

#if WITH_EDITOR
  #include "AssetRegistry/IAssetRegistry.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#endif //WITH_EDITOR

FString UHyperlinkBrowse::GetDefinitionName() const
{
	return TEXT("browse");
}

void UHyperlinkBrowse::Initialize()
{
#if WITH_EDITOR
	FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };

	// Assets context menu
	FContentBrowserMenuExtender_SelectedAssets SelectedAssetsDelegate
	{
		FContentBrowserMenuExtender_SelectedAssets::CreateUObject(this, &UHyperlinkBrowse::OnExtendAssetContextMenu)
	};
	AssetContextMenuHandle = SelectedAssetsDelegate.GetHandle();
	ContentBrowser.GetAllAssetViewContextMenuExtenders().Emplace(MoveTemp(SelectedAssetsDelegate));

	// Folders context menu
	FContentBrowserMenuExtender_SelectedPaths SelectedPathsDelegate
	{
		FContentBrowserMenuExtender_SelectedPaths::CreateUObject(this, &UHyperlinkBrowse::OnExtendFolderContextMenu)
	};
	AssetContextMenuHandle = SelectedAssetsDelegate.GetHandle();
	ContentBrowser.GetAllAssetViewContextMenuExtenders().Emplace(MoveTemp(SelectedAssetsDelegate));
#endif //WITH_EDITOR
}

void UHyperlinkBrowse::Deinitialize()
{
	// TODO
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

#if WITH_EDITOR
TSharedRef<FExtender> UHyperlinkBrowse::OnExtendAssetContextMenu(const TArray<FAssetData>& SelectedAssets) const
{
	TSharedRef<FExtender> Extender{ MakeShared<FExtender>() };
	//Extender->AddMenuExtension()
	
	return Extender;
}

TSharedRef<FExtender> UHyperlinkBrowse::OnExtendFolderContextMenu(const TArray<FString>& SelectedFolders) const
{
	TSharedRef<FExtender> Extender{ MakeShared<FExtender>() };
	//Extender->AddMenuExtension()
	
	return Extender;
}
#endif //WITH_EDITOR
