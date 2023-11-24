// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "Definitions/HyperlinkDefinitionBlueprintBase.h"

void UHyperlinkDefinitionBlueprintBase::Initialize()
{
	InitializeImpl();
}

void UHyperlinkDefinitionBlueprintBase::Deinitialize()
{
	DeinitializeImpl();
}

bool UHyperlinkDefinitionBlueprintBase::GenerateLink(FString& OutLink) const
{
	return GenerateLinkImpl(OutLink);
}

void UHyperlinkDefinitionBlueprintBase::ExecuteExtractedArgs(const TArray<FString>& LinkArguments)
{
	ExecuteExtractedArgsImpl(LinkArguments);
}
