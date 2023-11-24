// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkDefinitionSettings.h"
#include "HyperlinkEditSettings.generated.h"

/**
 * 
 */
UCLASS()
class UHyperlinkEditSettings : public UHyperlinkDefinitionSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config)
	bool bEnableInAssetContextMenu{ true };
};
