// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "Definitions/HyperlinkDefinitionBlueprintBase.h"

#include "JsonObjectWrapper.h"
#include "Serialization/JsonSerializer.h"

void UHyperlinkDefinitionBlueprintBase::Initialize()
{
	InitializeImpl();
}

void UHyperlinkDefinitionBlueprintBase::Deinitialize()
{
	DeinitializeImpl();
}

TSharedPtr<FJsonObject> UHyperlinkDefinitionBlueprintBase::GeneratePayload(const TArray<FString>& Args) const
{
	FJsonObjectWrapper JsonObjectWrapper{};
	if (GetClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UHyperlinkDefinitionBlueprintBase,
			GeneratePayloadImpl), EIncludeSuperFlag::ExcludeSuper))
	{
		JsonObjectWrapper = GeneratePayloadImpl();
	}
	else if (GetClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UHyperlinkDefinitionBlueprintBase,
		GeneratePayloadStringImpl), EIncludeSuperFlag::ExcludeSuper))
	{
		JsonObjectWrapper.JsonObjectFromString(GeneratePayloadStringImpl());
	}
	 
	return JsonObjectWrapper.JsonObject;
}

void UHyperlinkDefinitionBlueprintBase::ExecutePayload(const TSharedRef<FJsonObject>& InPayload)
{
	FJsonObjectWrapper JsonObjectWrapper{};
	JsonObjectWrapper.JsonObject = InPayload.ToSharedPtr();
	if (GetClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UHyperlinkDefinitionBlueprintBase,
		ExecutePayloadImpl), EIncludeSuperFlag::ExcludeSuper))
	{
		ExecutePayloadImpl(JsonObjectWrapper);
	}
	else if (GetClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UHyperlinkDefinitionBlueprintBase,
		ExecutePayloadStringImpl), EIncludeSuperFlag::ExcludeSuper))
	{
		FString JsonObjectString{};
		if (JsonObjectWrapper.JsonObjectToString(JsonObjectString))
		{
			ExecutePayloadStringImpl(JsonObjectString);
		}
	}
}
