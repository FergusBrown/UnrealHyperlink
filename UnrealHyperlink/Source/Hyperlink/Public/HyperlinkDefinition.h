// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HyperlinkDefinition.generated.h"

/**
 * Abstract class for defining hyperlink types
 */
UCLASS(Abstract, Blueprintable)
class HYPERLINK_API UHyperlinkDefinition : public UObject
{
	GENERATED_BODY()
};
