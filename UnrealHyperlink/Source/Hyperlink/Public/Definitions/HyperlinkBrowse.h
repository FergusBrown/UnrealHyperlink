// Copyright (c) 2023-2024 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

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
 * Hyperlink for browsing to an asset/folder in the content browser
 */
UCLASS()
class HYPERLINK_API UHyperlinkBrowse : public UHyperlinkDefinition
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual void Initialize() override;
	virtual void Deinitialize() override;
#endif //WITH_EDITOR
	
	virtual TSharedPtr<FJsonObject> GeneratePayload(const TArray<FString>& Args) const override;
	TSharedPtr<FJsonObject> GeneratePayloadFromPath(const FName& PackageOrFolderName) const;

#if WITH_EDITOR
	virtual void ExecutePayload(const TSharedRef<FJsonObject>& InPayload) override;
	
private:
	FDelegateHandle KeyboardShortcutHandle{};
	TSharedPtr<FUICommandList> BrowseCommands{};
#endif //WITH_EDITOR
};
