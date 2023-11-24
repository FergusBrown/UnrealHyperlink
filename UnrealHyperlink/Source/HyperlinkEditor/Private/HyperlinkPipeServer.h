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
	~FHyperlinkPipeServer();
	
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;
};
