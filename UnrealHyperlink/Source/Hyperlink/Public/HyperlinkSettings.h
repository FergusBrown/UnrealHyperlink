// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "HyperlinkSettings.generated.h"

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
	uint32 GetLocalServerPort() const{ return LocalServerPort; };
	
#if WITH_EDITOR
private:
	/* Delegates functions for different stages of editor initialisation */
	void OnAssetRegistryReady();
	void OnPythonInitialised();
	
	void OnAssetCreated(UObject* InObject);
	
	/**
	 * @brief Register "In Memory" classes (C++ and python classes)
	 * @return true if any new classes are registered
	 */
	bool RegisterInMemoryClasses();

	/**
	 * @brief Register any generated classes from blueprint assets
	 * @return true if any new classes are registered
	 */
	bool RegisterBlueprintClasses();
	
	bool RegisterDefinitionClass(UClass* Class);
	
	void PostRegister();
#endif //WITH_EDITOR

protected:
	/* Project identifier used in the link. By default this should be the name of the project. */
	UPROPERTY(Config, EditAnywhere, Category = "Project")
	FString ProjectIdentifier{ TEXT("") };

	/** The port of the web server used for handling links. */
	UPROPERTY(config, EditAnywhere, Category = "Project")
	uint32 LocalServerPort{ 10416 }; // (Rudy's Birthday, hopefully unused)
	
	/*
	 * List of definitions discovered in this project and whether each definition is enabled
	 * Only enabled hyperlink types can be generated and executed by the plugin
	 */
	UPROPERTY(Config, EditAnywhere, Category = "RegisteredDefinitions")
	TArray<FHyperlinkClassEntry> RegisteredDefinitions{};

private:
	bool bInMemoryClassesRegistered{ false };
	bool bBlueprintClassesRegistered{ false };
	
	friend class FHyperlinkSettingsCustomization;
};
