// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkDefinitionBlueprintBase.generated.h"

/**
 * Base class for implementing hyperlink definitions with blueprints
 */
UCLASS(Abstract, Blueprintable)
class HYPERLINK_API UHyperlinkDefinitionBlueprintBase : public UHyperlinkDefinition
{
	GENERATED_BODY()

public:
	virtual void Initialize() override;
	virtual void Deinitialize() override;
	virtual bool GenerateLink(FString& OutLink) const override;

protected:
	virtual void ExecuteExtractedArgs(const TArray<FString>& LinkArguments) override;

	// Blueprint Implementable versions
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Initialize"))
	void InitializeImpl();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Deinitialize"))
	void DeinitializeImpl();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="GenerateLink"))
	bool GenerateLinkImpl(FString& OutLink) const;

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="ExecuteExtractedArgs"))
	void ExecuteExtractedArgsImpl(const TArray<FString>& LinkArguments);
};
