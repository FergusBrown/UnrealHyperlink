// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkNode.generated.h"

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

protected:
	virtual void ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments) override;
};
