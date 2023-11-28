// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkScript.generated.h"

class FHyperlinkScriptCommands : public TCommands<FHyperlinkScriptCommands>
{
public:
	FHyperlinkScriptCommands();
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> CopyBlutilityLink{ nullptr };
};

/**
 * Hyperlink for executing an editor utility blueprint or python script
 */
UCLASS()
class HYPERLINKEDITOR_API UHyperlinkScript : public UHyperlinkDefinition
{
	GENERATED_BODY()

public:
	virtual void Initialize() override;
	virtual void Deinitialize() override;
	virtual TSharedPtr<FJsonObject> GeneratePayload(const TArray<FString>& Args) const override;
	virtual void ExecutePayload(const TSharedRef<FJsonObject>& InPayload) override;

	/* Generate payload from the provided string, creating a relative path if possible */
	static TSharedPtr<FJsonObject> GenerateScriptPayload(FString ScriptPath);
	
	/* Generate payload from selected blutility in content browser */
	static TSharedPtr<FJsonObject> GeneratePayloadFromSelectedBlutility();
private:
	static bool IsBlutilitySelected();
	static bool UserConfirmedScriptExecution(const FString& ScriptName);
	
private:
	TSharedPtr<FUICommandList> ScriptCommands{};
};
