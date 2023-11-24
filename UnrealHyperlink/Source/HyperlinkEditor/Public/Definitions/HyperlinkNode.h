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
 * Works with blueprints and materials for now
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
	FString GenerateLink(const FString& AssetPackageName, const FGuid& GraphGuid, const FGuid& NodeGuid) const;

protected:
	virtual void ExecuteExtractedArgs(const TArray<FString>& LinkArguments) override;
	
private:
	/* Generation helpers */
	static bool TryGetExtensionPoint(const UClass* Class, FName& OutExtensionPoint);
	bool TryGetMaterialParams(const UMaterial& InMaterial, FString& OutPackageName, FGuid& OutGraphGuid,
		FGuid& OutNodeGuid) const;

	/* Execution helpers */
	static void ExecuteBlueprintLink(const UBlueprint& InBlueprint, const FGuid& InGraphGuid, const FGuid& InNodeGuid);
	static void ExecuteMaterialLink(const UObject& InMaterial, const FGuid& InNodeGuid);
private:
	TSharedPtr<FUICommandList> NodeCommands{};
	FDelegateHandle NodeContextMenuHandle{};
	
	TWeakObjectPtr<const UEdGraph> ActiveGraph{ nullptr };
	TWeakObjectPtr<const UEdGraphNode> SelectedNode{ nullptr };
};
