// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "Definitions/HyperlinkBrowse.h"

#include "AssetRegistry/IAssetRegistry.h"
#include "ContentBrowserModule.h"
#include "Editor.h"
#include "HyperlinkUtils.h"
#include "IContentBrowserSingleton.h"
#include "Log.h"

#define LOCTEXT_NAMESPACE "HyperlinkBrowse"

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
	UI_COMMAND(CopyBrowseLink, "Copy Browse Link", "Copy a link to browse to the selected asset", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Alt | EModifierKey::Shift, EKeys::C));
	UI_COMMAND(CopyFolderLink, "Copy Browse Link", "Copy a link to browse to the selected folder", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Alt | EModifierKey::Shift, EKeys::C));
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
		FExecuteAction::CreateUObject(this, &UHyperlinkDefinition::CopyLink));
	
	BrowseCommands->MapAction(
		FHyperlinkBrowseCommands::Get().CopyFolderLink,
		FExecuteAction::CreateUObject(this, &UHyperlinkDefinition::CopyLink));
	
	FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };

	// Context menu extensions
	FHyperlinkUtils::ExtendToolMenuSection(TEXT("ContentBrowser.AssetContextMenu"), TEXT("CommonAssetActions"),
	BrowseCommands, FHyperlinkBrowseCommands::Get().CopyBrowseLink);

	FHyperlinkUtils::ExtendToolMenuSection(TEXT("ContentBrowser.FolderContextMenu"), TEXT("PathViewFolderOptions"),
	BrowseCommands, FHyperlinkBrowseCommands::Get().CopyBrowseLink);

	// Keyboard shortcut command
	// Note that the keyboard shortcut will only be registered if applied on startup because of the way content
	// browser commands work
	FContentBrowserCommandExtender CommandExtender
	{
		FContentBrowserCommandExtender::CreateLambda(
			[this](TSharedRef<FUICommandList> CommandList, FOnContentBrowserGetSelection GetSelectionDelegate)
			{
				CommandList->Append(BrowseCommands.ToSharedRef());
			}
		)
	};
	KeyboardShortcutHandle = CommandExtender.GetHandle();
	ContentBrowser.GetAllContentBrowserCommandExtenders().Emplace(MoveTemp(CommandExtender));
}

void UHyperlinkBrowse::Deinitialize()
{
	FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };
	ContentBrowser.GetAllContentBrowserCommandExtenders().RemoveAll(
		[=](const FContentBrowserCommandExtender& Delegate){ return Delegate.GetHandle() == KeyboardShortcutHandle; });
	
	FHyperlinkBrowseCommands::Unregister();
}

bool UHyperlinkBrowse::GenerateLink(FString& OutLink) const
{
	const FContentBrowserModule& ContentBrowser{ FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")) };
	TArray<FAssetData> SelectedAssets{};
	ContentBrowser.Get().GetSelectedAssets(SelectedAssets);
	bool bSuccess{ SelectedAssets.Num() > 0 };
	if (bSuccess)
	{
		OutLink = GenerateLinkFromPath(SelectedAssets[0].PackageName.ToString());
	}

	if (!bSuccess)
	{
		TArray<FString> SelectedFolders{};
		ContentBrowser.Get().GetSelectedFolders(SelectedFolders);
		if (SelectedFolders.Num() > 0)
		{
			// The path will be an "virtual path" which is usually (always?) the internal path prefixed with "/All"
			// We need to convert this to the regular internal path
			const FString& VirtualPath{ SelectedFolders[0] };
			FString InternalPath;
			const EContentBrowserPathType ConvertedType{ GEditor->GetEditorSubsystem<UContentBrowserDataSubsystem>()->TryConvertVirtualPath(VirtualPath, InternalPath) };
			if (ConvertedType == EContentBrowserPathType::Internal)
			{
				OutLink = GenerateLinkFromPath(InternalPath);
				bSuccess = true;
			}
			else
			{
				UE_LOG(LogHyperlinkEditor, Error, TEXT("Failed to convert %s to an internal path, cannot create browse link."), *VirtualPath);
			}
		}
		else
		{
			// Make folder link with current path
			const FContentBrowserItemPath CurrentPath{ ContentBrowser.Get().GetCurrentPath() };
			if (CurrentPath.HasInternalPath())
			{
				OutLink = GenerateLinkFromPath(CurrentPath.GetInternalPathString());
				bSuccess = true;
			}
			else
			{
				UE_LOG(LogHyperlinkEditor, Error, TEXT("Cannot create browse link at invalid path."));
			}
		}
	}
	
	return bSuccess;
}

FString UHyperlinkBrowse::GenerateLinkFromPath(const FString& PackageOrFolderName) const
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
