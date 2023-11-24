// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkLevelGoTo.generated.h"

#if WITH_EDITOR
class FHyperlinkGoToCommands : public TCommands<FHyperlinkGoToCommands>
{
public:
	FHyperlinkGoToCommands();
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> CopyGoToLink{ nullptr };
};
#endif //WITH_EDITOR


/**
 * Link type for teleporting viewport (Editor) or player controller (PIE/Game) to a given location + rotation
 */
UCLASS()
class HYPERLINK_API UHyperlinkLevelGoTo : public UHyperlinkDefinition
{
	GENERATED_BODY()
	
public:
	UHyperlinkLevelGoTo();
	
	virtual void Initialize() override;
	virtual void Deinitialize() override;

	/* Generate link using the active level editor viewport (editor) or player controller (game). Fails if viewport not found */
	virtual bool GenerateLink(FString& OutLink) const override;

	/* Generate link using the provided parameters*/
	FString GenerateLink(const FString& InLevelPackageName, const FVector& InLocation, const FRotator& InRotation) const;

#if WITH_EDITOR
protected:
	virtual void ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments) override;
#endif //WITH_EDITOR

private:
	static bool GetGameWorldCameraInfo(const UWorld* const World, FVector& OutLocation, FRotator& OutRotation);
	
private:
	IConsoleObject* CopyConsoleCommand{ nullptr };
#if WITH_EDITOR
	TSharedPtr<FUICommandList> GoToCommands{};
#endif //WITH_EDITOR
};