// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"

/**
 * Named pipe server for receiving link string from the protocol handler
 */
class FHyperlinkPipeServer : public FRunnable
{
public:
	FHyperlinkPipeServer();
	~FHyperlinkPipeServer();

	/* FRunnable Interface
	 * See FRunnableThreadWin::Run() for an example of how these functions are used */
	virtual uint32 Run() override;
	virtual void Stop() override;

private:
	static FString GetPipeName();
	bool StartPipeServer();
	
private:
	TUniquePtr<FRunnableThread> Thread{ nullptr };
	bool bRunThread{ true };
	Windows::HANDLE PipeHandle{ nullptr };
};
