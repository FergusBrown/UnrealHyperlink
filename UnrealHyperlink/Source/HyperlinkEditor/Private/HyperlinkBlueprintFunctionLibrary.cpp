// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkBlueprintFunctionLibrary.h"

#include "HyperlinkUtility.h"
#include "JsonObjectWrapper.h"

FString UHyperlinkBlueprintFunctionLibrary::CreateLinkFromPayload(const TSubclassOf<UHyperlinkDefinition> DefinitionClass,
                                                                  const FJsonObjectWrapper& InPayload)
{
	return FHyperlinkUtility::CreateLinkFromPayload(DefinitionClass, InPayload.JsonObject.ToSharedRef());
}

UObject* UHyperlinkBlueprintFunctionLibrary::LoadObject(const FString& PackageName)
{
	return FHyperlinkUtility::LoadObject(PackageName);
}

UObject* UHyperlinkBlueprintFunctionLibrary::OpenEditorForAsset(const FName& PackageName)
{
	return FHyperlinkUtility::OpenEditorForAsset(PackageName);
}
