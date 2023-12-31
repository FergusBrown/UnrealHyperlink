﻿// Copyright (c) 2023-2024 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkDefinitionBlueprintBase.generated.h"

struct FJsonObjectWrapper;

/**
 * Base class for implementing hyperlink definitions with blueprints
 */
UCLASS(Abstract, Blueprintable, meta=(ShowWorldContextPin))
class HYPERLINKEDITOR_API UHyperlinkDefinitionBlueprintBase : public UHyperlinkDefinition
{
	GENERATED_BODY()

public:
	virtual void Initialize() override;
	virtual void Deinitialize() override;
	virtual TSharedPtr<FJsonObject> GeneratePayload(const TArray<FString>& Args) const override;

protected:
	virtual void ExecutePayload(const TSharedRef<FJsonObject>& InPayload) override;

	// Blueprint Implementable versions
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Initialize"))
	void InitializeImpl();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Deinitialize"))
	void DeinitializeImpl();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="GeneratePayload"))
	FJsonObjectWrapper GeneratePayloadImpl() const;

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="ExecutePayload"))
	void ExecutePayloadImpl(const FJsonObjectWrapper& PayloadObject);

	// String versions intended for python
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="GeneratePayloadString"))
	FString GeneratePayloadStringImpl() const;

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="ExecutePayloadString"))
	void ExecutePayloadStringImpl(const FString& PayloadString);
};
