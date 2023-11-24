// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "Definitions/HyperlinkNode.h"

#include "BlueprintEditor.h"
#include "GraphEditorModule.h"
#include "HyperlinkFormat.h"
#include "HyperlinkUtils.h"
#include "Log.h"

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
	
	BodyPattern = FString::Printf(TEXT(R"((.*)%s(\S{32})%s(\S{32}))"), &FHyperlinkFormat::ArgSeparator, &FHyperlinkFormat::ArgSeparator);
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
		FGraphEditorModule::FGraphEditorMenuExtender_SelectedNode::CreateLambda([=](const TSharedRef<FUICommandList>,
			const UEdGraph* Graph, const UEdGraphNode* Node, const UEdGraphPin*, bool)
			{
				ActiveGraph = Graph;
				SelectedNode = Node;
				// Only support blueprint graphs for now.
				// TODO: Add support for the likes of material graph etc
				if (Cast<UBlueprint>(Graph->GetOuter()))
				{
					// TODO: current extension point doesn't work for all node types
					return FHyperlinkUtils::GetMenuExtender(TEXT("EdGraphSchemaNodeActions"), EExtensionHook::After,
						NodeCommands, FHyperlinkNodeCommands::Get().CopyNodeLink, TEXT("CopyNodeLink"));
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
	const bool bSuccess{ SelectedNode.IsValid() && ActiveGraph.IsValid() };
	
	if (bSuccess)
	{
		const FString AssetPackageName{ ActiveGraph->GetOuter()->GetPackage()->GetName() };
		const FGuid& GraphGuid{ ActiveGraph->GraphGuid };
		const FGuid& NodeGuid{ SelectedNode->NodeGuid };
		OutLink = GenerateLink(AssetPackageName, GraphGuid, NodeGuid);
	}
	else
	{
		UE_LOG(LogHyperlinkEditor, Display, TEXT("Cannot generate Node link: no graph editor node is selected."));
	}

	return bSuccess;
}

FString UHyperlinkNode::GenerateLink(const FString& AssetPackageName, const FGuid& GraphGuid, const FGuid& NodeGuid) const
{
	return GetHyperlinkBase() / AssetPackageName + FHyperlinkFormat::ArgSeparator + GraphGuid.ToString() +
		FHyperlinkFormat::ArgSeparator + NodeGuid.ToString();
}

void UHyperlinkNode::ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments)
{
	const FString& PackageName{ LinkArguments[1] };
	const FString& GraphGuidString{ LinkArguments[2] };
	const FString& NodeGuidString{ LinkArguments[3] };

	const FGuid GraphGuid{ FGuid(GraphGuidString) };
	const FGuid NodeGuid{ FGuid(NodeGuidString) };

	if (UBlueprint* const Blueprint{ Cast<UBlueprint>(FHyperlinkUtils::OpenEditorForAsset(PackageName)) })
	{
		TArray<UEdGraph*> AllGraphs{};
		Blueprint->GetAllGraphs(AllGraphs);
		
		if (UEdGraph** GraphPtr{ AllGraphs.FindByPredicate([&](const UEdGraph* const G){ return G->GraphGuid == GraphGuid; }) })
		{
			UEdGraph* const Graph{ *GraphPtr };
			IAssetEditorInstance* const AssetEditor{ GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(Blueprint, true) };
			FBlueprintEditor* const BlueprintEditor{ static_cast<FBlueprintEditor*>(AssetEditor) };
			const TSharedPtr<SGraphEditor> SlateEditor{ BlueprintEditor->OpenGraphAndBringToFront(Graph) };

			if (const TObjectPtr<UEdGraphNode>* NodePtr{ (*GraphPtr)->Nodes.FindByPredicate([&](const UEdGraphNode* const N){ return N->NodeGuid == NodeGuid; }) })
			{
				const TObjectPtr<UEdGraphNode> Node{ *NodePtr };
				// TODO: clear existing selection
				BlueprintEditor->AddToSelection(Node);
				SlateEditor->ZoomToFit(true);
			}
		}
	}
	// TODO: This needs to be handled differently for different asset types
}
