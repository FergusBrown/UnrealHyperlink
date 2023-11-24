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
	
	// Utility
	static FString GetLinkBase();
	static FString GetLinkFormatHint();

#if WITH_EDITOR
	/* Decode the provided unreal://... link and execute the associated action */
	void ExecuteLink(const FString& Link);
	void RefreshDefinitions();
private:
	void InitDefinitions();
	void DeinitDefinitions();
	void ExecuteLinkConsole(const TArray<FString>& Args);
	void ExecuteLinkDeferred(FString Link) const;
#endif //WITH_EDITOR

private:
	UPROPERTY()
	TMap<FString, TObjectPtr<UHyperlinkDefinition>> Definitions{};

#if WITH_EDITOR
	FDelegateHandle PostEditorTickHandle{};
	IConsoleObject* ExecuteConsoleCommand{ nullptr };
#endif //WITH_EDITOR
};
