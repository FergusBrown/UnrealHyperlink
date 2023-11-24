// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "HyperlinkSettings.generated.h"

class UHyperlinkDefinition;

/*
 * Define how you want links to be handled.
 * This aff
 */
UENUM()
enum class EHyperlinkHandlingMethod : uint8
{
	/* Use the local "unreal://" protocol handler on your PC */
	Local,
	/*
	 * Use the specified web server to handle links.
	 * This will involve querying a web server which redirects to the local "unreal://" link.
	 * While this method has an added intermediate stage it has the advantage that generated links
	 * will be treated as regular web links allowing you to past the hyperlinks in applications
	 * which block the "unreal://" link type
	 */
	Web,		
};

UCLASS(Config = Hyperlink, DefaultConfig, meta = (DisplayName = "Hyperlink"))
class HYPERLINK_API UHyperlinkSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;
#if WITH_EDITOR
  	virtual FName GetCategoryName() const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
#endif //WITH_EDITOR

	const TSet<TSubclassOf<UHyperlinkDefinition>>& GetRegisteredDefinitions() const;

	FString GetLinkGenerationBase() const;

#if WITH_EDITOR
private:
	void InitDefinitionSettings() const;
#endif //WITH_EDITOR

protected:
	/* Project identifier used in the link. By default this should be the name of the project. */
	UPROPERTY(Config, EditAnywhere, Category = "Project")
	FString ProjectIdentifier{ TEXT("") };

	/* What method should be used to generate and handle links */
	UPROPERTY(Config, EditAnywhere, Category = "Project")
	EHyperlinkHandlingMethod LinkHandlingMethod{ EHyperlinkHandlingMethod::Local };

	/* Web address which is used to handle links */
	UPROPERTY(Config, EditAnywhere, meta = (EditCondition = "HandlingMethod == EHyperlinkHandlingMethod::Web"), Category = "Project")
	FString LinkHandlerAddress{ TEXT("www.placeholder.com") };
	
	/*
	 * List of definitions registered with this project
	 * Only registered hyperlink types can be generated and executed by the plugin
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Definitions")
	TSet<TSubclassOf<UHyperlinkDefinition>> RegisteredDefinitions{};
	
};
