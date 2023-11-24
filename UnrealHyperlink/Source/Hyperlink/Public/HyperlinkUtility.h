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
#if WITH_EDITOR
	/* CODE ONLY UTILITY */
	
	// TODO: make equivalent without commandlist which can be called by blueprint
	static void ExtendToolMenuSection(const FName& MenuName, const FName& SectionName,
									  const TSharedPtr<FUICommandList>& CommandList, const TSharedPtr<const FUICommandInfo>& Command);

	static TSharedRef<FExtender> GetMenuExtender(const FName& ExtensionHook,
		EExtensionHook::Position HookPosition, const TSharedPtr<FUICommandList>& CommandList,
		const TSharedPtr<const FUICommandInfo>& Command, const FName& ExtenderName);
#endif //WITH_EDITOR
	
	/* EDITOR UTILITY */
	/* These functions should only be called in editor, never at runtime! */
	
	UFUNCTION(BlueprintCallable, Category = "Hyperlink | Editor Utility")
	static UObject* LoadObject(const FString& PackageName);
	
	/**
	 * @brief Open the asset editor for an asset or focus it if it's already open 
	 * @param PackageName Package name of the asset we wish to open the editor for
	 * @return The UObject the editor was opened for
	 */
	UFUNCTION(BlueprintCallable, Category = "Hyperlink | Editor Utility")
	static UObject* OpenEditorForAsset(const FString& PackageName);
	
	/* PARSING */
	
	// TODO: expose variables to blueprint
	static constexpr int32 DoubleStringLength{ sizeof(double) * 2 };
	static constexpr int32 VectorStringLength{ DoubleStringLength * 3 };

	UFUNCTION(BlueprintCallable, Category = "Hyperlink | Parsing")
	static FString VectorToHexString(const FVector& InVector);

	UFUNCTION(BlueprintCallable, Category = "Hyperlink | Parsing")
	static FString DoubleToHexString(double InDouble);

	UFUNCTION(BlueprintCallable, Category = "Hyperlink | Parsing")
	static FVector HexStringToVector(const FString& InHexString);

	UFUNCTION(BlueprintCallable, Category = "Hyperlink | Parsing")
	static double HexStringToDouble(const FString& InHexString);

private:
#if WITH_EDITOR
	/* Log call to editor only function */
	static void LogEditorOnlyCall(const TCHAR* FunctionName);
#endif //WITH_EDITOR
};
