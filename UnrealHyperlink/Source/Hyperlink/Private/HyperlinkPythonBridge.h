﻿// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HyperlinkPythonBridge.generated.h"

/**
 * Utility class for accessing python functions from C++
 */
UCLASS(Blueprintable)
class HYPERLINK_API UHyperlinkPythonBridge : public UObject
{
	GENERATED_BODY()

public:
	/* Get pointer to the python implementation of this class */
	static const UHyperlinkPythonBridge* Get();
	static const UHyperlinkPythonBridge& GetChecked();

	UFUNCTION(BlueprintImplementableEvent)
	FString ParseUrlString(const FString& InString) const;

	UFUNCTION(BlueprintImplementableEvent)
	FString EscapeUrlString(const FString& InString) const;
};

