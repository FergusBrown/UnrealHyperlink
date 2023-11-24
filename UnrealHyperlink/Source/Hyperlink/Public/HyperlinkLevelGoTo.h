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
 * Similar to BugIt console command // TODO: match BugIt features
 */
UCLASS()
class HYPERLINK_API UHyperlinkLevelGoTo : public UHyperlinkDefinition
{
	GENERATED_BODY()
	
public:
	UHyperlinkLevelGoTo();
	
	virtual void Initialize() override;
	virtual void Deinitialize() override;

	/* Generate link using the active viewport. Fails if viewport not found */
	bool GenerateLink(FString& OutLink) const;

	/* Generate link using the provided parameters*/
	FString GenerateLink(const FString& InLevelPackageName, const FVector& InLocation, const FRotator& InRotation) const;
protected:
	virtual FString GetBodyPattern() const override;
	virtual void ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments) override;

private:
	void CopyLink() const;
	// TODO: move to utilities
	// static bool GetLevelPackageName(const UWorld* const World, FString& OutLevelPackageName);
	static bool GetGameWorldCameraInfo(const UWorld* const World, FVector& OutLocation, FRotator& OutRotation);
	
private:
	IConsoleObject* CopyConsoleCommand{ nullptr };
#if WITH_EDITOR
	TSharedPtr<FUICommandList> GoToCommands{};
#endif //WITH_EDITOR
};
