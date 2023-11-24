// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "HyperlinkSettings.generated.h"

class UHyperlinkDefinition;

UCLASS(Config = Hyperlink, DefaultConfig, meta = (DisplayName = "Hyperlink"))
class HYPERLINK_API UHyperlinkSettings : public UDeveloperSettings
{
private:
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;
	virtual FName GetCategoryName() const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:
	/* Project identifier used in the link. By default this should be the name of the project. */
	UPROPERTY(Config, EditAnywhere)
	FString ProjectIdentifier{ TEXT("") };

	/* List of definitions registered with this project
	 * Only registered hyperlink types can be generated and executed by the plugin
	 */
	UPROPERTY(Config, EditAnywhere)
	TSet<TSubclassOf<UHyperlinkDefinition>> RegisteredDefinitions{};
};
