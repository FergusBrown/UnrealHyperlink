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

TSharedPtr<FJsonObject> UHyperlinkDefinitionBlueprintBase::GeneratePayload() const
{
	FJsonObjectWrapper JsonObjectWrapper{ GeneratePayloadImpl() };
	return JsonObjectWrapper.JsonObject;
}

void UHyperlinkDefinitionBlueprintBase::ExecutePayload(const TSharedRef<FJsonObject>& InPayload)
{
	FJsonObjectWrapper JsonObjectWrapper{};
	JsonObjectWrapper.JsonObject = InPayload.ToSharedPtr();
	ExecutePayloadImpl(JsonObjectWrapper);
}
