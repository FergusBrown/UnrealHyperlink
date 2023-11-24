// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkEditor.h"

#include "HyperlinkPipeServer.h"
#include "HyperlinkSubsystem.h"
#include "Interfaces/IMainFrameModule.h"
#include "Interfaces/IPluginManager.h"
#include "Log.h"
#include "Toolkits/FConsoleCommandExecutor.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
/* Begin Windows includes */
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
/* End Windows includes */

#define LOCTEXT_NAMESPACE "FHyperlinkEditorModule"

FHyperlinkEditorCommands::FHyperlinkEditorCommands()
	: TCommands<FHyperlinkEditorCommands>(
			TEXT("HyperlinkEditor"),
			NSLOCTEXT("Contexts", "HyperlinkEditor", "Hyperlink Editor"),
			NAME_None,
			FAppStyle::GetAppStyleSetName())
{
}

void FHyperlinkEditorCommands::RegisterCommands()
{
	UI_COMMAND(PasteLink, "Paste Link", "Execute a link stored in the clipboard.", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Alt | EModifierKey::Shift, EKeys::V));
}

void FHyperlinkEditorModule::StartupModule()
{
	PipeServer = MakeUnique<FHyperlinkPipeServer>();

	SetupRegistry();
	SetupProtocolHandler();

	// Map actions
	FHyperlinkEditorCommands::Register();
	IMainFrameModule& MainFrame{ FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame")) };
	
	FUICommandList& ActionList{ *MainFrame.GetMainFrameCommandBindings() };

	ActionList.MapAction(
		FHyperlinkEditorCommands::Get().PasteLink,
		FExecuteAction::CreateStatic(&FHyperlinkEditorModule::PasteLink));

	// Console command
	PasteConsoleCommand = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("uhl.PasteLink"),
		TEXT("Execute a link stored in the clipboard"),
		FConsoleCommandDelegate::CreateStatic(&FHyperlinkEditorModule::PasteLink));
}

void FHyperlinkEditorModule::ShutdownModule()
{
    // Unmap actions
	FHyperlinkEditorCommands::Register();
	IMainFrameModule& MainFrame{ FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame")) };
	
	FUICommandList& ActionList{ *MainFrame.GetMainFrameCommandBindings() };
	ActionList.UnmapAction(FHyperlinkEditorCommands::Get().PasteLink);
	FHyperlinkEditorCommands::Unregister();
}

void FHyperlinkEditorModule::SetupRegistry() const
{
	HKEY OutCreatedKey{ nullptr };
	if(RegCreateKey(HKEY_CURRENT_USER, TEXT(R"(SOFTWARE\Classes\unreal)"), &OutCreatedKey) == ERROR_SUCCESS)
	{
		// If we get this far then we can create the rest of the sub keys without checks
		DWORD Temp{ 0 };
		RegSetValue(OutCreatedKey, nullptr, REG_SZ, TEXT("URL:Unreal Hyperlink Protocol"), Temp);
		RegSetValueEx(OutCreatedKey, TEXT("URL Protocol"), 0, REG_SZ, nullptr, Temp);
		RegCreateKey(OutCreatedKey, TEXT("shell"), &OutCreatedKey);
		RegCreateKey(OutCreatedKey, TEXT("open"), &OutCreatedKey);
		RegCreateKey(OutCreatedKey, TEXT("command"), &OutCreatedKey);
		const FString CommandKeyValue{  FString::Format(TEXT(R"("{0}" "%1")"), { GetProtocolHandlerPath() }) }; 
		RegSetValue(OutCreatedKey, nullptr, REG_SZ, *CommandKeyValue, Temp);
	}
	else
	{
		UE_LOG(LogHyperlinkEditor, Error, TEXT("Failed to create registry keys. GLE = %d"), GetLastError());
	}
}

void FHyperlinkEditorModule::SetupProtocolHandler() const
{
	 FPlatformMisc::GetEnvironmentVariable(TEXT("LocalAppData"));
	const FString DestPath{ GetProtocolHandlerPath() };
	if (!FPaths::FileExists(DestPath))
	{
		const TSharedPtr<IPlugin> HyperlinkPlugin{ IPluginManager::Get().FindPlugin(TEXT("UnrealHyperlink")) };
		const FString SourcePath{ HyperlinkPlugin->GetBaseDir() / TEXT("Resources") / TEXT("ProtocolHandler.exe") };
		if (FPaths::FileExists(SourcePath))
		{
			IFileManager::Get().Copy(*DestPath, *SourcePath);
		}
	}
}

FString FHyperlinkEditorModule::GetProtocolHandlerPath()
{
	return FPlatformMisc::GetEnvironmentVariable(TEXT("LocalAppData")) + TEXT(R"(\UnrealHyperlink\ProtocolHandler.exe)");
}

/*static*/void FHyperlinkEditorModule::PasteLink()
{
	FString ClipboardContents{};
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContents);
	GEngine->GetEngineSubsystem<UHyperlinkSubsystem>()->ExecuteLink(ClipboardContents);
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FHyperlinkEditorModule, HyperlinkEditor)