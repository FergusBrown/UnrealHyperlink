// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkEdit.generated.h"

class FHyperlinkEditCommands : public TCommands<FHyperlinkEditCommands>
{
public:
	FHyperlinkEditCommands();
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> CopyEditLink{ nullptr };
};

/**
 * Hyperlink definition for edit links
 */
UCLASS()
class HYPERLINKEDITOR_API UHyperlinkEdit : public UHyperlinkDefinition
{
	GENERATED_BODY()

public:
	UHyperlinkEdit();
	
	virtual void Initialize() override;
	virtual void Deinitialize() override;

	virtual bool GenerateLink(FString& OutLink) const override;
	FString GenerateLinkFromPackageName(const FString& PackageName) const;

protected:
	virtual void ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments) override;

private:
	FDelegateHandle AssetContextMenuHandle{};
	FDelegateHandle KeyboardShortcutHandle{};
	TSharedPtr<FUICommandList> EditCommands{};
};
