// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkDefinitionBlueprintBase.generated.h"

/**
 * Base class for implementing hyperlink definitions with blueprints
 */
UCLASS(Abstract, Blueprintable)
class HYPERLINKEDITOR_API UHyperlinkDefinitionBlueprintBase : public UHyperlinkDefinition
{
	GENERATED_BODY()

public:
	virtual void Initialize() override;
	virtual void Deinitialize() override;
	virtual TSharedPtr<FJsonObject> GeneratePayload() const override;

protected:
	virtual void ExecutePayload(const TSharedRef<FJsonObject>& InPayload) override;

	// Blueprint Implementable versions
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Initialize"))
	void InitializeImpl();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Deinitialize"))
	void DeinitializeImpl();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="GeneratePayload"))
	FString GeneratePayloadImpl() const;

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="ExecutePayload"))
	void ExecutePayloadImpl(const FString& Payload);
};
