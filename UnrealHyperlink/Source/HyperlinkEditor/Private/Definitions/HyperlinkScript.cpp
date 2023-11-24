// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "Definitions/HyperlinkScript.h"

#include "ContentBrowserModule.h"
#include "EditorUtilityBlueprint.h"
#include "EditorUtilitySubsystem.h"
#include "HyperlinkCommonPayload.h"
#include "HyperlinkUtility.h"
#include "IContentBrowserSingleton.h"
#include "JsonObjectConverter.h"

#define LOCTEXT_NAMESPACE "HyperlinkScript"

FHyperlinkScriptCommands::FHyperlinkScriptCommands()
	: TCommands<FHyperlinkScriptCommands>(
		TEXT("HyperlinkScript"),
		NSLOCTEXT("Contexts", "HyperlinkScript", "Hyperlink Script"),
		NAME_None,
		FAppStyle::GetAppStyleSetName())
{
}

void FHyperlinkScriptCommands::RegisterCommands()
{
	UI_COMMAND(CopyBlutilityLink, "Copy Script Link", "Copy a link to run the selected Editor Utility Blueprint",
		EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE

void UHyperlinkScript::Initialize()
{
	FHyperlinkScriptCommands::Register();
	ScriptCommands = MakeShared<FUICommandList>();
	ScriptCommands->MapAction(
		FHyperlinkScriptCommands::Get().CopyBlutilityLink,
		FExecuteAction::CreateUObject(this, &UHyperlinkDefinition::CopyLink),
		FCanExecuteAction(), FIsActionChecked(),
		FIsActionButtonVisible::CreateStatic(&UHyperlinkScript::IsBlutilitySelected));
	
	FHyperlinkUtility::AddHyperlinkSubMenuAndEntry(TEXT("ContentBrowser.AssetContextMenu"),
		TEXT("CommonAssetActions"), ScriptCommands, FHyperlinkScriptCommands::Get().CopyBlutilityLink);
}

void UHyperlinkScript::Deinitialize()
{
	// TODO
}

TSharedPtr<FJsonObject> UHyperlinkScript::GeneratePayload() const
{
	TSharedPtr<FJsonObject> Payload{ nullptr };

	// TODO: implement sort of content browser operation in utilities
	const FContentBrowserModule& ContentBrowser =
		FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FAssetData> SelectedAssets{};
	ContentBrowser.Get().GetSelectedAssets(SelectedAssets);
	
	TArray<FAssetData> FilteredAssets
	{
		SelectedAssets.FilterByPredicate([](const FAssetData& AssetData)
		{
			return AssetData.AssetClassPath == UEditorUtilityBlueprint::StaticClass()->GetClassPathName();
		})
	};
	if (SelectedAssets.Num() > 0 )
	{
		const FHyperlinkNamePayload PayloadStruct{ SelectedAssets[0].PackageName };
		Payload =  FJsonObjectConverter::UStructToJsonObject(PayloadStruct);
	}

	return Payload;
}

void UHyperlinkScript::ExecutePayload(const TSharedRef<FJsonObject>& InPayload)
{
	FHyperlinkNamePayload PayloadStruct{};
	if (FJsonObjectConverter::JsonObjectToUStruct(InPayload, &PayloadStruct))
	{
		UObject* const LoadedBlutility{ FHyperlinkUtility::LoadObject(PayloadStruct.Name.ToString()) };

		UEditorUtilitySubsystem* const EditorUtilitySubsystem{ GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>() };
		if (EditorUtilitySubsystem)
		{
			EditorUtilitySubsystem->TryRun(LoadedBlutility);
		}
	}
}

bool UHyperlinkScript::IsBlutilitySelected()
{
	const FContentBrowserModule& ContentBrowser =
	FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FAssetData> SelectedAssets{};
	ContentBrowser.Get().GetSelectedAssets(SelectedAssets);
	
	return SelectedAssets.ContainsByPredicate([](const FAssetData& AssetData)
		{
			return AssetData.AssetClassPath == UEditorUtilityBlueprint::StaticClass()->GetClassPathName();
		});
}
