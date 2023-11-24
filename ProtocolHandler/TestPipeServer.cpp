#include <iostream>
#include "windows.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Provided " << argc - 1 << " arguments, expected 1 argument." << std::endl;
        return EXIT_FAILURE;
    }

    std::string pipeServerName{ R"(\\.\pipe\)" };
    pipeServerName += argv[1];
    pipeServerName += "Link";

    std::cout << "Setting up pipe server " << pipeServerName << std::endl;

    constexpr DWORD inBufferSize{ 4096 };
    HANDLE pipeHandle
    {
        CreateNamedPipeA( 
        pipeServerName.c_str(),     // pipe name 
        PIPE_ACCESS_INBOUND,        // read/write access 
        PIPE_TYPE_MESSAGE |         // message-type pipe 
        PIPE_READMODE_MESSAGE |     // message read mode 
        PIPE_WAIT,                  // blocking mode - wait indefinitely for a client with ConnectNamedPipe
        1,                          // Only allow 1 instance to exist at any one time
        0,                          // output buffer size 
        inBufferSize * sizeof(char),// input buffer size 
        500,                        // default client time-out (ms)
        nullptr)                    // default security attributes
    };

    if (pipeHandle == INVALID_HANDLE_VALUE)
    {
        std::cout << "CreateNamedPipe failed, GLE = " << GetLastError() << std::endl;
        return EXIT_FAILURE;
    }

    // Connect to a client (blocks until a client is found)
    std::cout << "Waiting for client..." << std::endl;
    if (!ConnectNamedPipe(pipeHandle, nullptr))
    {
        std::cout << "ConnectNamedPipe failed, GLE = " << GetLastError() << std::endl;
        return EXIT_FAILURE;
    }

    char buffer[inBufferSize];
    DWORD bytesRead{};
    while (ReadFile(pipeHandle, buffer, sizeof(buffer) - 1, &bytesRead, nullptr) != FALSE)
    {
        /* add terminating zero */
        buffer[bytesRead] = '\0';

        /* do something with data in buffer */
        printf("%s", buffer);
    }

    CloseHandle(pipeHandle);
    return EXIT_SUCCESS;
}