// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkUtility.h"

#include "HyperlinkDefinition.h"
#include "HyperlinkExecutePayload.h"
#include "HyperlinkSettings.h"
#include "Internationalization/Regex.h"
#include "JsonObjectConverter.h"
#include "Log.h"
#include "Serialization/JsonSerializer.h"

#if WITH_EDITOR
#include "Styling/StarshipCoreStyle.h"

#define LOCTEXT_NAMESPACE "Hyperlink"

namespace FHyperlinkUtilityConstants
{
	static const FName SubMenuName{ TEXT("HyperlinkSubMenu") };
}

FString UHyperlinkUtility::GetLinkBaseAddress()
{
	const UHyperlinkSettings* const Settings{ GetDefault<UHyperlinkSettings>() };
	return FString::Printf(TEXT("http://localhost:%d/%s"),
		Settings->GetLocalServerPort(), *Settings->GetProjectIdentifier());
}

FString UHyperlinkUtility::GetLinkStructureHint()
{
	return GetLinkBaseAddress() / TEXT("JsonPayload");
}

FString UHyperlinkUtility::CreateLinkFromPayload(const TSubclassOf<UHyperlinkDefinition> DefinitionClass,
                                                 const FJsonObjectWrapper& InPayload)
{
	FString PayloadString{};
	if (DefinitionClass)
	{
		// Create execute payload
		FHyperlinkExecutePayload ExecutePayload{};
		{
			ExecutePayload.Class = DefinitionClass;;
			ExecutePayload.DefinitionPayload = InPayload;
		}
		
		FJsonObjectConverter::UStructToJsonObjectString(ExecutePayload, PayloadString, 0, 0, 0, nullptr, false);
	}
	
	return GetLinkBaseAddress() / PayloadString;
}

FString UHyperlinkUtility::CreateLinkFromPayload(const TSubclassOf<UHyperlinkDefinition> DefinitionClass,
                                                 const TSharedRef<FJsonObject>& InPayload)
{
	FJsonObjectWrapper ObjectWrapper{};
	ObjectWrapper.JsonObject = InPayload.ToSharedPtr();
	
	return CreateLinkFromPayload(DefinitionClass, ObjectWrapper);
}

void UHyperlinkUtility::AddHyperlinkSubMenu(const FName& MenuName, const FName& SectionName)
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

void UHyperlinkUtility::AddHyperlinkMenuEntry(const FName& MenuName, const TSharedPtr<FUICommandList>& CommandList,
	const TSharedPtr<const FUICommandInfo>& Command)
{
	const FName SubMenuPath{ UToolMenus::JoinMenuPaths(MenuName, FHyperlinkUtilityConstants::SubMenuName) };
	UToolMenu* const SubMenu{ UToolMenus::Get()->ExtendMenu(SubMenuPath) };

	// Add action entry to the submenu
	static const FName CopySectionName{ TEXT("HyperlinkActions") };
	FToolMenuEntry EntryArgs{ FToolMenuEntry::InitMenuEntryWithCommandList(Command, CommandList) };
	EntryArgs.Owner = MenuName;
	SubMenu->AddMenuEntry(CopySectionName, EntryArgs);
}

void UHyperlinkUtility::AddHyperlinkSubMenuAndEntry(const FName& MenuName, const FName& SectionName,
                                              const TSharedPtr<FUICommandList>& CommandList,
                                              const TSharedPtr<const FUICommandInfo>& Command)
{
	//TODO: provide mechanism for unregistering tool menu extensions with UToolMenu::RemoveEntry or UToolMenus::UnregisterOwner
	
	// Make our submenu entry.
	AddHyperlinkSubMenu(MenuName, SectionName);

	// Add action entry to the submenu
	AddHyperlinkMenuEntry(MenuName, CommandList, Command);
}

#undef LOCTEXT_NAMESPACE

TSharedRef<FExtender> UHyperlinkUtility::GetMenuExtender(const FName& ExtensionHook,
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
					FSlateIcon(FStarshipCoreStyle::GetCoreStyle().GetStyleSetName(), TEXT("Icons.Link"))
				);
			}
		)
	);
	
	return Extender;
}

#endif //WITH_EDITOR

FString UHyperlinkUtility::CreateClassDisplayString(const UClass* Class)
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

UObject* UHyperlinkUtility::LoadObject(const FString& PackageName)
{
#if WITH_EDITOR
	UObject* Ret{ nullptr };
	
	if (UPackage* const Package{ LoadPackage(nullptr, *PackageName, LOAD_NoRedirects) })
	{
		Package->FullyLoad();

		const FString AssetName{ FPaths::GetBaseFilename(PackageName) };
		Ret = FindObject<UObject>(Package, *AssetName);
	}
	UE_CLOG(!Ret, LogHyperlink, Error, TEXT("Failed to load %s"), *PackageName);

	return Ret;
#else //WITH_EDITOR
	LogEditorOnlyCall(TEXT("LoadObject"));
	return nullptr;
#endif //WITH_EDITOR
}

UObject* UHyperlinkUtility::OpenEditorForAsset(const FName& PackageName)
{
#if WITH_EDITOR
	UObject* const Object{ LoadObject(PackageName.ToString()) };
	if (Object)
	{
		// Need to check if this is a level first. Levels will be reopened rather than focused by OpenEditorForAsset
		// so we need skip this step if the level is already open in the level editor
		bool bSkipOpen{ false };
		if (const UWorld* const EditorWorld{ GEditor->GetEditorWorldContext().World() })
		{
			bSkipOpen = PackageName == EditorWorld->PersistentLevel->GetPackage()->GetFName();
		}
		if (!bSkipOpen)
		{
			GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Object);
		}
	}
	return Object;
#else //WITH_EDITOR
	LogEditorOnlyCall(TEXT("OpenEditorForAsset"));
	return nullptr;
#endif //WITH_EDITOR
}

#if WITH_EDITOR
void UHyperlinkUtility::LogEditorOnlyCall(const TCHAR* FunctionName)
{
	UE_LOG(LogHyperlink, Log, TEXT("Editor only hyperlink utility called at runtime (UHyperlinkUtility::%s)"), FunctionName);
}
#endif //WITH_EDITOR

