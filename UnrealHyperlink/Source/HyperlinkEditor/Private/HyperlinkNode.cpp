// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkNode.h"

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

	// TODO: Should really have a regex for the GUID 
	BodyPattern = FString::Printf(TEXT("(.*)%s(.*)"), &FHyperlinkFormat::ArgSeparator);
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
			const UEdGraph*, const UEdGraphNode* Node, const UEdGraphPin*, bool)
			{
				SelectedNode = Node;
				return FHyperlinkUtils::GetMenuExtender(TEXT("EdGraphSchemaNodeActions"), EExtensionHook::After,
					NodeCommands, FHyperlinkNodeCommands::Get().CopyNodeLink, TEXT("CopyNodeLink"));
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
	const bool bSuccess{ SelectedNode.IsValid() && SelectedNode->GetGraph() };
	
	if (bSuccess)
	{
		const FString AssetPackageName{ SelectedNode->GetGraph()->GetOuter()->GetPackage()->GetName() };
		const FGuid& NodeGuid{ SelectedNode->NodeGuid };
		OutLink = GenerateLink(AssetPackageName, NodeGuid);
	}
	else
	{
		UE_LOG(LogHyperlinkEditor, Display, TEXT("Cannot generate Node link: no graph editor node is selected."));
	}

	return bSuccess;
}

FString UHyperlinkNode::GenerateLink(const FString& AssetPackageName, const FGuid& NodeGuid) const
{
	return GetHyperlinkBase() / AssetPackageName + FHyperlinkFormat::ArgSeparator + NodeGuid.ToString();
}

void UHyperlinkNode::ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments)
{
	const FString& PackageName{ LinkArguments[1] };
	const FString& GUID{ LinkArguments[2] };

	FHyperlinkUtils::OpenEditorForAsset(PackageName);

	// TODO
	// Need to ensure the graph editor is opened for the asset
	// This needs to be handled differently for different asset types
}
