#define UNICODE // Use unicode for windows API calls 

#include <iostream>
#include <windows.h>

int wmain(int argc, wchar_t* argv[])
{
    if (argc != 2)
    {
        std::wcout << L"Provided " << argc - 1 << L" arguments, expected 1 argument." << std::endl;
        return EXIT_FAILURE;
    }

    std::wstring pipeName{ LR"(\\.\pipe\)" };
    pipeName += argv[1];
    pipeName += L"Link";

    std::wcout << L"Setting up pipe server " << pipeName << std::endl;

    static constexpr DWORD bufSize{ 4096 };
    HANDLE pipeHandle
    {
        ::CreateNamedPipe( 
        pipeName.c_str(),           // pipe name 
        PIPE_ACCESS_INBOUND,        // read/write access 
        PIPE_TYPE_MESSAGE |         // message-type pipe 
        PIPE_READMODE_MESSAGE |     // message read mode 
        PIPE_WAIT,                  // blocking mode - wait indefinitely for a client with ConnectNamedPipe
        1,                          // Only allow 1 instance to exist at any one time
        0,                          // output buffer size 
        bufSize * sizeof(wchar_t),  // input buffer size 
        500,                        // default client time-out (ms)
        nullptr)                    // default security attributes
    };

    if (pipeHandle == INVALID_HANDLE_VALUE)
    {
        std::wcout << L"CreateNamedPipe failed, GLE = " << GetLastError() << std::endl;
        return EXIT_FAILURE;
    }

    while (true)
    {
        // Connect to a client (blocks until a client is found)
        std::wcout << L"Waiting for client..." << std::endl;
        if (!::ConnectNamedPipe(pipeHandle, nullptr))
        {
            std::wcout << L"ConnectNamedPipe failed, GLE = " << GetLastError() << std::endl;
            return EXIT_FAILURE;
        }

        char buffer[bufSize];
        DWORD bytesRead{};

        if (::ReadFile(pipeHandle, buffer, sizeof(buffer) - 1, &bytesRead, nullptr))
        {
            // add terminating zero
            buffer[bytesRead] = '\0';

            // do something with data in buffer
            std::wcout << L"Message received:\n" << buffer << std::endl;
        }

        ::DisconnectNamedPipe(pipeHandle);
    }
}