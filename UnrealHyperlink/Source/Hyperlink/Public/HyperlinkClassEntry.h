// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "HyperlinkDefinition.h"
#include "HyperlinkClassEntry.generated.h"

USTRUCT()
struct FHyperlinkClassEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = HyperlinkClassEntry)
	bool bEnabled{ true };
	
	/*
	 * A C++ class which inherits from UHyperlinkDefinition
	 * or a Blueprint which inherits from UHyperlinkDefinitionBlueprintBase
	 */
	UPROPERTY(VisibleAnywhere, NoClear, DisplayName = "Definition Class", Category = HyperlinkClassEntry)
	TSoftClassPtr<UHyperlinkDefinition> Class{ nullptr };

	/* The string which will be used to identify this class */
	UPROPERTY(EditAnywhere, DisplayName = "Definition Identifier", Category = HyperlinkClassEntry, meta = (EditCondition = "bEnabled"))
	FString Identifier{ TEXT("") };
};

