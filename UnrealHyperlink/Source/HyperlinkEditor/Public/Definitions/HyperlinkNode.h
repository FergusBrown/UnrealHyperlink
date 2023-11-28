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

USTRUCT()
struct FHyperlinkBlueprintPayload
{
	GENERATED_BODY()

	UPROPERTY()
	FName BlueprintPackageName{};

	UPROPERTY()
	FGuid GraphGuid{};

	UPROPERTY()
	FGuid NodeGuid{};
};

USTRUCT()
struct FHyperlinkMaterialPayload
{
	GENERATED_BODY()

	UPROPERTY()
	FName MaterialPackageName{};

	UPROPERTY()
	FGuid MaterialExpressionGuid{};

	UPROPERTY()
	int32 ExpressionX{};

	UPROPERTY()
	int32 ExpressionY{};
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
	virtual void Initialize() override;
	virtual void Deinitialize() override;
	
	virtual TSharedPtr<FJsonObject> GeneratePayload(const TArray<FString>& Args) const override;
	static TSharedPtr<FJsonObject> GenerateBlueprintPayload(const FName& AssetPackageName, const FGuid& GraphGuid,
		const FGuid& NodeGuid);
	static TSharedPtr<FJsonObject> GenerateMaterialPayload(const FName& AssetPackageName, const FGuid& NodeGuid,
		int32 NodeX, int32 NodeY);
	static TSharedPtr<FJsonObject> GenerateMaterialPayload(const UMaterial& InMaterial, const UEdGraphNode& InNode);
	
	virtual void ExecutePayload(const TSharedRef<FJsonObject>& InPayload) override;
	static void ExecuteBlueprintPayload(const FHyperlinkBlueprintPayload& InPayload);
	static void ExecuteMaterialPayload(const FHyperlinkMaterialPayload& InPayload);
	
private:
	static bool TryGetExtensionPoint(const UClass* Class, FName& OutExtensionPoint);

private:
	TSharedPtr<FUICommandList> NodeCommands{};
	FDelegateHandle NodeContextMenuHandle{};
	
	TWeakObjectPtr<const UEdGraph> ActiveGraph{ nullptr };
	TWeakObjectPtr<const UEdGraphNode> SelectedNode{ nullptr };
};
