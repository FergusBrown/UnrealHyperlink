// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "HyperlinkSettings.generated.h"

UCLASS(Config = Hyperlink, DefaultConfig, meta = (DisplayName = "Hyperlink"))
class UHyperlinkSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UHyperlinkSettings();
	virtual FName GetCategoryName() const override;
	
public:
	/* Project identifier used in the link. By default this should be the name of the project. */
	UPROPERTY(Config, EditAnywhere)
	FString ProjectIdentifier{ TEXT("") };
};
