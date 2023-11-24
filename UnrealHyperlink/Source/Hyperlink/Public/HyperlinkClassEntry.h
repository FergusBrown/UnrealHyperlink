// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "HyperlinkDefinition.h"
#include "HyperlinkClassEntry.generated.h"

USTRUCT()
struct FHyperlinkClassEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bEnabled{ true };
	
	/* A class which inherits from UHyperlinkDefinition */
	UPROPERTY(VisibleAnywhere, NoClear, DisplayName = "Definition Class")
	TSubclassOf<UHyperlinkDefinition> Class{ nullptr };

	/* The string which will be used to identify this class */
	UPROPERTY(EditAnywhere, DisplayName = "Definition Identifier", meta = (EditCondition = "bEnabled"))
	FString Identifier{ TEXT("") };
};

