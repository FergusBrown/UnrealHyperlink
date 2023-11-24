// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "HyperlinkSubsystem.generated.h"

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
	bool ExecuteLink(const FString& Link) const;

	static void ExecuteBrowse(const FString& LinkBody);
	static void ExecuteEdit(const FString& LinkBody);

	// Utility
	static FString GetLinkBase();
	static FString GetLinkFormatHint();

private:
	void ExecuteLinkConsole(const TArray<FString>& Args) const;
	
private:
	// TODO: move this to editor module
	IConsoleObject* ExecuteConsoleCommand{ nullptr };
};
