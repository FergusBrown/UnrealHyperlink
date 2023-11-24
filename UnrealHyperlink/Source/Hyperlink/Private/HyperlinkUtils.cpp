// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkUtils.h"

#include "Log.h"

#if WITH_EDITOR
#include "Styling/StarshipCoreStyle.h"
#endif //WITH_EDITOR


UObject* FHyperlinkUtils::LoadObjectFromPackageName(const FString& PackageName)
{
	UObject* Ret{ nullptr };
	
	if (UPackage* const Package{ LoadPackage(nullptr, *PackageName, LOAD_NoRedirects) })
	{
		Package->FullyLoad();

		const FString AssetName{ FPaths::GetBaseFilename(PackageName) };
		Ret = FindObject<UObject>(Package, *AssetName);
	}
	UE_CLOG(!Ret, LogHyperlink, Error, TEXT("Failed to load %s"), *PackageName);

	return Ret;
}

#if WITH_EDITOR
TSharedRef<FExtender> FHyperlinkUtils::GetMenuExtender(const FName& ExtensionHook, EExtensionHook::Position HookPosition, const TSharedPtr<FUICommandList> CommandList, const TSharedPtr<const FUICommandInfo> Command, const FName& ExtenderName)
{
	TSharedRef<FExtender> Extender{ MakeShared<FExtender>() };
	
	Extender->AddMenuExtension(
		ExtensionHook,
		HookPosition,
		CommandList,
		FMenuExtensionDelegate::CreateLambda(
			[=](FMenuBuilder& MenuBuilder)
			{
				MenuBuilder.AddMenuEntry(
					Command,
					ExtenderName,
					TAttribute<FText>(),
					TAttribute<FText>(),
					FSlateIcon(FStarshipCoreStyle::GetCoreStyle().GetStyleSetName(), TEXT("Icons.Link"))
				);
			}
		)
	);
	
	return Extender;
}
#endif //WITH_EDITOR

namespace HexConstants
{
	static constexpr int32 NibbleLength{ 4 };
	static constexpr int32 DoubleStringLength{ sizeof(double) / NibbleLength };
	static constexpr int32 VectorStringLength{ DoubleStringLength * 3 };
	static constexpr TCHAR ZeroChar{ TEXT('0') };
	static constexpr TCHAR NineChar{ TEXT('9') };
	static constexpr TCHAR AChar{ TEXT('A') };
	static constexpr TCHAR FChar{ TEXT('F') };
}

FString FHyperlinkUtils::VectorToHexString(const FVector& InVector)
{
	return DoubleToHexString(InVector.X) + DoubleToHexString(InVector.Y) + DoubleToHexString(InVector.Z);
}

FString FHyperlinkUtils::DoubleToHexString(double InDouble)
{
	static constexpr int32 NibbleCount{ sizeof(double) * 2 };

	// We require an int for bit operations
	const int64 DoubleAsInt{ *reinterpret_cast<int64*>(&InDouble) };

	FString RetString{ TEXT("") };
	for (int32 Idx{ 0 }; Idx < NibbleCount; ++Idx)
	{
		RetString += NibbleToHexChar(DoubleAsInt >> (HexConstants::NibbleLength * Idx));
	}
	return RetString;
}

TCHAR FHyperlinkUtils::NibbleToHexChar(int64 InNibble)
{
	InNibble &= 0xF;

	if (InNibble <= 0x9)
	{
		return HexConstants::ZeroChar + InNibble;
	}
	else
	{
		return HexConstants::AChar + InNibble - 0xA;
	}
}

FVector FHyperlinkUtils::HexStringToVector(const FString& InHexString)
{
	check(InHexString.Len() == HexConstants::VectorStringLength);

	FVector RetVector{};
	
	for (int32 Idx{ 0 }; Idx < 3; ++Idx)
	{
		RetVector[Idx] = HexStringToDouble(InHexString.Mid(Idx * HexConstants::DoubleStringLength, HexConstants::DoubleStringLength));
	}

	return RetVector;
}

double FHyperlinkUtils::HexStringToDouble(const FString& InHexString)
{
	check(InHexString.Len() == HexConstants::DoubleStringLength);
	int64 DoubleAsInt{ 0 };
	for (int32 Idx{ 0 }; Idx < InHexString.Len(); ++Idx)
	{
		DoubleAsInt |= HexCharToNibble(InHexString[Idx]) << (HexConstants::NibbleLength * Idx);
	}
	
	return *reinterpret_cast<double*>(&DoubleAsInt);
}

int64 FHyperlinkUtils::HexCharToNibble(TCHAR InHexChar)
{
	if (InHexChar >= HexConstants::ZeroChar && InHexChar <= HexConstants::NineChar)
	{
		return InHexChar - HexConstants::ZeroChar;
	}
	else if (InHexChar >= HexConstants::AChar && InHexChar <= HexConstants::FChar)
	{
		return InHexChar - HexConstants::AChar;
	}
	else
	{
		return 0; // TODO: some sort of error checking
	}
}
