// Copyright (c) 2023-2024 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HyperlinkPythonBridge.generated.h"

/**
 * Utility class for accessing python functions from C++
 */
UCLASS(Blueprintable, Hidden)
class HYPERLINK_API UHyperlinkPythonBridge : public UObject
{
	GENERATED_BODY()

public:
	/* Get pointer to the python implementation of this class */
	static const UHyperlinkPythonBridge* Get();
	static const UHyperlinkPythonBridge& GetChecked();

	/* Decode any escaped special characters in the URL. For example replacing "%7B" with '{' */
	UFUNCTION(BlueprintImplementableEvent)
	FString ParseUrlString(const FString& InString) const;

	/* Escape special characters in the URL. For or example replacing '{' with "%7B" */
	UFUNCTION(BlueprintImplementableEvent)
	FString EscapeUrlString(const FString& InString) const;

	UFUNCTION(BlueprintImplementableEvent)
	TArray<FString> GetSystemPaths() const;
};

