// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkSettings.h"

UHyperlinkSettings::UHyperlinkSettings()
{
	// Fill project identifier if it's empty
	if (ProjectIdentifier.IsEmpty())
	{
		// TODO: config doesn't save here for some reason
		ProjectIdentifier = FApp::GetProjectName();
		SaveConfig();
	}
}

FName UHyperlinkSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}
