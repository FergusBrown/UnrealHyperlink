// Copyright (c) 2023-2024 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkEditor.h"

#include "Customization/HyperlinkSettingsCustomization.h"
#include "HttpServerModule.h"
#include "HttpServerRequest.h"
#include "HyperlinkSettings.h"
#include "HyperlinkSubsystem.h"
#include "IHttpRouter.h"
#include "Interfaces/IMainFrameModule.h"
#include "LogHyperlinkEditor.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

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
	RegisterCustomisation();
	RegisterPaste();
	StartHttpServer();
}

void FHyperlinkEditorModule::ShutdownModule()
{
	ShutdownHttpServer();
    UnregisterPaste();
}

void FHyperlinkEditorModule::RegisterCustomisation()
{
	FPropertyEditorModule& PropertyEditorModule =
	FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	PropertyEditorModule.RegisterCustomClassLayout(UHyperlinkSettings::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FHyperlinkSettingsCustomization::MakeInstance));
}

void FHyperlinkEditorModule::RegisterPaste()
{
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

void FHyperlinkEditorModule::UnregisterPaste()
{
	IMainFrameModule* const MainFrame{ FModuleManager::LoadModulePtr<IMainFrameModule>(TEXT("MainFrame")) };
	if (MainFrame)
	{
		FUICommandList& ActionList = *MainFrame->GetMainFrameCommandBindings();
		ActionList.UnmapAction(FHyperlinkEditorCommands::Get().PasteLink);
	}
	
	FHyperlinkEditorCommands::Unregister();

	IConsoleManager::Get().UnregisterConsoleObject(PasteConsoleCommand);
	PasteConsoleCommand = nullptr;
}

/*static*/void FHyperlinkEditorModule::PasteLink()
{
	FString ClipboardContents{};
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContents);
	ExecuteLinkFromString(ClipboardContents);
}

void FHyperlinkEditorModule::StartHttpServer()
{
	if (!HttpRouter.IsValid())
	{
		// TODO: we need to restart the server if the user changes this port
		const uint32 ServerPort{ GetDefault<UHyperlinkSettings>()->GetLocalServerPort() };
		HttpRouter = FHttpServerModule::Get().GetHttpRouter(ServerPort, /*bFailOnBindFailure = */true);
		if (HttpRouter.IsValid())
		{
			// Use route for versioning
			HttpRequestHandle = HttpRouter->BindRoute(
				FHttpPath(FString::Printf(TEXT("/%s"), *GetDefault<UHyperlinkSettings>()->GetProjectIdentifier())),
				EHttpServerRequestVerbs::VERB_GET,
				HandleHttpRequest);

			FHttpServerModule::Get().StartAllListeners();
		}
		else
		{
			UE_LOG(LogHyperlinkEditor, Error, TEXT("Hyperlink local server couldn't be started on port %d"),
				ServerPort);
		}
	}
}

void FHyperlinkEditorModule::ShutdownHttpServer()
{
	if (HttpRouter.IsValid())
	{
		HttpRouter->UnbindRoute(HttpRequestHandle);
	}

	HttpRequestHandle.Reset();
	HttpRouter.Reset();
}

bool FHyperlinkEditorModule::HandleHttpRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	const FString& PathString = Request.RelativePath.GetPath();
	ExecuteLinkFromString(PathString);

	// Redirect to the local URL scheme as a workaround to close the opened tab (this only works on chrome but it's
	// better than nothing
	TUniquePtr<FHttpServerResponse> Response{ MakeUnique<FHttpServerResponse>() };
	Response->Headers.Add(TEXT("Location"), { TEXT("unrealhyperlink://open") });
	Response->Code = EHttpServerResponseCodes::Moved;
	
	OnComplete(MoveTemp(Response));
	return true; // true = request handled
}

/*static*/void FHyperlinkEditorModule::ExecuteLinkFromString(const FString& InString)
{
	if (UHyperlinkSubsystem* const HyperlinkSubsystem{ GEngine->GetEngineSubsystem<UHyperlinkSubsystem>() })
	{
		HyperlinkSubsystem->ExecuteLink(InString);
	}
	else
	{
		UE_LOG(LogHyperlinkEditor, Error, TEXT("Could not execute link, could not find Hyperlink Subsystem!"));
	}
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FHyperlinkEditorModule, HyperlinkEditor)