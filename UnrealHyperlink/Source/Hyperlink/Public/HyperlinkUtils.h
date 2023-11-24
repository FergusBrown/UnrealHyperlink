// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"

/**
 * Common utilities for link executors
 */
namespace FHyperlinkUtils
{
	HYPERLINK_API UObject* LoadObjectFromPackageName(const FString& PackageName);

#if WITH_EDITOR
	HYPERLINK_API TSharedRef<FExtender> GetMenuExtender(const FName& ExtensionHook, EExtensionHook::Position HookPosition, const TSharedPtr<FUICommandList> CommandList, const TSharedPtr<const FUICommandInfo> Command, const FName& ExtenderName);
#endif //WITH_EDITOR

	HYPERLINK_API FString VectorToHexString(const FVector& InVector);
	HYPERLINK_API FString DoubleToHexString(double InDouble);
	HYPERLINK_API TCHAR NibbleToHexChar(int64 InNibble);

	HYPERLINK_API FVector HexStringToVector(const FString& InHexString);
	HYPERLINK_API double HexStringToDouble(const FString& InHexString);
	HYPERLINK_API int64 HexCharToNibble(TCHAR InHexChar);
};
