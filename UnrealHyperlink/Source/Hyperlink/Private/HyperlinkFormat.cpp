// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkFormat.h"

#include "HyperlinkSettings.h"


FString FHyperlinkFormat::GetLinkGenerationBase()
{
	const UHyperlinkSettings* const Settings{ GetDefault<UHyperlinkSettings>() };
	
	const FString LinkBase
	{
		Settings->GetLinkHandlingMethod() == EHyperlinkHandlingMethod::Local
			? FHyperlinkFormat::ApplicationBase
			: Settings->GetLinkHandlerAddress()
	};

	return LinkBase / Settings->GetProjectIdentifier();
}

FString FHyperlinkFormat::GetLinkRegexBase()
{
	const UHyperlinkSettings* const Settings{ GetDefault<UHyperlinkSettings>() };
	
	const FString EscapedLocalHandler{ RegexEscapeString(FHyperlinkFormat::ApplicationBase) };
	const FString EscapedWebHandler{ RegexEscapeString(Settings->GetLinkHandlerAddress()) };
	return FString::Format(TEXT(R"((?:{0}|{1}\/){2})"),
		{ EscapedLocalHandler, EscapedWebHandler, Settings->GetProjectIdentifier() });
}

FString FHyperlinkFormat::RegexEscapeString(const FString& InString)
{
	// TODO: These characters are probably enough but might want to escape some more characters
	static const TArray<TCHAR> CharsToEscape{ '.', '/' };

	FString Ret{ InString };
	const FString Escape{ TEXT("\\") };
	for (const TCHAR Char : CharsToEscape)
	{
		Ret.ReplaceInline(&Char, *(Escape + Char));
	}

	return Ret;
}