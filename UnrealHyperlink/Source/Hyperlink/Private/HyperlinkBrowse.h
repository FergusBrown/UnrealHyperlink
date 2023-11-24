// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkBrowse.generated.h"

#if WITH_EDITOR
class FHyperlinkBrowseCommands : public TCommands<FHyperlinkBrowseCommands>
{
public:
	FHyperlinkBrowseCommands();
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> CopyBrowseLink{ nullptr };
	TSharedPtr<FUICommandInfo> CopyFolderLink{ nullptr };
};
#endif //WITH_EDITOR

/**
 * 
 */
UCLASS()
class UHyperlinkBrowse : public UHyperlinkDefinition
{
	GENERATED_BODY()
	
public:
	virtual FString GetDefinitionName() const override;
	virtual void Initialize() override;
	virtual void Deinitialize() override;

	FString GenerateLink(const FString& PackageOrFolderName) const;

protected:
	virtual void ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments) override;

#if WITH_EDITOR
private:
	FDelegateHandle AssetContextMenuHandle{};
	FDelegateHandle FolderContextMenuHandle{};
	TSharedPtr<FUICommandList> BrowseCommands{};
#endif //WITH_EDITOR
};
