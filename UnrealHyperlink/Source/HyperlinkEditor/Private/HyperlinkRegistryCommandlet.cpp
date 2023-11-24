// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkRegistryCommandlet.h"

#include "Log.h"

/* Begin Windows includes */
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
/* End Windows includes */

int32 UHyperlinkRegistryCommandlet::Main(const FString& Params)
{
	// TODO: Check we're running in elevated mode
	
	static const FString ProtocolHandlerPath{ TEXT(R"(%LocalAppData%\UnrealHyperlink)") };
	static const FString RegistryRoot{ TEXT(R"(Computer\HKEY_CLASSES_ROOT\UnrealHyperlink)") };
	static const FString RegistrySubKey{ RegistryRoot + TEXT(R"(UnrealHyperlink\shell\open\command)") };
	
	// create root path
	HKEY OutCreatedKey{ nullptr };
	if(RegCreateKey(HKEY_CLASSES_ROOT, *RegistrySubKey, &OutCreatedKey) != ERROR_SUCCESS)
	{
		UE_LOG(LogHyperlinkEditor, Error, TEXT("Failed to create registry key."));
		return EXIT_FAILURE;

	}
	
	// Check registry value exists
	return EXIT_SUCCESS;
}
