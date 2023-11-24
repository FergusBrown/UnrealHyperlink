﻿// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkUtils.h"

#include "Log.h"

#if WITH_EDITOR
#include "Styling/StarshipCoreStyle.h"


UObject* FHyperlinkUtils::LoadObject(const FString& PackageName)
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

UObject* FHyperlinkUtils::OpenEditorForAsset(const FString& PackageName)
{
	UObject* const Object{ LoadObject(PackageName) };
	if (Object)
	{
		// Need to check if this is a level first. Levels will be reopened rather than focused by OpenEditorForAsset
		// so we need skip this step if the level is already open in the level editor
		bool bSkipOpen{ false };
		if (const UWorld* const EditorWorld{ GEditor->GetEditorWorldContext().World() })
		{
			bSkipOpen = PackageName == EditorWorld->PersistentLevel->GetPackage()->GetName();
		}
		if (!bSkipOpen)
		{
			GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Object);
		}
	}
	return Object;
}

#define LOCTEXT_NAMESPACE "Hyperlink"

void FHyperlinkUtils::ExtendToolMenuSection(const FName& MenuName, const FName& SectionName,
	const TSharedPtr<FUICommandList> CommandList, const TSharedPtr<const FUICommandInfo> Command)
{
	//TODO: provide mechanism for unregistering tool menu extensions with UToolMenu::RemoveEntry or UToolMenus::UnregisterOwner
	
	// Make our submenu entry.
	static const FName SubMenuName{ TEXT("HyperlinkSubMenu") };
	FToolMenuEntry Args = FToolMenuEntry::InitSubMenu(
		SubMenuName,
		LOCTEXT("HyperlinkSubMenuLabel", "Share Hyperlink"),
		LOCTEXT("HyperlinkSubMenuToolTip", "Copy a link to which can be used to navigate to this item."),
		FNewToolMenuChoice(),
		false,
		FSlateIcon(FStarshipCoreStyle::GetCoreStyle().GetStyleSetName(), TEXT("Icons.Link")));
	Args.Owner = MenuName;

	UToolMenu* const ToolMenu{ UToolMenus::Get()->ExtendMenu(MenuName) };
	ToolMenu->FindOrAddSection(SectionName).AddEntry(Args);
	
	UToolMenu* const SubMenu
	{
		UToolMenus::Get()->ExtendMenu(UToolMenus::JoinMenuPaths(MenuName, SubMenuName))
	};

	// Add action entry to the submenu
	static const FName CopySectionName{ TEXT("HyperlinkActions") };
	SubMenu->FindOrAddSection(CopySectionName);
	SubMenu->AddMenuEntry(CopySectionName,
	FToolMenuEntry::InitMenuEntryWithCommandList(Command, CommandList));
}

#undef LOCTEXT_NAMESPACE

TSharedRef<FExtender> FHyperlinkUtils::GetMenuExtender(const FName& ExtensionHook,
	const EExtensionHook::Position HookPosition, const TSharedPtr<FUICommandList> CommandList,
	const TSharedPtr<const FUICommandInfo> Command, const FName& ExtenderName)
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

FString FHyperlinkUtils::VectorToHexString(const FVector& InVector)
{
	return DoubleToHexString(InVector.X) + DoubleToHexString(InVector.Y) + DoubleToHexString(InVector.Z);
}

FString FHyperlinkUtils::DoubleToHexString(double InDouble)
{
	const uint64 Bytes{ NETWORK_ORDER64(*reinterpret_cast<uint64*>(&InDouble)) };

	FString Result{};
	BytesToHex(reinterpret_cast<const uint8*>(&Bytes), sizeof(Bytes), Result);

	return Result;
}

FVector FHyperlinkUtils::HexStringToVector(const FString& InHexString)
{
	check(InHexString.Len() == VectorStringLength);

	FVector RetVector{};
	
	for (int32 Idx{ 0 }; Idx < 3; ++Idx)
	{
		RetVector[Idx] = HexStringToDouble(InHexString.Mid(Idx * DoubleStringLength, DoubleStringLength));
	}

	return RetVector;
}

double FHyperlinkUtils::HexStringToDouble(const FString& InHexString)
{
	check(InHexString.Len() == DoubleStringLength);
	uint64 DoubleAsInt{ FParse::HexNumber64(*InHexString) };
	
	return *reinterpret_cast<double*>(&DoubleAsInt);
}
