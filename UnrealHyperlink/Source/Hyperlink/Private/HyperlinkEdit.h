// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkEdit.generated.h"

/**
 * Hyperlink definition for edit links
 */
UCLASS()
class UHyperlinkEdit : public UHyperlinkDefinition
{
	GENERATED_BODY()
	
public:
	virtual FString GetDefinitionName() const override;
	virtual void Initialize() override;
	virtual void Deinitialize() override;

	FString GenerateLink(const FString& PackageName) const;

protected:
	virtual void ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments) override;
	
};
