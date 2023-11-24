// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkLevelActor.generated.h"

/**
 * Hyperlink for opening a level and focusing on an actor in the level
 */
UCLASS()
class HYPERLINKEDITOR_API UHyperlinkLevelActor : public UHyperlinkDefinition
{
	GENERATED_BODY()
	
public:
	UHyperlinkLevelActor();

	virtual void Initialize() override;
	virtual void Deinitialize() override;
	virtual bool GenerateLink(FString& OutLink) const override;

protected:
	virtual void ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments) override;
};
