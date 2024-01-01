// Copyright (c) 2023-2024 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkCommonPayload.generated.h"

/* Common payload struct formats for use by multiple definitions */

/* Simple payload containing a single FName */
USTRUCT()
struct FHyperlinkNamePayload
{
	GENERATED_BODY()

	UPROPERTY()
	FName Name{};
};