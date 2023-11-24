// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "Definitions/HyperlinkDefinitionBlueprintBase.h"

#include "JsonObjectWrapper.h"

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
	return GeneratePayloadImpl().JsonObject;
}

void UHyperlinkDefinitionBlueprintBase::ExecutePayload(const TSharedRef<FJsonObject>& InPayload)
{
	FJsonObjectWrapper JsonWrapper{};
	JsonWrapper.JsonObject = InPayload;
	ExecutePayloadImpl(JsonWrapper);
}
