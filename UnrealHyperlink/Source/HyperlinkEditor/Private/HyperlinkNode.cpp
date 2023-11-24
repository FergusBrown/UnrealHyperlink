// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkNode.h"

#include "HyperlinkFormat.h"
#include "HyperlinkUtils.h"

UHyperlinkNode::UHyperlinkNode()
{
	DefinitionIdentifier = TEXT("Node");

	// TODO: Should really have a regex for the GUID 
	BodyPattern = FString::Printf(TEXT("(.*)%s(.*)"), &FHyperlinkFormat::ArgSeparator);
}

void UHyperlinkNode::Initialize()
{

}

void UHyperlinkNode::Deinitialize()
{

}

bool UHyperlinkNode::GenerateLink(FString& OutLink) const
{
	bool bSuccess{ false };

	
	
	return bSuccess;
}

void UHyperlinkNode::ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments)
{
	const FString& PackageName{ LinkArguments[1] };
	const FString& GUID{ LinkArguments[2] };

	FHyperlinkUtils::OpenEditorForAsset(PackageName);

	// Need to ensure the graph editor is opened for the asset
	// This needs to be handled differently for different asset types
}
