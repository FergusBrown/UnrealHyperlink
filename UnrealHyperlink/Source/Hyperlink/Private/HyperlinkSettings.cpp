// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkSettings.h"

#include "HyperlinkUtility.h"

#if WITH_EDITOR
#include "AssetRegistry/IAssetRegistry.h"
#include "HyperlinkClassEntry.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkSubsystem.h"
#include "Kismet2/KismetEditorUtilities.h"
#endif //WITH_EDITOR


class FAssetRegistryModule;

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
	if (GIsEditor)
	{
		FCoreDelegates::OnAllModuleLoadingPhasesComplete.AddUObject(this, &UHyperlinkSettings::OnAllModulesLoaded);
		IAssetRegistry::GetChecked().OnFilesLoaded().AddUObject(this, &UHyperlinkSettings::OnAssetRegistryReady);
	}
#endif //WITH_EDITOR
}

#if WITH_EDITOR
FName UHyperlinkSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

void UHyperlinkSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
		
	GEngine->GetEngineSubsystem<UHyperlinkSubsystem>()->RefreshDefinitions();
}

#endif //WITH_EDITOR

TConstArrayView<FHyperlinkClassEntry> UHyperlinkSettings::GetRegisteredDefinitions() const
{
	return TConstArrayView<FHyperlinkClassEntry>(RegisteredDefinitions);
}

#if WITH_EDITOR
void UHyperlinkSettings::OnAllModulesLoaded()
{
	// Remove null classes
	RegisteredDefinitions.RemoveAll([](const FHyperlinkClassEntry& Entry){ return Entry.Class == nullptr; });

	if (RegisterCppClasses())
	{
		PostRegister();
	}
}

void UHyperlinkSettings::OnAssetRegistryReady()
{
	if (RegisterBlueprintClasses())
	{
		PostRegister();
	}

	FKismetEditorUtilities::RegisterOnBlueprintCreatedCallback(this, UHyperlinkDefinition::StaticClass(),
			FKismetEditorUtilities::FOnBlueprintCreated::CreateUObject(this, &UHyperlinkSettings::OnBlueprintCreated));
}

void UHyperlinkSettings::OnBlueprintCreated(UBlueprint* InBlueprint)
{
	RegisterBlueprintClasses();
	PostRegister();
}

bool UHyperlinkSettings::RegisterCppClasses()
{
	bool bResult{ false };
	
	for(TObjectIterator<UClass> It; It; ++It)
	{
		if(It->IsChildOf(UHyperlinkDefinition::StaticClass()) &&
			!It->HasAnyClassFlags(CLASS_Abstract | CLASS_CompiledFromBlueprint))
		{
			bResult = RegisterDefinitionClass(*It);
		}
	}

	return bResult;
}

bool UHyperlinkSettings::RegisterBlueprintClasses()
{
	bool bResult{ false };
	
	const IAssetRegistry& AssetRegistry{ IAssetRegistry::GetChecked() };
	
	// Get set of all class names deriving from UHyperlinkDefinition
	TSet<FTopLevelAssetPath> DerivedClassPaths{};
	const FName BaseClassName{ UHyperlinkDefinition::StaticClass()->GetFName() };
	const FName BaseClassPackageName{ UHyperlinkDefinition::StaticClass()->GetPackage()->GetFName() };
	
	TArray<FTopLevelAssetPath> BaseClassPaths{};
	BaseClassPaths.Emplace(BaseClassPackageName, BaseClassName);
	
	AssetRegistry.GetDerivedClassNames(BaseClassPaths, TSet<FTopLevelAssetPath>(), DerivedClassPaths);

	TArray<FAssetData> AssetList{};
	AssetRegistry.GetAssetsByClass(UBlueprint::StaticClass()->GetClassPathName(), AssetList);
	// Iterate over retrieved blueprint assets
	for (const FAssetData& Asset : AssetList)
	{
		// Get the the blueprint's parent class to check
		const FAssetTagValueRef ParentClassPath{
			Asset.TagsAndValues.FindTag(FBlueprintTags::ParentClassPath) };
		
		if(ParentClassPath.IsSet())
		{
			// Convert path to just the name part
			const FTopLevelAssetPath ClassObjectPath{
				FPackageName::ExportTextPathToObjectPath(ParentClassPath.GetValue()) };
			
			if (DerivedClassPaths.Contains(ClassObjectPath))
			{
				const FAssetTagValueRef GeneratedClassPath{
					Asset.TagsAndValues.FindTag(FBlueprintTags::GeneratedClassPath) };
				
				const TSoftObjectPtr<UClass> SoftObjectPtr(GeneratedClassPath.GetValue());
				if (UClass* const Class{ SoftObjectPtr.LoadSynchronous() })
				{
					bResult = RegisterDefinitionClass(Class);
				}
			}
		}
	}

	return bResult;
}

bool UHyperlinkSettings::RegisterDefinitionClass(UClass* const Class)
{
	bool bResult{ false };
	
	if (!RegisteredDefinitions.FindByPredicate([=](const FHyperlinkClassEntry& Entry)
		{ return Entry.Class == Class; }))
	{
		FHyperlinkClassEntry NewEntry{};
		NewEntry.Class = Class;
		NewEntry.Identifier = UHyperlinkUtility::CreateClassDisplayString(Class);
		
		RegisteredDefinitions.Emplace(MoveTemp(NewEntry));
		bResult = true;
	}

	return bResult;
}

void UHyperlinkSettings::PostRegister()
{
	// Sort registered definitions so they appear in alphabetical order
	RegisteredDefinitions.Sort([](const FHyperlinkClassEntry& Lhs, const FHyperlinkClassEntry& Rhs)
		{ return Lhs.Class->GetName() < Rhs.Class->GetName(); });
	
	// Ensure subsystem is up to date
	GEngine->GetEngineSubsystem<UHyperlinkSubsystem>()->RefreshDefinitions();
}
#endif //WITH_EDITOR
