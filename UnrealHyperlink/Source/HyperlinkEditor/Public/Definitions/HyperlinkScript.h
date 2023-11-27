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
 * python is TODO!
 */
UCLASS()
class HYPERLINKEDITOR_API UHyperlinkScript : public UHyperlinkDefinition
{
	GENERATED_BODY()

public:
	virtual void Initialize() override;
	virtual void Deinitialize() override;
	virtual TSharedPtr<FJsonObject> GeneratePayload() const override;
	virtual void ExecutePayload(const TSharedRef<FJsonObject>& InPayload) override;

private:
	static bool IsBlutilitySelected();
	static bool UserConfirmedScriptExecution(const FString& ScriptName);
	
private:
	TSharedPtr<FUICommandList> ScriptCommands{};
};
