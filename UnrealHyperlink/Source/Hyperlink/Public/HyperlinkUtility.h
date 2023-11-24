// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HyperlinkUtility.generated.h"

/**
 * 
 */
UCLASS()
class HYPERLINK_API UHyperlinkUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/* LINK HANDLING UTILITY */
	UFUNCTION(BlueprintCallable, Category = "Hyperlink | Link Handling Utility")
	static FString GetLinkBaseAddress();
	static FString GetLinkStructureHint();
	
	UFUNCTION(BlueprintCallable, Category = "Hyperlink | Link Handling Utility")
	static FString CreateLinkFromPayload(TSubclassOf<UHyperlinkDefinition> DefinitionClass,
		const FJsonObjectWrapper& InPayload);
	
	static FString CreateLinkFromPayload(TSubclassOf<UHyperlinkDefinition> DefinitionClass,
		const TSharedRef<FJsonObject>& InPayload);
	
#if WITH_EDITOR
	/* CODE ONLY UTILITY */
	// TODO: Add Comments
	static void AddHyperlinkSubMenu(const FName& MenuName, const FName& SectionName);
	static void AddHyperlinkMenuEntry(const FName& MenuName, const TSharedPtr<FUICommandList>& CommandList,
									  const TSharedPtr<const FUICommandInfo>& Command);
	
	// TODO: make equivalent without commandlist which can be called by blueprint
	static void AddHyperlinkSubMenuAndEntry(const FName& MenuName, const FName& SectionName,
									  const TSharedPtr<FUICommandList>& CommandList,
									  const TSharedPtr<const FUICommandInfo>& Command);

	static TSharedRef<FExtender> GetMenuExtender(const FName& ExtensionHook,
		EExtensionHook::Position HookPosition, const TSharedPtr<FUICommandList>& CommandList,
		const TSharedPtr<const FUICommandInfo>& Command, const FName& ExtenderName);
#endif //WITH_EDITOR

	/* Use to create a nice display string for a class*/
	static FString CreateClassDisplayString(const UClass* Class);
	
	/* EDITOR UTILITY */
	/* These functions should only be called in editor, they'll do nothing at runtime! */
	
	UFUNCTION(BlueprintCallable, Category = "Hyperlink | Editor Utility")
	static UObject* LoadObject(const FString& PackageName);
	
	/**
	 * @brief Open the asset editor for an asset or focus it if it's already open 
	 * @param PackageName Package name of the asset we wish to open the editor for
	 * @return The UObject the editor was opened for
	 */
	UFUNCTION(BlueprintCallable, Category = "Hyperlink | Editor Utility")
	static UObject* OpenEditorForAsset(const FName& PackageName);

private:
#if WITH_EDITOR
	/* Log call to editor only function */
	static void LogEditorOnlyCall(const TCHAR* FunctionName);
#endif //WITH_EDITOR
};
