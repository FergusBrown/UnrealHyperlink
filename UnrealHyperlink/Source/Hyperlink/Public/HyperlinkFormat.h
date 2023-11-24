// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"

/**
 * Utilities for formatting a hyperlink
 */
namespace FHyperlinkFormat
{
	static const TCHAR* ApplicationBase{ TEXT("unreal://") };
	/* Use colon to separate link args as it's illegal to have it in UE paths */
	static constexpr TCHAR ArgSeparator{ TEXT(':') };
	static const TCHAR* BaseFormat{ TEXT("{0}/{1}/{2}") };

	static const TCHAR* StructureHint{ TEXT("Handler/ProjectID/DefinitionID/Body") };

	HYPERLINK_API FString GetLinkGenerationBase();
	HYPERLINK_API FString GetLinkRegexBase();
	HYPERLINK_API FString RegexEscapeString(const FString& InString);
};
