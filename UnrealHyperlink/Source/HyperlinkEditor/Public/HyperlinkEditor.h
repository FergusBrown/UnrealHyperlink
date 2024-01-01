// Copyright (c) 2023-2024 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HttpResultCallback.h"
#include "HttpRouteHandle.h"
#include "Modules/ModuleManager.h"

class IHttpRouter;
struct FHttpServerRequest;

class FHyperlinkEditorCommands : public TCommands<FHyperlinkEditorCommands>
{
public:
    FHyperlinkEditorCommands();
    
    virtual void RegisterCommands() override;

public:
    TSharedPtr<FUICommandInfo> PasteLink{ nullptr };
};

class FHyperlinkEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    static void RegisterCustomisation();
    
    void RegisterPaste();
    void UnregisterPaste();
    static void PasteLink();

    void StartHttpServer();
    void ShutdownHttpServer();
    static bool HandleHttpRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

    static void ExecuteLinkFromString(const FString& InString);

private:
    FDelegateHandle HttpRouteHandle{};
    IConsoleObject* PasteConsoleCommand{ nullptr };
    
    TSharedPtr<IHttpRouter> HttpRouter{ nullptr };
    FHttpRouteHandle HttpRequestHandle{};
};
