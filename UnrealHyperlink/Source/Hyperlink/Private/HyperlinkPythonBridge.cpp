// Copyright (c) 2023-2024 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkPythonBridge.h"

const UHyperlinkPythonBridge* UHyperlinkPythonBridge::Get()
{
	const UHyperlinkPythonBridge* PythonBridge{ nullptr };
	
	if (const UClass* const Class
		{ LoadObject<UClass>(nullptr, TEXT("/Engine/PythonTypes.HyperlinkPythonBridgeImplementation")) })
	{
		PythonBridge = Cast<UHyperlinkPythonBridge>(Class->GetDefaultObject());
	}
	
	return PythonBridge;
}

const UHyperlinkPythonBridge& UHyperlinkPythonBridge::GetChecked()
{
	const UHyperlinkPythonBridge* PythonBridge{ Get() };
	check(PythonBridge);
	return *PythonBridge;
}

