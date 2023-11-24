#define UNICODE // Use unicode for windows API calls 

#include <iostream>
#include <regex>
#include <windows.h>

int wmain(int argc, wchar_t* argv[])
{
    if (argc != 2)
    {
        std::wcout << L"Provided " << argc - 1 << L" arguments, expected 1 argument." << std::endl;
        return EXIT_FAILURE;
    }

    const std::wstring link{ argv[1] };
    const std::wregex projectPattern{ LR"(^unreal:\/\/(\w+)\/.*)" };

    std::wsmatch match{};
    
    if (!std::regex_match(link, match, projectPattern))
    {
        std::wcout << L"Provided link is not in the format unreal://PROJECT_NAME/..." << std::endl;
        return EXIT_FAILURE;
    }

    // Create pipe name in format \\.\pipe\PipeName
    // Add "Link" to the pipe name to avoid potential conflicts with other services
    std::wstring pipeName{ LR"(\\.\pipe\)" + match.str(1) + L"Link" };

    // Connect to pipe server
    HANDLE pipeHandle
    { 
        ::CreateFile(
            pipeName.c_str(),       // pipe name
            GENERIC_WRITE,          // desired access
            0,                      // default share more
            nullptr,                // default security attributes
            OPEN_EXISTING,          // creation disposition
            0,                      // no flags
            nullptr)                // no template file
    };
    
    // If we fail to connect then pipe is likely busy
    // Don't wait for busy pipe as this could result in confusing behaviour
    if (pipeHandle == INVALID_HANDLE_VALUE)
    {
        std::wcout << L"Failed to connect to pipe server " << pipeName << L" GLE = " << GetLastError() << std::endl;
        return EXIT_FAILURE;
    }

    // Write message to pipe server
    const wchar_t* message{ link.c_str() };
    const DWORD messageSize{ static_cast<DWORD>((link.length() + 1) * sizeof(wchar_t)) }; // + 1 is for \0 terminator
    DWORD bytesWritten{};

    if (!::WriteFile(pipeHandle, message, messageSize, &bytesWritten, nullptr))
    {
        std::wcout << L"Failed to write message to pipe server " << pipeName << L" GLE = " << GetLastError() << std::endl;
        return EXIT_FAILURE;
    }

    ::CloseHandle(pipeHandle);
    return EXIT_SUCCESS;
}