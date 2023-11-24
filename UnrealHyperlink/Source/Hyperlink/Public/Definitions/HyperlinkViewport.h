// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkViewport.generated.h"

#if WITH_EDITOR
class FHyperlinkViewportCommands : public TCommands<FHyperlinkViewportCommands>
{
public:
	FHyperlinkViewportCommands();
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> CopyViewportLink{ nullptr };
};
#endif //WITH_EDITOR


/**
 * Link type for teleporting viewport to a given location + rotation. Can be generated in-game
 */
UCLASS()
class HYPERLINK_API UHyperlinkViewport : public UHyperlinkDefinition
{
	GENERATED_BODY()
	
public:
	UHyperlinkViewport();
	
	virtual void Initialize() override;
	virtual void Deinitialize() override;

	/* Generate link using the active level editor viewport (editor) or player controller (game). Fails if viewport not found */
	virtual bool GenerateLink(FString& OutLink) const override;

	/* Generate link using the provided parameters */
	FString GenerateLink(const FString& InLevelPackageName, const FVector& InLocation, const FRotator& InRotation) const;

#if WITH_EDITOR
protected:
	virtual void ExecuteExtractedArgs(const TArray<FString>& LinkArguments) override;
#endif //WITH_EDITOR

private:
	static bool GetGameWorldCameraInfo(const UWorld* World, FVector& OutLocation, FRotator& OutRotation);

#if WITH_EDITOR
private:
	TSharedPtr<FUICommandList> ViewportCommands{};
#endif //WITH_EDITOR
};
