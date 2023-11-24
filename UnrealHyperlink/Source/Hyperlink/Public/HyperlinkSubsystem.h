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
	void RefreshDefinitions();
#if WITH_EDITOR
	/* Decode the provided unreal://... link and execute the associated action */
	void ExecuteLink(const FString& Link);
#endif //WITH_EDITOR
	
private:
	void CopyLinkConsole(const TArray<FString>& Args);
	void InitDefinitions();
	void DeinitDefinitions();
#if WITH_EDITOR
	void ExecuteLinkConsole(const TArray<FString>& Args);
	void ExecuteLinkDeferred(const FString Link) const;
#endif //WITH_EDITOR

private:
	UPROPERTY()
	TMap<FString, TObjectPtr<UHyperlinkDefinition>> Definitions{};

	IConsoleObject* CopyConsoleCommand{ nullptr };
#if WITH_EDITOR
	FDelegateHandle PostEditorTickHandle{};
	IConsoleObject* ExecuteConsoleCommand{ nullptr };
#endif //WITH_EDITOR
};
