// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"

class UHyperlinkDefinition;

/**
 * 
 */
class HYPERLINK_API FHyperlinkUtility
{

public:
	/* LINK HANDLING UTILITY */
	static FString GetLinkBaseAddress();
	static FString GetLinkStructureHint();
	
	static FString CreateLinkFromPayload(TSubclassOf<UHyperlinkDefinition> DefinitionClass,
		const TSharedRef<FJsonObject>& InPayload);
	
#if WITH_EDITOR
	/* CODE ONLY UTILITY */

	static FSlateIcon GetMenuIcon();
	
	// TODO: Add Comments
	static void AddHyperlinkSubMenu(const FName& MenuName, const FName& SectionName);
	static void AddHyperlinkMenuEntry(const FName& MenuName, const TSharedPtr<FUICommandList>& CommandList,
									  const TSharedPtr<const FUICommandInfo>& Command, bool bWithSubMenu = true);
	
	// TODO: make equivalent without commandlist which can be called by blueprint
	static void AddHyperlinkSubMenuAndEntry(const FName& MenuName, const FName& SectionName,
									  const TSharedPtr<FUICommandList>& CommandList,
									  const TSharedPtr<const FUICommandInfo>& Command);

	static TSharedRef<FExtender> GetMenuExtender(const FName& ExtensionHook,
		EExtensionHook::Position HookPosition, const TSharedPtr<FUICommandList>& CommandList,
		const TSharedPtr<const FUICommandInfo>& Command, const FName& ExtenderName);

	/* EDITOR UTILITY */
	
	static UObject* LoadObject(const FString& PackageName);
	
	/**
	 * @brief Open the asset editor for an asset or focus it if it's already open 
	 * @param PackageName Package name of the asset we wish to open the editor for
	 * @return The UObject the editor was opened for
	 */
	static UObject* OpenEditorForAsset(const FName& PackageName);
#endif //WITH_EDITOR

	/* Use to create a nice display string for a class*/
	static FString CreateClassDisplayString(const UClass* Class);
};
