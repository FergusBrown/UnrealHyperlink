// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkNode.generated.h"

class FHyperlinkNodeCommands : public TCommands<FHyperlinkNodeCommands>
{
public:
	FHyperlinkNodeCommands();
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> CopyNodeLink{ nullptr };
};

/**
 * Hyperlink for opening a graph editor and focusing on a node
 */
UCLASS()
class HYPERLINKEDITOR_API UHyperlinkNode : public UHyperlinkDefinition
{
	GENERATED_BODY()

public:
	UHyperlinkNode();
	
	virtual void Initialize() override;
	virtual void Deinitialize() override;
	virtual bool GenerateLink(FString& OutLink) const override;
	FString GenerateLink(const FString& AssetPackageName, const FGuid& NodeGuid) const;

protected:
	virtual void ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments) override;

private:
	TSharedPtr<FUICommandList> NodeCommands{};
	FDelegateHandle NodeContextMenuHandle{};
	
	TWeakObjectPtr<const UEdGraphNode> SelectedNode{ nullptr };
};
