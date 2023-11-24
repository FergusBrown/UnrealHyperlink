// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "HyperlinkDefinitionSettings.generated.h"

/**
 * Base class for implementing any settings specific to a class inheriting from UHyperlinkDefinition
 */
UCLASS(Abstract, Config = Hyperlink, DefaultConfig, meta = (DisplayName = "Hyperlink"))
class HYPERLINK_API UHyperlinkDefinitionSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual FName GetCategoryName() const override;
	virtual bool SupportsAutoRegistration() const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif //WITH_EDITOR
};
