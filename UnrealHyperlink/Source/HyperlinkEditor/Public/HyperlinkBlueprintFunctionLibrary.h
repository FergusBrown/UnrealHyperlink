// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HyperlinkBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class HYPERLINKEDITOR_API UHyperlinkBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Hyperlink | Link Handling Utility")
	static FString CreateLinkFromPayload(TSubclassOf<UHyperlinkDefinition> DefinitionClass,
		const FJsonObjectWrapper& InPayload);

	UFUNCTION(BlueprintCallable, Category = "Hyperlink | Editor Utility")
	static UObject* LoadObject(const FString& PackageName);
	
	/**
	 * @brief Open the asset editor for an asset or focus it if it's already open 
	 * @param PackageName Package name of the asset we wish to open the editor for
	 * @return The UObject the editor was opened for
	 */
	UFUNCTION(BlueprintCallable, Category = "Hyperlink | Editor Utility")
	static UObject* OpenEditorForAsset(const FName& PackageName);

	UFUNCTION(BlueprintCallable, Category = "Hyperlink | Editor Utility")
	static void AddCopyLinkMenuEntry(const FName& MenuName, const FName& SectionName,
		const FText& EntryLabel, const FText& ToolTip, const UHyperlinkDefinition* HyperlinkDefinition,
		bool bWithSubMenu = true);
};
