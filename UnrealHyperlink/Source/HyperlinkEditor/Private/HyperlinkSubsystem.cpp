// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkSubsystem.h"

#include "HyperlinkPipeServer.h"

void UHyperlinkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	PipeServer = MakeUnique<FHyperlinkPipeServer>();
}

void UHyperlinkSubsystem::Deinitialize()
{
}


