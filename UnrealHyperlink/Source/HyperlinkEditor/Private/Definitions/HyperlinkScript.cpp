// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "Definitions/HyperlinkScript.h"

#include "ContentBrowserModule.h"
#include "EditorUtilityBlueprint.h"
#include "EditorUtilitySubsystem.h"
#include "HyperlinkCommonPayload.h"
#include "HyperlinkPythonBridge.h"
#include "HyperlinkUtility.h"
#include "IContentBrowserSingleton.h"
#include "IPythonScriptPlugin.h"
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

TSharedPtr<FJsonObject> UHyperlinkScript::GeneratePayload(const TArray<FString>& Args) const
{
	TSharedPtr<FJsonObject> Payload{ nullptr };

	if (Args.Num() > 0)
	{
		Payload = GenerateScriptPayload(Args[0]);
	}
	else
	{
		Payload = GeneratePayloadFromSelectedBlutility();
	}

	return Payload;
}

void UHyperlinkScript::ExecutePayload(const TSharedRef<FJsonObject>& InPayload)
{
	FHyperlinkNamePayload PayloadStruct{};
	if (FJsonObjectConverter::JsonObjectToUStruct(InPayload, &PayloadStruct) &&
		UserConfirmedScriptExecution(PayloadStruct.Name.ToString()))
	{
		const FString ScriptPath{ PayloadStruct.Name.ToString() };
		if (ScriptPath.EndsWith(TEXT(".py")))
		{
			IPythonScriptPlugin::Get()->ExecPythonCommand(*ScriptPath);
		}
		else // This is a path for a blutility
		{
			UObject* const LoadedBlutility{ FHyperlinkUtility::LoadObject(PayloadStruct.Name.ToString()) };

			UEditorUtilitySubsystem* const EditorUtilitySubsystem{ GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>() };
			if (EditorUtilitySubsystem)
			{
				EditorUtilitySubsystem->TryRun(LoadedBlutility);
			}
		}
	}
}

TSharedPtr<FJsonObject> UHyperlinkScript::GenerateScriptPayload(FString ScriptPath)
{
	// Ensure the provided path only has forward slashes
	ScriptPath.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	
	// If this path is listed in sys.path then we can just use the relative path for this script
	for (const FString& SystemPath : UHyperlinkPythonBridge::GetChecked().GetSystemPaths())
	{
		// If successful remove any remaining forward slashes and exit
		if (ScriptPath.RemoveFromStart(SystemPath))
		{
			break;
		}
	}
	
	const FHyperlinkNamePayload PayloadStruct{ FName(ScriptPath) };
	return FJsonObjectConverter::UStructToJsonObject(PayloadStruct);
}

TSharedPtr<FJsonObject> UHyperlinkScript::GeneratePayloadFromSelectedBlutility()
{
	TSharedPtr<FJsonObject> Payload{ nullptr };
	
	// TODO: implement this sort of content browser operation in utilities (get selected assets of type)
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
		Payload = FJsonObjectConverter::UStructToJsonObject(PayloadStruct);
	}

	return Payload;
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

bool UHyperlinkScript::UserConfirmedScriptExecution(const FString& ScriptName)
{
	const FString DialogMessage
		{ FString::Printf(TEXT("Script execution via link requested. Execute this script?\n\n%s"), *ScriptName) };
	
	const EAppReturnType::Type Choice
		{ FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString(DialogMessage)) };

	return Choice == EAppReturnType::Type::Yes;
}
