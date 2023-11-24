// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "HyperlinkSubsystem.generated.h"

class FHyperlinkPipeServer;
/**
 * 
 */
UCLASS()
class HYPERLINKEDITOR_API UHyperlinkSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	/* Decode the provided unreal://... link and execute the associated action */
	bool ExecuteLink(const FString& Link) const;
	
	// Link functions
	using FHyperlinkExecutor = const TFunction<void(const FString& LinkBody)>;
	void RegisterHyperlinkExecutor(const FName& ExecutorID, FHyperlinkExecutor Executor);

	static void ExecuteBrowse(const FString& LinkBody);
	static void ExecuteEdit(const FString& LinkBody);

	// Utility
	static FString GetLinkBase();
	static FString GetLinkFormatHint();

private:
	void ExecuteLinkConsole(const TArray<FString>& Args) const;
	
private:
	TUniquePtr<FHyperlinkPipeServer> PipeServer{ nullptr };

	TMap<FName, FHyperlinkExecutor> LinkExecutorMap{};

	IConsoleObject* ExecuteConsoleCommand{ nullptr };
};
