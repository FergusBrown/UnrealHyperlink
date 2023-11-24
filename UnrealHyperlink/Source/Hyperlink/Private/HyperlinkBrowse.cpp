// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkBrowse.h"

#include "AssetRegistry/IAssetRegistry.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

FString UHyperlinkBrowse::GetDefinitionName() const
{
	return TEXT("browse");
}

void UHyperlinkBrowse::Initialize()
{
	// TODO
}

void UHyperlinkBrowse::Deinitialize()
{
	// TODO
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
