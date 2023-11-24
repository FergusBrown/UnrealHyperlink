// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "HyperlinkSettings.generated.h"

class UHyperlinkDefinition;

UENUM()
enum class EHyperlinkHandlingMethod : uint8
{
	/* Use the local "unreal://" url scheme */
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
#endif //WITH_EDITOR

	// Note TConstArrayView means the caller cannot edit the contents of the array
	TConstArrayView<FHyperlinkClassEntry> GetRegisteredDefinitions() const;
	
	FString GetLinkGenerationBase() const;
	FString GetLinkRegexBase() const;

private:
	// TODO: Maybe move this to utils
	static FString RegexEscapeString(const FString& InString);
	
#if WITH_EDITOR
	void OnAllModulesLoaded();
#endif //WITH_EDITOR

protected:
	/* Project identifier used in the link. By default this should be the name of the project. */
	UPROPERTY(Config, EditAnywhere, Category = "Project")
	FString ProjectIdentifier{ TEXT("") };

	/* What method should be used to generate and handle links */
	UPROPERTY(Config, EditAnywhere, Category = "Project")
	EHyperlinkHandlingMethod LinkHandlingMethod{ EHyperlinkHandlingMethod::Web };

	/* Web address which is used to handle links */
	UPROPERTY(Config, EditAnywhere, Category = "Project", meta = (EditCondition = "HandlingMethod == EHyperlinkHandlingMethod::Web"))
	FString LinkHandlerAddress{ TEXT("uehyper.link") };
	
	/*
	 * List of definitions discovered in this project and whether each definition is enabled
	 * Only enabled hyperlink types can be generated and executed by the plugin
	 */
	UPROPERTY(Config, EditAnywhere, Category = "RegisteredDefinitions")
	TArray<FHyperlinkClassEntry> RegisteredDefinitions{};

	friend class FHyperlinkSettingsCustomization;
};
