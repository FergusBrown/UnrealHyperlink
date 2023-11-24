// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkEditor.h"

#include "HyperlinkPipeServer.h"
#include "Interfaces/IPluginManager.h"
#include "Log.h"
/* Begin Windows includes */
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
/* End Windows includes */

#define LOCTEXT_NAMESPACE "FHyperlinkEditorModule"

void FHyperlinkEditorModule::StartupModule()
{
	PipeServer = MakeUnique<FHyperlinkPipeServer>();

	SetupRegistry();
	SetupProtcolHandler();
}

void FHyperlinkEditorModule::ShutdownModule()
{
    
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

void FHyperlinkEditorModule::SetupProtcolHandler() const
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

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FHyperlinkEditorModule, HyperlinkEditor)