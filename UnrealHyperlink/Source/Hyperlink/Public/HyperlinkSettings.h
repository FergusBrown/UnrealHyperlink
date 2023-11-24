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

	const FString& GetProjectIdentifier() const{ return ProjectIdentifier; };
	EHyperlinkHandlingMethod GetLinkHandlingMethod()const { return LinkHandlingMethod; };
	const FString& GetLinkHandlerAddress() const{ return LinkHandlerAddress; };
	uint32 GetLocalServerPort() const{ return LocalServerPort; };
	
#if WITH_EDITOR
private:
	/* Delegates functions for different stages of editor initialisation */
	void OnAllModulesLoaded();
	void OnAssetRegistryReady();

	/* Functions for registering hyperlink classes */
	bool RegisterCppClasses();
	bool RegisterBlueprintClasses();
	bool RegisterDefinitionClass(UClass* Class);
	
	void PostRegister();
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

	/** The port of the web server used for handling links. */
	UPROPERTY(config, EditAnywhere, Category = "Project")
	uint32 LocalServerPort{ 10416 }; // (Rudy's Birthday, hopefully unused)
	
	/*
	 * List of definitions discovered in this project and whether each definition is enabled
	 * Only enabled hyperlink types can be generated and executed by the plugin
	 */
	UPROPERTY(Config, EditAnywhere, Category = "RegisteredDefinitions")
	TArray<FHyperlinkClassEntry> RegisteredDefinitions{};

	friend class FHyperlinkSettingsCustomization;
};
