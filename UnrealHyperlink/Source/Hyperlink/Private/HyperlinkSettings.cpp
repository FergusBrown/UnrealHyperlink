// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkSettings.h"

#include "HyperlinkFormat.h"
#if WITH_EDITOR
#include "HyperlinkDefinition.h"
#include "HyperlinkDefinitionSettings.h"
#include "ISettingsModule.h"
#endif //WITH_EDITOR


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
#if WITH_EDITOR
	InitDefinitionSettings();
#endif //WITH_EDITOR
}

#if WITH_EDITOR
FName UHyperlinkSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

void UHyperlinkSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropName{ PropertyChangedEvent.Property->GetFName() };
	if (PropName == GET_MEMBER_NAME_CHECKED(UHyperlinkSettings, RegisteredDefinitions))
	{
		InitDefinitionSettings();
	}
}

void UHyperlinkSettings::PreEditChange(FProperty* PropertyAboutToChange)
{
	const FName PropName{ PropertyAboutToChange->GetFName() };
	if (PropName == GET_MEMBER_NAME_CHECKED(UHyperlinkSettings, RegisteredDefinitions))
	{
		ISettingsModule& SettingsModule = FModuleManager::GetModuleChecked<ISettingsModule>(TEXT("Settings"));
		for (const TSubclassOf<UHyperlinkDefinition> Def : RegisteredDefinitions)
		{
			if (Def)
			{
				if (const TSubclassOf<UHyperlinkDefinitionSettings> SettingsClass{ GetDefault<UHyperlinkDefinition>(Def)->GetSettingsClass() })
				{
					const UDeveloperSettings* Settings{ GetDefault<UDeveloperSettings>(SettingsClass) };
					SettingsModule.UnregisterSettings(Settings->GetContainerName(), Settings->GetCategoryName(), Settings->GetSectionName());
				}
			}
		}
	}
}

#endif //WITH_EDITOR

const TSet<TSubclassOf<UHyperlinkDefinition>>& UHyperlinkSettings::GetRegisteredDefinitions() const
{
	return RegisteredDefinitions;
}

FString UHyperlinkSettings::GetLinkGenerationBase() const
{
	const FString LinkBase
	{
		LinkHandlingMethod == EHyperlinkHandlingMethod::Local
			? FHyperlinkFormat::ApplicationBase
			: LinkHandlerAddress
	};

	return LinkBase / ProjectIdentifier;
}

#if WITH_EDITOR
void UHyperlinkSettings::InitDefinitionSettings() const
{
	ISettingsModule& SettingsModule = FModuleManager::GetModuleChecked<ISettingsModule>(TEXT("Settings"));
	for (const TSubclassOf<UHyperlinkDefinition> Def : RegisteredDefinitions)
	{
		if (Def)
		{
			if (const TSubclassOf<UHyperlinkDefinitionSettings> SettingsClass{ GetDefault<UHyperlinkDefinition>(Def)->GetSettingsClass() })
			{
				UDeveloperSettings* Settings{ GetMutableDefault<UDeveloperSettings>(SettingsClass) };
				TSharedPtr<SWidget> CustomWidget{ Settings->GetCustomSettingsWidget() };
				if (CustomWidget.IsValid())
				{
					SettingsModule.RegisterSettings(Settings->GetContainerName(), Settings->GetCategoryName(), Settings->GetSectionName(),
					                                Settings->GetSectionText(),
					                                Settings->GetSectionDescription(),
					                                CustomWidget.ToSharedRef());
				}
				else
				{
					SettingsModule.RegisterSettings(Settings->GetContainerName(), Settings->GetCategoryName(), Settings->GetSectionName(),
					                                Settings->GetSectionText(),
					                                Settings->GetSectionDescription(),
					                                Settings);
				}
			}
		}
	}
}
#endif //WITH_EDITOR
