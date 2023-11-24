// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FHyperlinkPipeServer : public FRunnable
{
public:
	FHyperlinkPipeServer();

	/* FRunnable Interface
	 * See FRunnableThreadWin::Run() for an example of how these functions are used */
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;

private:
	static FString GetPipeName();
	
private:
	TUniquePtr<FRunnableThread> Thread{ nullptr };
	bool bRunThread{ true };
	Windows::HANDLE PipeHandle{ nullptr };
};
