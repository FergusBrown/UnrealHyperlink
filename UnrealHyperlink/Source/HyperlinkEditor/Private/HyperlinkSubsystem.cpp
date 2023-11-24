// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkSubsystem.h"

void UHyperlinkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	LaunchPipeServer();

	// Setup tick to relaunch pipe server later if needed
	TickDelegate = FTickerDelegate::CreateUObject(this, &UHyperlinkSubsystem::Tick);
	static constexpr float TickFrequency{ 10.f }; // Only tick every 10 seconds, we don't need to check server status very often
	TickHandle = FTSTicker::GetCoreTicker().AddTicker(TickDelegate, TickFrequency);
}

void UHyperlinkSubsystem::Deinitialize()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
	if (PipeServerThread.IsValid())
	{
		PipeServerThread->Kill();
	}
}

bool UHyperlinkSubsystem::Tick(float DeltaTime)
{
	LaunchPipeServer();
	
	return true;
}

void UHyperlinkSubsystem::LaunchPipeServer()
{
	if (!PipeServer.IsValid())
	{
		PipeServer = MakeUnique<FHyperlinkPipeServer>();
	}
	if (!PipeServerThread.IsValid())
	{
		PipeServerThread = TUniquePtr<FRunnableThread>(FRunnableThread::Create(PipeServer.Get(), TEXT("HyperlinkPipeServer")));
	}
}


