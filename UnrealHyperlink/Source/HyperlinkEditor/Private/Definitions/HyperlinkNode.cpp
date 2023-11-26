// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "Definitions/HyperlinkNode.h"

#include "BlueprintEditor.h"
#include "GraphEditorModule.h"
#include "HyperlinkUtility.h"
#include "IMaterialEditor.h"
#include "JsonObjectConverter.h"
#include "Log.h"
#include "Toolkits/ToolkitManager.h"

#define LOCTEXT_NAMESPACE "HyperlinkNode"

FHyperlinkNodeCommands::FHyperlinkNodeCommands()
	: TCommands<FHyperlinkNodeCommands>(
		TEXT("HyperlinkNode"),
		NSLOCTEXT("Contexts", "HyperlinkNode", "Hyperlink Node"),
		NAME_None,
		FAppStyle::GetAppStyleSetName())
{
}

void FHyperlinkNodeCommands::RegisterCommands()
{
	UI_COMMAND(CopyNodeLink, "Copy Node Link", "Copy a link to the selected node in the graph editor",
		EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE

void UHyperlinkNode::Initialize()
{
	FHyperlinkNodeCommands::Register();
	NodeCommands = MakeShared<FUICommandList>();
	NodeCommands->MapAction(
		FHyperlinkNodeCommands::Get().CopyNodeLink,
		FExecuteAction::CreateUObject(this, &UHyperlinkDefinition::CopyLink)
	);
	
	FGraphEditorModule& GraphEditor{ FModuleManager::LoadModuleChecked<FGraphEditorModule>(TEXT("GraphEditor")) };
	// TODO: See if it's possible to add keyboard shortcut to graph editor. Would have to be added SGraphEditor on creation
	FGraphEditorModule::FGraphEditorMenuExtender_SelectedNode SelectedNodesDelegate
	{
		FGraphEditorModule::FGraphEditorMenuExtender_SelectedNode::CreateLambda(
			[this](const TSharedRef<FUICommandList>&,
			const UEdGraph* Graph, const UEdGraphNode* Node, const UEdGraphPin*, bool)
			{
				ActiveGraph = Graph;
				SelectedNode = Node;
				// Only support blueprint and material graphs for now.
				const UClass* const OuterClass{ Graph->GetOuter()->GetClass() };
				FName ExtensionPoint{};
				if (TryGetExtensionPoint(OuterClass, ExtensionPoint))
				{
					return FHyperlinkUtility::GetMenuExtender(ExtensionPoint,
						EExtensionHook::After, NodeCommands,
						FHyperlinkNodeCommands::Get().CopyNodeLink, TEXT("CopyNodeLink"));
				}
				else
				{
					return MakeShared<FExtender>();
				}
			})
	};

	NodeContextMenuHandle = SelectedNodesDelegate.GetHandle();
	GraphEditor.GetAllGraphEditorContextMenuExtender().Emplace(MoveTemp(SelectedNodesDelegate));
}

void UHyperlinkNode::Deinitialize()
{
	FGraphEditorModule& GraphEditor{ FModuleManager::LoadModuleChecked<FGraphEditorModule>(TEXT("GraphEditor")) };
	GraphEditor.GetAllGraphEditorContextMenuExtender().RemoveAll(
		[this](const FGraphEditorModule::FGraphEditorMenuExtender_SelectedNode& Delegate){ return Delegate.GetHandle() == NodeContextMenuHandle; });
	
	FHyperlinkNodeCommands::Unregister();
}

TSharedPtr<FJsonObject> UHyperlinkNode::GeneratePayload() const
{
	TSharedPtr<FJsonObject> Payload{ nullptr };

	if (SelectedNode.IsValid() && ActiveGraph.IsValid())
	{
		UObject* const AssetObject{ ActiveGraph->GetOuter() };
		
		// Handle material and material functions differently
		if (const UMaterial* const Material{ Cast<UMaterial>(AssetObject) })
		{
			Payload = GenerateMaterialPayload(*Material, *SelectedNode);
		}
		else // UBlueprint
		{
			Payload = GenerateBlueprintPayload(AssetObject->GetPackage()->GetFName(), ActiveGraph->GraphGuid,
				SelectedNode->NodeGuid);
		}
	}
	else
	{
		UE_LOG(LogHyperlinkEditor, Display, TEXT("Cannot generate Node link: no graph editor node is selected."));
	}

	return Payload;
}

TSharedPtr<FJsonObject> UHyperlinkNode::GenerateBlueprintPayload(const FName& AssetPackageName, const FGuid& GraphGuid,
																 const FGuid& NodeGuid)
{
	const FHyperlinkBlueprintPayload PayloadStruct
	{
		AssetPackageName,
		GraphGuid,
		NodeGuid
	};
	return FJsonObjectConverter::UStructToJsonObject(PayloadStruct);
}

TSharedPtr<FJsonObject> UHyperlinkNode::GenerateMaterialPayload(const FName& AssetPackageName, const FGuid& NodeGuid,
                                                                const int32 NodeX, const int32 NodeY)
{
	const FHyperlinkMaterialPayload PayloadStruct
	{
		AssetPackageName,
		NodeGuid,
		NodeX,
		NodeY
	};
	return FJsonObjectConverter::UStructToJsonObject(PayloadStruct);
}

TSharedPtr<FJsonObject> UHyperlinkNode::GenerateMaterialPayload(const UMaterial& InMaterial, const UEdGraphNode& InNode)
{
	TSharedPtr<FJsonObject> Payload{ nullptr };

	const TConstArrayView<TObjectPtr<UMaterialExpression>> MaterialExpressions{ InMaterial.GetExpressions() };
	const TObjectPtr<UMaterialExpression>* const ExpressionPtr{ MaterialExpressions.FindByPredicate(
		[&](const TObjectPtr<UMaterialExpression> Expression)
		{
			return Expression->GraphNode == &InNode;
		})};
	if (ExpressionPtr)
	{
		// Now find asset package name. We need to make sure we get the package name from the material asset
		// instead of duplicate material created for the material editor
		const TSharedPtr<FAssetEditorToolkit> MaterialEditor{
			StaticCastSharedPtr<FAssetEditorToolkit>(FToolkitManager::Get().FindEditorForAsset(&InMaterial)) };
		if (MaterialEditor.IsValid())
		{
			const TArray<UObject*>* EditedObjects{ MaterialEditor->GetObjectsCurrentlyBeingEdited() };
			const UObject* const * const MaterialPtr{ EditedObjects->FindByPredicate([=](const UObject* Object)
				{
					return Object->IsAsset();
				}
			) };
			
			if (MaterialPtr)
			{
				const TObjectPtr<const UMaterialExpression> MaterialExpression{ *ExpressionPtr };
				const TObjectPtr<const UObject> Material{ *MaterialPtr };
				Payload = GenerateMaterialPayload(Material->GetPackage()->GetFName(),
					MaterialExpression->MaterialExpressionGuid, MaterialExpression->MaterialExpressionEditorX,
					MaterialExpression->MaterialExpressionEditorY);
			}
		}
	}	
	
	return Payload;
}

void UHyperlinkNode::ExecutePayload(const TSharedRef<FJsonObject>& InPayload)
{
	if (FHyperlinkBlueprintPayload BlueprintPayload{};
		FJsonObjectConverter::JsonObjectToUStruct(InPayload, &BlueprintPayload, 0, 0, true))
	{
		ExecuteBlueprintPayload(BlueprintPayload);
	}
	else if (FHyperlinkMaterialPayload MaterialPayload{};
			 FJsonObjectConverter::JsonObjectToUStruct(InPayload, &MaterialPayload, 0, 0, true))
	{
		ExecuteMaterialPayload(MaterialPayload);
	}
	else
	{
		UE_LOG(LogHyperlinkEditor, Error, TEXT("Failed to execute node link: unsupported payload"));
	}
}

void UHyperlinkNode::ExecuteBlueprintPayload(const FHyperlinkBlueprintPayload& InPayload)
{
	const UObject* const EditedObject{ FHyperlinkUtility::OpenEditorForAsset(InPayload.BlueprintPackageName) };
	if (const UBlueprint* const Blueprint{ Cast<UBlueprint>(EditedObject) })
	{
		TArray<UEdGraph*> AllGraphs{};
		Blueprint->GetAllGraphs(AllGraphs);
		
		if (UEdGraph** GraphPtr{ AllGraphs.FindByPredicate([&](const UEdGraph* const G)
			{ return G->GraphGuid == InPayload.GraphGuid; }) })
		{
			UEdGraph* const Graph{ *GraphPtr };
			const TSharedPtr<FBlueprintEditor> BlueprintEditor
				{ StaticCastSharedPtr<FBlueprintEditor>(FToolkitManager::Get().FindEditorForAsset(Blueprint)) };
			if (BlueprintEditor.IsValid())
			{
				const TSharedPtr<SGraphEditor> SlateEditor{ BlueprintEditor->OpenGraphAndBringToFront(Graph) };

				if (const TObjectPtr<UEdGraphNode>* NodePtr{ (*GraphPtr)->Nodes.FindByPredicate(
					[&](const UEdGraphNode* const N){ return N->NodeGuid == InPayload.NodeGuid; }) })
				{
					BlueprintEditor->AddToSelection(*NodePtr);
					SlateEditor->ZoomToFit(true);
				}
			}
		}
	}
}

void UHyperlinkNode::ExecuteMaterialPayload(const FHyperlinkMaterialPayload& InPayload)
{
	const UObject* const EditedObject{ FHyperlinkUtility::OpenEditorForAsset(InPayload.MaterialPackageName) };
	if (EditedObject->IsA<UMaterial>() || EditedObject->IsA<UMaterialFunction>())
	{
		const TSharedPtr<IMaterialEditor> MaterialEditor
			{ StaticCastSharedPtr<IMaterialEditor>(FToolkitManager::Get().FindEditorForAsset(EditedObject)) };

		if (MaterialEditor.IsValid())
		{
			const UMaterial* const PreviewMaterial{ Cast<UMaterial>(MaterialEditor->GetMaterialInterface()) };
	
			const TConstArrayView<TObjectPtr<UMaterialExpression>> MaterialExpressions{ PreviewMaterial->GetExpressions() };
			TArray<TObjectPtr<UMaterialExpression>> Expressions{ MaterialExpressions.FilterByPredicate(
				[=](const TObjectPtr<UMaterialExpression> Expression)
				{
					return Expression->MaterialExpressionGuid == InPayload.MaterialExpressionGuid;
				}
			) };

			if (Expressions.Num() > 0)
			{
				// Find closest to the provided coords
				Expressions.Sort(
					[&](const UMaterialExpression& A, const UMaterialExpression& B)
					{
						auto SqDistFromNode = [&](const UMaterialExpression& Expression)
						{
							const int32 dx{ Expression.MaterialExpressionEditorX - InPayload.ExpressionX };
							const int32 dy{ Expression.MaterialExpressionEditorY - InPayload.ExpressionY };
							return dx * dx + dy * dy;
						};

						return SqDistFromNode(A) < SqDistFromNode(B);
					});

				MaterialEditor->JumpToExpression(Expressions[0]);
			}
		}
	}
}

bool UHyperlinkNode::TryGetExtensionPoint(const UClass* const Class, FName& OutExtensionPoint)
{
	bool bResult{ true };

	if (Class->IsChildOf<UBlueprint>())
	{
		OutExtensionPoint = TEXT("EdGraphSchemaNodeActions");
	}
	else if (Class->IsChildOf<UMaterialInterface>() || Class->IsChildOf<UMaterialFunctionInterface>())
	{
		OutExtensionPoint = TEXT("MaterialSchemaNodeActions");
	}
	else
	{
		bResult = false;
	}
	
	return bResult;
}
