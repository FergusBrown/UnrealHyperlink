// Copyright (c) 2023-2024 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

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
 * Hyperlink for browsing to an asset/folder in the content browser
 */
UCLASS()
class HYPERLINKEDITOR_API UHyperlinkBrowse : public UHyperlinkDefinition
{
	GENERATED_BODY()

public:
	virtual void Initialize() override;
	virtual void Deinitialize() override;

	virtual TSharedPtr<FJsonObject> GeneratePayload(const TArray<FString>& Args) const override;
	TSharedPtr<FJsonObject> GeneratePayloadFromPath(const FName& PackageOrFolderName) const;
	
	virtual void ExecutePayload(const TSharedRef<FJsonObject>& InPayload) override;
	
private:
	FDelegateHandle KeyboardShortcutHandle{};
	TSharedPtr<FUICommandList> BrowseCommands{};
};
