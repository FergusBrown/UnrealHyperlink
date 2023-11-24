// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkEdit.h"

#if WITH_EDITOR
#include "AssetViewUtils.h"
#include "ContentBrowserModule.h"
#include "HyperlinkUtils.h"
#endif //WITH_EDITOR

FString UHyperlinkEdit::GetDefinitionName() const
{
	return TEXT("edit");
}

void UHyperlinkEdit::Initialize()
{
#if WITH_EDITOR
	FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };

	// Assets context menu
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
	// TODO
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
	//Extender->AddMenuExtension()
	
	return Extender;
}
#endif //WITH_EDITOR
