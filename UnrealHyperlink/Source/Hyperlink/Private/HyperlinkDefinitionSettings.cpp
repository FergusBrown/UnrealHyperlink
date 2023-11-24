// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkDefinitionSettings.h"

#include "HyperlinkSubsystem.h"

FName UHyperlinkDefinitionSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

bool UHyperlinkDefinitionSettings::SupportsAutoRegistration() const
{
	return false;
}

void UHyperlinkDefinitionSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	GEngine->GetEngineSubsystem<UHyperlinkSubsystem>()->RefreshDefinitions();
}

