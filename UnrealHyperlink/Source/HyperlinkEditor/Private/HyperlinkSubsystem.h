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
class UHyperlinkSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
private:
	TUniquePtr<FHyperlinkPipeServer> PipeServer{ nullptr };
};
