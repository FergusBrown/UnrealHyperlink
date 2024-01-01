// Copyright (c) 2023-2024 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkLevelActor.generated.h"

class FHyperlinkLevelActorCommands : public TCommands<FHyperlinkLevelActorCommands>
{
public:
	FHyperlinkLevelActorCommands();
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> CopyLevelActorLink{ nullptr };
};

USTRUCT()
struct FHyperlinkLevelActorPayload
{
	GENERATED_BODY()
	
	UPROPERTY()
	FName LevelPackageName{};

	UPROPERTY()
	FName ActorName{};
};

/**
 * Hyperlink for opening a level and focusing on an actor in the level
 */
UCLASS()
class HYPERLINKEDITOR_API UHyperlinkLevelActor : public UHyperlinkDefinition
{
	GENERATED_BODY()
	
public:
	virtual void Initialize() override;
	virtual void Deinitialize() override;
	
	virtual TSharedPtr<FJsonObject> GeneratePayload(const TArray<FString>& Args) const override;
	
	virtual void ExecutePayload(const TSharedRef<FJsonObject>& InPayload) override;
	
private:
	TSharedPtr<FUICommandList> LevelActorCommands{};
};
