// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkUtility.h"

#include "HyperlinkDefinition.h"
#include "HyperlinkExecutePayload.h"
#include "HyperlinkSettings.h"
#include "JsonObjectConverter.h"
#include "Log.h"
#include "Serialization/JsonSerializer.h"

#if WITH_EDITOR
#include "Styling/StarshipCoreStyle.h"

#define LOCTEXT_NAMESPACE "Hyperlink"

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

void UHyperlinkUtility::ExtendToolMenuSection(const FName& MenuName, const FName& SectionName,
                                              const TSharedPtr<FUICommandList>& CommandList,
                                              const TSharedPtr<const FUICommandInfo>& Command)
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
		{	UToolMenus::Get()->ExtendMenu(UToolMenus::JoinMenuPaths(MenuName, SubMenuName)) };

	// Add action entry to the submenu
	static const FName CopySectionName{ TEXT("HyperlinkActions") };
	SubMenu->FindOrAddSection(CopySectionName);
	SubMenu->AddMenuEntry(CopySectionName,
	FToolMenuEntry::InitMenuEntryWithCommandList(Command, CommandList));
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

FString UHyperlinkUtility::VectorToHexString(const FVector& InVector)
{
	return DoubleToHexString(InVector.X) + DoubleToHexString(InVector.Y) + DoubleToHexString(InVector.Z);
}

FString UHyperlinkUtility::DoubleToHexString(double InDouble)
{
	const uint64 Bytes{ NETWORK_ORDER64(*reinterpret_cast<uint64*>(&InDouble)) };

	FString Result{};
	BytesToHex(reinterpret_cast<const uint8*>(&Bytes), sizeof(Bytes), Result);

	return Result;
}

FVector UHyperlinkUtility::HexStringToVector(const FString& InHexString)
{
	check(InHexString.Len() == VectorStringLength);

	FVector RetVector{};
	
	for (int32 Idx{ 0 }; Idx < 3; ++Idx)
	{
		RetVector[Idx] = HexStringToDouble(InHexString.Mid(Idx * DoubleStringLength, DoubleStringLength));
	}

	return RetVector;
}

double UHyperlinkUtility::HexStringToDouble(const FString& InHexString)
{
	check(InHexString.Len() == DoubleStringLength);
	uint64 DoubleAsInt{ FParse::HexNumber64(*InHexString) };
	
	return *reinterpret_cast<double*>(&DoubleAsInt);
}

#if WITH_EDITOR
void UHyperlinkUtility::LogEditorOnlyCall(const TCHAR* FunctionName)
{
	UE_LOG(LogHyperlink, Log, TEXT("Editor only hyperlink utility called at runtime (UHyperlinkUtility::%s)"), FunctionName);
}
#endif //WITH_EDITOR

