// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkSettings.h"

void UHyperlinkSettings::PostInitProperties()
{
	Super::PostInitProperties();
	// Fill project identifier if it's empty
	if (ProjectIdentifier.IsEmpty())
	{
		ProjectIdentifier = FApp::GetProjectName();
		const FString DefaultConfigFile{ GetDefaultConfigFilename() };
		SaveConfig(CPF_Config, *DefaultConfigFile);
	}
}

FName UHyperlinkSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}
