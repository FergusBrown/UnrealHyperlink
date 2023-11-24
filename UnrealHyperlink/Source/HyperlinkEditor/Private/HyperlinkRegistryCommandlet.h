// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "HyperlinkRegistryCommandlet.generated.h"

/**
 * Commandlet which installs registry values required for Unreal Hyperlink
 * Needs to be executed with elevated privileges
 */
UCLASS()
class UHyperlinkRegistryCommandlet : public UCommandlet
{
	GENERATED_BODY()
	
public:
	virtual int32 Main(const FString& Params) override;
};
