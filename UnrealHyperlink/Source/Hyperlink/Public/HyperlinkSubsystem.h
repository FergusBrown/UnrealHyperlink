// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "HyperlinkSubsystem.generated.h"

class UHyperlinkDefinition;
/**
 * 
 */
UCLASS()
class HYPERLINK_API UHyperlinkSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	/* Decode the provided unreal://... link and execute the associated action */
	void ExecuteLink(const FString& Link) const;

	static void ExecuteBrowse(const FString& LinkBody);

	// Utility
	static FString GetLinkBase();
	static FString GetLinkFormatHint();

private:
#if WITH_EDITOR
	void ExecuteLinkConsole(const TArray<FString>& Args) const;
#endif //WITH_EDITOR

private:
	UPROPERTY()
	TMap<FString, TObjectPtr<UHyperlinkDefinition>> Definitions{};

#if WITH_EDITORONLY_DATA
	IConsoleObject* ExecuteConsoleCommand{ nullptr };
#endif //WITH_EDITORONLY_DATA
};
