#include <iostream>
#include <regex>
#include "windows.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Provided " << argc - 1 << " arguments, expected 1 argument." << std::endl;
        return EXIT_FAILURE;
    }

    std::string link{ argv[1] };
    std::regex projectPattern{ R"(^unreal:\/\/(\w+)\/.*)" };

    std::smatch match{};
    
    if (!std::regex_match(link, match, projectPattern))
    {
        std::cout << "Provided link is not in the format unreal://PROJECT_NAME/..." << std::endl;
        return EXIT_FAILURE;
    }

    // Create pipe name in format \\.\pipe\PipeName
    // Add "Link" to the pipe name to avoid potential conflicts with other services
    std::string pipeName{ R"(\\.\pipe\)" + match.str(1) + "Link" };

    // Connect to pipe server
    constexpr DWORD access{ GENERIC_WRITE };
    constexpr DWORD shareMode{ 0 };
    constexpr LPSECURITY_ATTRIBUTES securityAttributes{ nullptr };
    constexpr DWORD creationDisposition{ OPEN_EXISTING };
    constexpr DWORD flags{ 0 };
    constexpr HANDLE templateFile{ nullptr };

    HANDLE pipeHandle
    { 
        CreateFileA(
            pipeName.c_str(), 
            access, 
            shareMode, 
            securityAttributes, 
            creationDisposition, 
            flags, 
            templateFile) 
    };
    
    // If we fail to connect then pipe is likely busy
    // Don't wait for busy pipe as this could result in confusing behaviour
    if (pipeHandle == INVALID_HANDLE_VALUE)
    {
        std::cout << "Failed to connect to pipe server " << pipeName << " GLE = " << GetLastError() << std::endl;
        return EXIT_FAILURE;
    }

    // Send message to pipe server
    LPCSTR message{ link.c_str() };
    const DWORD bufferSize{ static_cast<DWORD>((strlen(message) + 1) * sizeof(char)) }; // +1 is for \0 terminator
    DWORD writtenBytes{};
    constexpr LPOVERLAPPED overlapped{ nullptr };

    if (!WriteFile(pipeHandle, message, bufferSize, &writtenBytes, overlapped))
    {
        std::cout << "Failed to write to pipe server " << pipeName << " GLE = " << GetLastError() << std::endl;
        return EXIT_FAILURE;
    }

    CloseHandle(pipeHandle);
    return EXIT_SUCCESS;
}