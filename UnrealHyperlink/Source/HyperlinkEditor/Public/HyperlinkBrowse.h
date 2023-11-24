// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkBrowse.generated.h"

class FHyperlinkBrowseCommands : public TCommands<FHyperlinkBrowseCommands>
{
public:
	FHyperlinkBrowseCommands();
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> CopyBrowseLink{ nullptr };
	TSharedPtr<FUICommandInfo> CopyFolderLink{ nullptr };
};

/**
 * 
 */
UCLASS()
class HYPERLINKEDITOR_API UHyperlinkBrowse : public UHyperlinkDefinition
{
	GENERATED_BODY()
	
public:
	UHyperlinkBrowse();
	
	virtual void Initialize() override;
	virtual void Deinitialize() override;

	FString GenerateLink(const FString& PackageOrFolderName) const;

protected:
	virtual void ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments) override;

private:
	FDelegateHandle AssetContextMenuHandle{};
	FDelegateHandle FolderContextMenuHandle{};
	TSharedPtr<FUICommandList> BrowseCommands{};
};