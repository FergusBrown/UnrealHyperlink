// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkSubsystem.h"

void UHyperlinkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// Setup tick
	TickDelegate = FTickerDelegate::CreateUObject(this, &UHyperlinkSubsystem::Tick);
	static constexpr float TickFrequency{ 10.f }; // Only tick every 10 seconds, we don't need to check server status very often
	TickHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate, TickFrequency);

	// Create pipe server instance
	PipeServer = MakeUnique<FHyperlinkPipeServer>();
}

void UHyperlinkSubsystem::Deinitialize()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
}

bool UHyperlinkSubsystem::Tick(float DeltaTime)
{
	// Setup the pipe server if it's not running
	
	
	return true;
}


