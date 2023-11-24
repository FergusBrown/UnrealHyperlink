// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "HyperlinkPipeServer.h"
#include "HyperlinkSubsystem.generated.h"

/**
 * Subsystem to manager the pipe server which receives links
 * Periodically checks that the pipe server is running and restarts it if needed
 */
UCLASS()
class UHyperlinkSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
private:
	bool Tick(float DeltaTime);
	
private:
	/** Holds a delegate to be invoked on a tick. */
	FTickerDelegate TickDelegate{};
	FTSTicker::FDelegateHandle TickHandle{};

	/* Pipe server instance */
	TUniquePtr<FHyperlinkPipeServer> PipeServer{ nullptr };
};
