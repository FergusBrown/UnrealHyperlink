// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkPipeServer.h"

#include "Log.h"
/* Begin Windows includes */
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>

#include "HyperlinkSubsystem.h"
#include "Windows/HideWindowsPlatformTypes.h"
/* End Windows includes */

namespace PipeConstants
{
	static constexpr DWORD BufferSize{ 1028 };
}

FHyperlinkPipeServer::FHyperlinkPipeServer()
{
	Thread = TUniquePtr<FRunnableThread>(FRunnableThread::Create(this, TEXT("HyperlinkPipeServer")));
}

FHyperlinkPipeServer::~FHyperlinkPipeServer()
{
	if (Thread.IsValid())
	{
		Thread->Kill();
	}
}

uint32 FHyperlinkPipeServer::Run()
{
	// Retry every second until the server starts
	while (bRunThread && !StartPipeServer())
	{
		FPlatformProcess::Sleep(1.f);
	}
	
	while (bRunThread)
	{
		// Connect to a client (blocks until a client is found)
		UE_LOG(LogHyperlinkEditor, Log, TEXT("Hyperlink Pipe Server waiting for client..."));

		if (!::ConnectNamedPipe(PipeHandle, nullptr))
		{
			UE_LOG(LogHyperlinkEditor, Error, TEXT("Hyperlink Pipe Server ConnectNamedPipe failed, GLE = %d"), ::GetLastError());
			return EXIT_FAILURE;
		}

		// Check bRunThread again to see if we're exiting if we're exiting
		if (bRunThread)
		{
			// Read the contents of the client message
			TCHAR MessageBuffer[PipeConstants::BufferSize];
			DWORD BytesRead{ 0 };
			if (::ReadFile(PipeHandle, MessageBuffer, sizeof(MessageBuffer) - 1, &BytesRead, nullptr))
			{
				// Make sure BytesRead does not exceed our buffer size
				BytesRead = FMath::Min(BytesRead, PipeConstants::BufferSize - 1);

				// Add terminating zero
				MessageBuffer[BytesRead] = TEXT('\0');

				UE_LOG(LogHyperlinkEditor, Log, TEXT("Hyperlink message received: %s"), MessageBuffer);
				AsyncTask(ENamedThreads::GameThread, [=]()
				{
					GEngine->GetEngineSubsystem<UHyperlinkSubsystem>()->ExecuteLink(MessageBuffer);
				});
			}
		}

		// Disconnect from the client so that we can wait for a new one
		::DisconnectNamedPipe(PipeHandle);
	}

	UE_LOG(LogHyperlinkEditor, Log, TEXT("Successfully shutdown Hyperlink Piper Server"));
	return EXIT_SUCCESS;
}

void FHyperlinkPipeServer::Stop()
{
	UE_LOG(LogHyperlinkEditor, Log, TEXT("Beginning Hyperlink Pipe Server shuttdown."));
	
	bRunThread = false;
	// Connect to the pipe server to stop it blocking.
	// If this fails then the server is already shutting down
	::CreateFile(
		*GetPipeName(),         // pipe name
		GENERIC_WRITE,          // desired access
		0,                      // default share more
		nullptr,                // default security attributes
		OPEN_EXISTING,          // creation disposition
		0,                      // no flags
		nullptr);               // no template file
}

FString FHyperlinkPipeServer::GetPipeName()
{
	return FString::Format(TEXT(R"(\\.\pipe\{0}Link)"), { FApp::GetProjectName() } );
}

bool FHyperlinkPipeServer::StartPipeServer()
{
	// Attempt to start the pipe server
	const FString PipeName{ GetPipeName() };
	UE_LOG(LogHyperlinkEditor, Log, TEXT("Setting up pipe server %s"), *PipeName);

	static constexpr DWORD InBufferSize{ PipeConstants::BufferSize * sizeof(TCHAR) };
	static constexpr float Timeout{ 500.f };
	PipeHandle =
		::CreateNamedPipeW
		( 
			*PipeName,					// pipe name 
			PIPE_ACCESS_INBOUND,        // read incoming messages only
			PIPE_TYPE_MESSAGE |         // message-type pipe 
			PIPE_READMODE_MESSAGE |     // message read mode 
			PIPE_WAIT,                  // blocking mode - wait indefinitely for a client with ConnectNamedPipe
			1,                          // Only allow 1 instance to exist at any one time
			0,                          // output buffer size 
			InBufferSize,				// input buffer size 
			Timeout,                    // default client time-out (ms)
			nullptr 					// default security attributes
		);

	const bool bSuccess{ PipeHandle != INVALID_HANDLE_VALUE };
	UE_CLOG(!bSuccess, LogHyperlinkEditor, Error, TEXT("Hyperlink Pipe Server CreateNamedPipe failed, GLE = %d"), ::GetLastError());
	
	return bSuccess;
}