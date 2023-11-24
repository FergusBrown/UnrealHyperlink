// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"
#include "HyperlinkExecutePayload.generated.h"

class UHyperlinkDefinition;

USTRUCT(BlueprintType)
struct FHyperlinkExecutePayload
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UHyperlinkDefinition> Class{ nullptr };

	UPROPERTY(BlueprintReadWrite)
	FJsonObjectWrapper DefinitionPayload{};
};