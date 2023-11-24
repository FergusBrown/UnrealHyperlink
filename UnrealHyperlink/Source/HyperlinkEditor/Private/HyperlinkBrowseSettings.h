// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkDefinitionSettings.h"
#include "HyperlinkBrowseSettings.generated.h"

/**
 * 
 */
UCLASS()
class UHyperlinkBrowseSettings : public UHyperlinkDefinitionSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config)
	bool bEnableInAssetContextMenu{ true };

	UPROPERTY(EditAnywhere, Config)
	bool bEnableInFolderContextMenu{ true };
};