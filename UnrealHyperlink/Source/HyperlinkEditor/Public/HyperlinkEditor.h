﻿// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FHyperlinkPipeServer;

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
    /* Create the registry values required to launch the protocol handler */
    void SetupRegistry() const;

    /* Copy the protocol handler exe to the required directory */
    void SetupProtocolHandler() const;

    static FString GetProtocolHandlerPath();

private:
    TUniquePtr<FHyperlinkPipeServer> PipeServer{ nullptr };
};
