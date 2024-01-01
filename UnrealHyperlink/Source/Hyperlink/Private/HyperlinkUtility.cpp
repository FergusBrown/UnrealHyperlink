// Copyright (c) 2023-2024 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkUtility.h"

#include "HyperlinkDefinition.h"
#include "HyperlinkExecutePayload.h"
#include "HyperlinkPythonBridge.h"
#include "HyperlinkSettings.h"
#include "Internationalization/Regex.h"
#include "JsonObjectConverter.h"

#if WITH_EDITOR
#include "LogHyperlink.h"
#include "Styling/StarshipCoreStyle.h"

namespace FHyperlinkUtilityConstants
{
	static const FName SubMenuName{ TEXT("HyperlinkSubMenu") };
}
#endif //WITH_EDITOR

#define LOCTEXT_NAMESPACE "Hyperlink"

FString FHyperlinkUtility::GetLinkBaseAddress()
{
	const UHyperlinkSettings* const Settings{ GetDefault<UHyperlinkSettings>() };
	return FString::Printf(TEXT("http://localhost:%d/%s"),
		Settings->GetLocalServerPort(), *Settings->GetProjectIdentifier());
}

FString FHyperlinkUtility::GetLinkStructureHint()
{
	return GetLinkBaseAddress() / TEXT("JsonPayload");
}

FString FHyperlinkUtility::CreateLinkFromPayload(const TSubclassOf<UHyperlinkDefinition> DefinitionClass,
                                                 const TSharedRef<FJsonObject>& InPayload)
{
	FString PayloadString{};
	if (DefinitionClass)
	{
		// Create execute payload
		FHyperlinkExecutePayload ExecutePayload{};
		{
			ExecutePayload.Class = DefinitionClass;
			
			FJsonObjectWrapper ObjectWrapper{};
			ObjectWrapper.JsonObject = InPayload.ToSharedPtr();
			ExecutePayload.DefinitionPayload = MoveTemp(ObjectWrapper);
		}
		
		FJsonObjectConverter::UStructToJsonObjectString(ExecutePayload, PayloadString, 0, 0, 0, nullptr, false);

		// Escape any special characters in the URL
#if WITH_EDITOR
		// Use the more complete python version of the function if we're in the editor
		if (GEditor)
		{
			PayloadString = UHyperlinkPythonBridge::GetChecked().EscapeUrlString(PayloadString);
		}
		else
#endif //WITH_EDITOR
		{
			// Escape code here
			PayloadString = FHyperlinkUtility::EscapeUrlString(PayloadString);
		}
	}
	
	return GetLinkBaseAddress() / PayloadString;
}

FString FHyperlinkUtility::EscapeUrlString(const FString& InString)
{
	// Map of char to the URL encoded equivalent.
	static const TCHAR* CharEncodingMap[][2] 
	{
		// Always replace "%" first to avoid double-encoding characters
		{ TEXT("%"), TEXT("%25") },
		{ TEXT(" "), TEXT("%20") },
		{ TEXT("!"), TEXT("%21") },
		{ TEXT("\""), TEXT("%22") },
		{ TEXT("#"), TEXT("%23") },
		{ TEXT("$"), TEXT("%24") },
		{ TEXT("&"), TEXT("%26") },
		{ TEXT("'"), TEXT("%27") },
		{ TEXT("("), TEXT("%28") },
		{ TEXT(")"), TEXT("%29") },
		{ TEXT("*"), TEXT("%2A") },
		{ TEXT("+"), TEXT("%2B") },
		{ TEXT(","), TEXT("%2C") },
		{ TEXT(":"), TEXT("%3A") },
		{ TEXT(";"), TEXT("%3B") },
		{ TEXT("="), TEXT("%3D") },
		{ TEXT("?"), TEXT("%3F") },
		{ TEXT("["), TEXT("%5B") },
		{ TEXT("]"), TEXT("%5D") },
		{ TEXT("@"), TEXT("%40") },
		{ TEXT("{"), TEXT("%7B") },
		{ TEXT("}"), TEXT("%7D") },
		{ TEXT("~"), TEXT("%7E") },
	};

	FString EscapedString{ InString };

	for (int32 Idx { 0 }; Idx < GetNum(CharEncodingMap); ++Idx )
	{
		const TCHAR* const OriginalChar{ CharEncodingMap[Idx][0] };
		const TCHAR* const EscapedChar{ CharEncodingMap[Idx][1] };
		// Use ReplaceInline as that won't create a copy of the string if the character isn't found
		EscapedString.ReplaceInline(OriginalChar, EscapedChar, ESearchCase::CaseSensitive);
	}

	return EscapedString;
}

#if WITH_EDITOR
FSlateIcon FHyperlinkUtility::GetMenuIcon()
{
	return FSlateIcon(FStarshipCoreStyle::GetCoreStyle().GetStyleSetName(), TEXT("Icons.Link"));
}

void FHyperlinkUtility::AddHyperlinkSubMenu(const FName& MenuName, const FName& SectionName)
{
	FToolMenuEntry SubMenuArgs
	{
		FToolMenuEntry::InitSubMenu(
			FHyperlinkUtilityConstants::SubMenuName,
			LOCTEXT("HyperlinkSubMenuLabel", "Share Hyperlink"),
			LOCTEXT("HyperlinkSubMenuToolTip", "Copy a link to which can be used to navigate to this item."),
			FNewToolMenuChoice(),
			false,
			FSlateIcon(FStarshipCoreStyle::GetCoreStyle().GetStyleSetName(), TEXT("Icons.Link")))
	};
	SubMenuArgs.Owner = MenuName;

	UToolMenu* const ToolMenu{ UToolMenus::Get()->ExtendMenu(MenuName) };
	ToolMenu->FindOrAddSection(SectionName).AddEntry(SubMenuArgs);
}

void FHyperlinkUtility::AddHyperlinkMenuEntry(const FName& MenuName, const TSharedPtr<FUICommandList>& CommandList,
	const TSharedPtr<const FUICommandInfo>& Command, const bool bWithSubMenu/*= true*/)
{
	FName MenuPath;
	if (bWithSubMenu)
	{
		MenuPath = UToolMenus::JoinMenuPaths(MenuName, FHyperlinkUtilityConstants::SubMenuName);
	}
	else
	{
		MenuPath = MenuName;
	}
	UToolMenu* const Menu{ UToolMenus::Get()->ExtendMenu(MenuPath) };

	// Add action entry to the menu
	static const FName CopySectionName{ TEXT("HyperlinkActions") };
	FToolMenuEntry EntryArgs{ FToolMenuEntry::InitMenuEntryWithCommandList(Command, CommandList) };
	EntryArgs.Owner = MenuName;
	if (!bWithSubMenu)
	{
		EntryArgs.Icon = GetMenuIcon();
	}
	Menu->AddMenuEntry(CopySectionName, EntryArgs);
}

void FHyperlinkUtility::AddHyperlinkSubMenuAndEntry(const FName& MenuName, const FName& SectionName,
                                              const TSharedPtr<FUICommandList>& CommandList,
                                              const TSharedPtr<const FUICommandInfo>& Command)
{
	//TODO: provide mechanism for unregistering tool menu extensions with UToolMenu::RemoveEntry or UToolMenus::UnregisterOwner
	
	// Make our submenu entry.
	AddHyperlinkSubMenu(MenuName, SectionName);

	// Add action entry to the submenu
	AddHyperlinkMenuEntry(MenuName, CommandList, Command);
}

void FHyperlinkUtility::AddHyperlinkCopyEntry(const FName& MenuName, const FText& EntryLabel, const FText& ToolTip,
	const UHyperlinkDefinition* const HyperlinkDefinition, const bool bWithSubMenu/*= true*/)
{
	FName MenuPath;
	if (bWithSubMenu)
	{
		MenuPath = UToolMenus::JoinMenuPaths(MenuName, FHyperlinkUtilityConstants::SubMenuName);
	}
	else
	{
		MenuPath = MenuName;
	}
	UToolMenu* const Menu{ UToolMenus::Get()->ExtendMenu(MenuPath) };

	// Add action entry to the menu
	static const FName CopySectionName{ TEXT("HyperlinkActions") };
	FName EntryName{ HyperlinkDefinition->GetClass()->GetDisplayNameText().ToString() };
	FToolMenuEntry EntryArgs
	{
		FToolMenuEntry::InitMenuEntry(EntryName, EntryLabel, ToolTip, FSlateIcon(),
			FUIAction(FExecuteAction::CreateUObject(HyperlinkDefinition, &UHyperlinkDefinition::CopyLink)))
	};
	EntryArgs.Owner = MenuName;
	if (!bWithSubMenu)
	{
		EntryArgs.Icon = GetMenuIcon();
	}
	Menu->AddMenuEntry(CopySectionName, EntryArgs);
}

void FHyperlinkUtility::AddHyperlinkCopySubMenuAndEntry(const FName& MenuName, const FName& SectionName,
	const FText& EntryLabel, const FText& ToolTip, const UHyperlinkDefinition* const HyperlinkDefinition)
{
	// Make our submenu entry.
	AddHyperlinkSubMenu(MenuName, SectionName);

	// Add action entry to the submenu
	AddHyperlinkCopyEntry(MenuName, EntryLabel, ToolTip, HyperlinkDefinition);
}

TSharedRef<FExtender> FHyperlinkUtility::GetMenuExtender(const FName& ExtensionHook,
	const EExtensionHook::Position HookPosition, const TSharedPtr<FUICommandList>& CommandList,
	const TSharedPtr<const FUICommandInfo>& Command, const FName& ExtenderName)
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
					GetMenuIcon()
				);
			}
		)
	);
	
	return Extender;
}

UObject* FHyperlinkUtility::LoadObject(const FString& PackageName)
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

UObject* FHyperlinkUtility::OpenEditorForAsset(const FString& PackageName)
{
	UObject* const Object{ LoadObject(PackageName) };
	if (Object)
	{
		// Need to check if this is a level first. Levels will be reopened rather than focused by OpenEditorForAsset
		// so we need skip this step if the level is already open in the level editor
		bool bSkipOpen{ false };
		if (const UWorld* const EditorWorld{ GEditor->GetEditorWorldContext().World() })
		{
			bSkipOpen = PackageName == EditorWorld->GetMapName();
		}
		if (!bSkipOpen)
		{
			GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Object);
		}
	}
	return Object;
}

UObject* FHyperlinkUtility::OpenEditorForAsset(const FName& PackageName)
{
	return OpenEditorForAsset(PackageName.ToString());
}

FString FHyperlinkUtility::CreateClassDisplayString(const UClass* const Class)
{
	FString Identifier{ Class->GetDisplayNameText().ToString() };
	Identifier.RemoveSpacesInline();
	
	// Try create a nice display name. Expect class names to typically follow the format "...HyperlinkType"
	const FRegexPattern Pattern{ TEXT("Hyperlink(.*)$") };
	FRegexMatcher Matcher{ Pattern, Identifier };

	if (Matcher.FindNext())
	{
		Identifier = Matcher.GetCaptureGroup(1);
	}
	
	return  Identifier;
}

#endif //WITH_EDITOR

#undef LOCTEXT_NAMESPACE
