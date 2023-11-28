// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkSettings.h"

#if WITH_EDITOR
#include "HyperlinkUtility.h"
#include "IPythonScriptPlugin.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "EditorUtilityBlueprint.h"
#include "HyperlinkClassEntry.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkSubsystem.h"
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
	if (GIsEditor)
	{
		IAssetRegistry::GetChecked().OnFilesLoaded().AddUObject(this, &UHyperlinkSettings::OnAssetRegistryReady);

		// All C++ classes loaded into memory once all modules are loaded
		// All python classes loaded into memory once python plugin is initialised
		// Python plugin initialisation happens after all modules are loaded so this covers both cases
		IPythonScriptPlugin::Get()->OnPythonInitialized().AddUObject(this, &UHyperlinkSettings::OnPythonInitialised);
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

void UHyperlinkSettings::OnAssetRegistryReady()
{
	RegisterBlueprintClasses();
	PostRegister();
	IAssetRegistry::GetChecked().OnInMemoryAssetCreated().AddUObject(this, &UHyperlinkSettings::OnAssetCreated);
}

void UHyperlinkSettings::OnPythonInitialised()
{
	RegisterInMemoryClasses();
	PostRegister();
}

void UHyperlinkSettings::OnAssetCreated(UObject* InObject)
{
	if (const UEditorUtilityBlueprint* const BP{ Cast<UEditorUtilityBlueprint>(InObject) })
	{
		if (BP->ParentClass->IsChildOf<UHyperlinkDefinition>() &&
			RegisterDefinitionClass(BP->GeneratedClass))
		{
			PostRegister();
		}
	}
}

bool UHyperlinkSettings::RegisterInMemoryClasses()
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

	bInMemoryClassesRegistered = true;
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
	AssetRegistry.GetAssetsByClass(UEditorUtilityBlueprint::StaticClass()->GetClassPathName(), AssetList);
	
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

	bBlueprintClassesRegistered = true;
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
		NewEntry.Identifier = FHyperlinkUtility::CreateClassDisplayString(Class);
		
		RegisteredDefinitions.Emplace(MoveTemp(NewEntry));
		bResult = true;
	}

	return bResult;
}

void UHyperlinkSettings::PostRegister()
{
	// With class registration complete all classes in RegisteredDefinitions should be loaded and we can perform some
	// cleanup and sorting
	if (bBlueprintClassesRegistered && bInMemoryClassesRegistered)
	{
		// Any nullptr can be removed as invalid classes
		RegisteredDefinitions.RemoveAll([](const FHyperlinkClassEntry& Entry)
			{ return Entry.Class.LoadSynchronous() == nullptr; });
	
		// Sort registered definitions so they appear in alphabetical order
		RegisteredDefinitions.Sort([](const FHyperlinkClassEntry& Lhs, const FHyperlinkClassEntry& Rhs)
			{ return Lhs.Class.GetAssetName() < Rhs.Class.GetAssetName(); });

		// Update default config
		SaveConfig(CPF_Config, *GetDefaultConfigFilename());

		// Ensure subsystem is up to date
		GEngine->GetEngineSubsystem<UHyperlinkSubsystem>()->RefreshDefinitions();
	}
}
#endif //WITH_EDITOR
