// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkDefinitionSettings.h"

#include "HyperlinkSettings.h"

FName UHyperlinkDefinitionSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

bool UHyperlinkDefinitionSettings::SupportsAutoRegistration() const
{
	return false;
}
