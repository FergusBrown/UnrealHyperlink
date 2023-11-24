// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"

/**
 * Common utilities for link executors
 */
namespace FHyperlinkUtils
{
#if WITH_EDITOR
	HYPERLINK_API UObject* LoadObject(const FString& PackageName);
	/**
	 * @brief Open the asset editor for an asset or focus it if it's already open 
	 * @param PackageName Package name of the asset we wish to open the editor for
	 * @return The UObject the editor was opened for
	 */
	HYPERLINK_API UObject* OpenEditorForAsset(const FString& PackageName);
	HYPERLINK_API TSharedRef<FExtender> GetMenuExtender(const FName& ExtensionHook, EExtensionHook::Position HookPosition, const TSharedPtr<FUICommandList> CommandList, const TSharedPtr<const FUICommandInfo> Command, const FName& ExtenderName);
#endif //WITH_EDITOR

	static constexpr int32 DoubleStringLength{ sizeof(double) * 2 };
	static constexpr int32 VectorStringLength{ DoubleStringLength * 3 };
	HYPERLINK_API FString VectorToHexString(const FVector& InVector);
	HYPERLINK_API FString DoubleToHexString(double InDouble);
	HYPERLINK_API TCHAR NibbleToHexChar(int64 InNibble);

	HYPERLINK_API FVector HexStringToVector(const FString& InHexString);
	HYPERLINK_API double HexStringToDouble(const FString& InHexString);
	HYPERLINK_API int64 HexCharToNibble(TCHAR InHexChar);
};
