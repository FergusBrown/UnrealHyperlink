// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkPipeServer.h"

FHyperlinkPipeServer::FHyperlinkPipeServer()
{
}

FHyperlinkPipeServer::~FHyperlinkPipeServer()
{
}

bool FHyperlinkPipeServer::Init()
{
	return FRunnable::Init();
}

uint32 FHyperlinkPipeServer::Run()
{
	return EXIT_SUCCESS;
}

void FHyperlinkPipeServer::Stop()
{
	FRunnable::Stop();
}

void FHyperlinkPipeServer::Exit()
{
	FRunnable::Exit();
}
