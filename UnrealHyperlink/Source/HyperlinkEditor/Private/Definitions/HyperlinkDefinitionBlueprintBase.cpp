// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "Definitions/HyperlinkDefinitionBlueprintBase.h"

#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonReader.h"
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
	TSharedPtr<FJsonObject> Payload{};
	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(GeneratePayloadImpl());
	FJsonSerializer::Deserialize(JsonReader, Payload);
	return Payload;
}

void UHyperlinkDefinitionBlueprintBase::ExecutePayload(const TSharedRef<FJsonObject>& InPayload)
{
	FString Str;
	const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter
		{ TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Str, 0) };
	if (FJsonSerializer::Serialize(InPayload, JsonWriter, true))
	{
		ExecutePayloadImpl(Str);
	}
}
