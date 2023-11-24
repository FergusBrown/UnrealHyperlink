// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "Definitions/HyperlinkNode.h"

#include "BlueprintEditor.h"
#include "GraphEditorModule.h"
#include "HyperlinkFormat.h"
#include "HyperlinkUtility.h"
#include "IMaterialEditor.h"
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

UHyperlinkNode::UHyperlinkNode()
{
	DefinitionIdentifier = TEXT("Node");

	// TODO: make final group optional (materials only need the node GUID)
	BodyPattern = FString::Printf(TEXT(R"((.*)%s(\S{32})%s(\S{32}))"),
		&FHyperlinkFormat::ArgSeparator, &FHyperlinkFormat::ArgSeparator);
}

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
			[=](const TSharedRef<FUICommandList>&,
			const UEdGraph* Graph, const UEdGraphNode* Node, const UEdGraphPin*, bool)
			{
				ActiveGraph = Graph;
				SelectedNode = Node;
				// Only support blueprint and material graphs for now.
				const UClass* const OuterClass{ Graph->GetOuter()->GetClass() };
				FName ExtensionPoint{};
				if (TryGetExtensionPoint(OuterClass, ExtensionPoint))
				{
					return UHyperlinkUtility::GetMenuExtender(ExtensionPoint,
						EExtensionHook::After, NodeCommands,
						FHyperlinkNodeCommands::Get().CopyNodeLink, TEXT("CopyNodeLink"));
				}
				else
				{
					return TSharedRef<FExtender>();
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
		[=](const FGraphEditorModule::FGraphEditorMenuExtender_SelectedNode& Delegate){ return Delegate.GetHandle() == NodeContextMenuHandle; });
	
	FHyperlinkNodeCommands::Unregister();
}

bool UHyperlinkNode::GenerateLink(FString& OutLink) const
{
	const bool bGraphObjectsValid{ SelectedNode.IsValid() && ActiveGraph.IsValid() };
	
	if (bGraphObjectsValid)
	{
		UObject* const AssetObject{ ActiveGraph->GetOuter() };
		
		FString AssetPackageName{};
		FGuid GraphGuid{};
		FGuid NodeGuid{};
		bool bLinkParamsFound{ false };
		
		// Handle material and material functions differently
		if (const UMaterial* const Material{ Cast<UMaterial>( AssetObject ) })
		{
			bLinkParamsFound = TryGetMaterialParams(*Material, AssetPackageName, GraphGuid, NodeGuid);
		}
		else if (const UMaterialFunction* const MaterialFunction{ Cast<UMaterialFunction>( AssetObject ) })
		{
			TConstArrayView<TObjectPtr<UMaterialExpression>> MaterialExpressions{ MaterialFunction->GetExpressions() };
			// TODO
		}
		else
		{
			AssetPackageName = AssetObject->GetPackage()->GetName();
			GraphGuid = ActiveGraph->GraphGuid;
			NodeGuid = SelectedNode->NodeGuid;
			bLinkParamsFound = true;
		}
			
		if (bLinkParamsFound)
		{
			OutLink = GenerateLink(AssetPackageName, GraphGuid, NodeGuid);
		}
		else
		{
			UE_LOG(LogHyperlinkEditor, Display, TEXT("Cannot generate Node link: could not find package name, graph or node GUID."));
		}
	}
	else
	{
		UE_LOG(LogHyperlinkEditor, Display, TEXT("Cannot generate Node link: no graph editor node is selected."));
	}

	return bGraphObjectsValid;
}

FString UHyperlinkNode::GenerateLink(const FString& AssetPackageName, const FGuid& GraphGuid, const FGuid& NodeGuid) const
{
	return GetHyperlinkBase() / AssetPackageName + FHyperlinkFormat::ArgSeparator + GraphGuid.ToString() +
		FHyperlinkFormat::ArgSeparator + NodeGuid.ToString();
}

void UHyperlinkNode::ExecuteExtractedArgs(const TArray<FString>& LinkArguments)
{
	const FString& PackageName{ LinkArguments[1] };
	const FString& GraphGuidString{ LinkArguments[2] };
	const FString& NodeGuidString{ LinkArguments[3] };

	const FGuid GraphGuid{ FGuid(GraphGuidString) };
	const FGuid NodeGuid{ FGuid(NodeGuidString) };

	const UObject* const EditedObject{ UHyperlinkUtility::OpenEditorForAsset(PackageName) };
	if (const UBlueprint* const Blueprint{ Cast<UBlueprint>(EditedObject) })
	{
		ExecuteBlueprintLink(*Blueprint, GraphGuid, NodeGuid);
	}
	else if (const UMaterial* const Material{ Cast<UMaterial>(EditedObject) })
	{
		ExecuteMaterialLink(*Material, NodeGuid);
	}
	else
	{
		UE_LOG(LogHyperlinkEditor, Error, TEXT("Failed to execute node link: unsupported asset type"));
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

bool UHyperlinkNode::TryGetMaterialParams(const UMaterial& InMaterial, FString& OutPackageName, FGuid& OutGraphGuid,
	FGuid& OutNodeGuid) const
{
	bool bResult{ false };

	const TConstArrayView<TObjectPtr<UMaterialExpression>> MaterialExpressions{ InMaterial.GetExpressions() };
	const TObjectPtr<UMaterialExpression>* const ExpressionPtr{ MaterialExpressions.FindByPredicate(
		[=](const TObjectPtr<UMaterialExpression> Expression)
		{
			return Expression->GraphNode == SelectedNode;
		}
	) };
	if (ExpressionPtr)
	{
		const TObjectPtr<const UMaterialExpression> MaterialExpression{ *ExpressionPtr };
		OutGraphGuid = MaterialExpression->MaterialExpressionGuid;
		OutNodeGuid = MaterialExpression->MaterialExpressionGuid;

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
				OutPackageName = (*MaterialPtr)->GetPackage()->GetName();
				bResult = true;
			}
		}
	}	
	
	return bResult;
}

void UHyperlinkNode::ExecuteBlueprintLink(const UBlueprint& InBlueprint, const FGuid& InGraphGuid,
	const FGuid& InNodeGuid)
{
	TArray<UEdGraph*> AllGraphs{};
	InBlueprint.GetAllGraphs(AllGraphs);
		
	if (UEdGraph** GraphPtr{ AllGraphs.FindByPredicate([&](const UEdGraph* const G){ return G->GraphGuid == InGraphGuid; }) })
	{
		UEdGraph* const Graph{ *GraphPtr };
		const TSharedPtr<FBlueprintEditor> BlueprintEditor
			{ StaticCastSharedPtr<FBlueprintEditor>(FToolkitManager::Get().FindEditorForAsset(&InBlueprint)) };
		if (BlueprintEditor.IsValid())
		{
			const TSharedPtr<SGraphEditor> SlateEditor{ BlueprintEditor->OpenGraphAndBringToFront(Graph) };

			if (const TObjectPtr<UEdGraphNode>* NodePtr{ (*GraphPtr)->Nodes.FindByPredicate(
				[&](const UEdGraphNode* const N){ return N->NodeGuid == InNodeGuid; }) })
			{
				BlueprintEditor->AddToSelection(*NodePtr);
				SlateEditor->ZoomToFit(true);
			}
		}
	}
}

void UHyperlinkNode::ExecuteMaterialLink(const UMaterial& InMaterial, const FGuid& InNodeGuid)
{
	const TSharedPtr<IMaterialEditor> MaterialEditor
		{ StaticCastSharedPtr<IMaterialEditor>(FToolkitManager::Get().FindEditorForAsset(&InMaterial)) };

	if (MaterialEditor.IsValid())
	{
		const UMaterial* const PreviewMaterial{ Cast<UMaterial>(MaterialEditor->GetMaterialInterface()) };
	
		const TConstArrayView<TObjectPtr<UMaterialExpression>> MaterialExpressions{ PreviewMaterial->GetExpressions() };
		const TObjectPtr<UMaterialExpression>* const ExpressionPtr{ MaterialExpressions.FindByPredicate(
			[=](const TObjectPtr<UMaterialExpression> Expression)
			{
				return Expression->MaterialExpressionGuid == InNodeGuid;
			}
		) };

		if (ExpressionPtr)
		{
			MaterialEditor->JumpToExpression((*ExpressionPtr));
		}
	}
}
