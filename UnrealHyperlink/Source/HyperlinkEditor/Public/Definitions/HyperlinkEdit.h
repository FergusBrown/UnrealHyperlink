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
 * Hyperlink for opening the editor for an asset
 */
UCLASS()
class HYPERLINKEDITOR_API UHyperlinkEdit : public UHyperlinkDefinition
{
	GENERATED_BODY()

public:
	virtual void Initialize() override;
	virtual void Deinitialize() override;

	virtual TSharedPtr<FJsonObject> GeneratePayload() const override;
	TSharedPtr<FJsonObject> GeneratePayloadFromPackageName(const FName& PackageName) const;
	
	virtual void ExecutePayload(const TSharedRef<FJsonObject>& InPayload) override;
	
private:
	FDelegateHandle KeyboardShortcutHandle{};
	TSharedPtr<FUICommandList> EditCommands{};
};
