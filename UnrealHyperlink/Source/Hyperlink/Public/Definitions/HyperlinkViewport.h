// Copyright (c) 2023-2024 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

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

USTRUCT()
struct FHyperlinkViewportPayload
{
	GENERATED_BODY()

	UPROPERTY()
	FName LevelPackageName{};

	UPROPERTY()
	FVector Location{ FVector::ZeroVector };

	UPROPERTY()
	FRotator Rotation{ FRotator::ZeroRotator };
};

/**
 * Link type for teleporting viewport to a given location + rotation. Can be generated in-game
 */
UCLASS()
class HYPERLINK_API UHyperlinkViewport : public UHyperlinkDefinition
{
	GENERATED_BODY()
	
public:
	virtual void Initialize() override;
	virtual void Deinitialize() override;

	/* Generate payload using the active level editor viewport (editor) or player controller (game). Fails if viewport not found */
	virtual TSharedPtr<FJsonObject> GeneratePayload(const TArray<FString>& Args) const override;

	/* Generate payload using the provided parameters */
	TSharedPtr<FJsonObject> GeneratePayload(const FName& InLevelPackageName, const FVector& InLocation, const FRotator& InRotation) const;

#if WITH_EDITOR
	virtual void ExecutePayload(const TSharedRef<FJsonObject>& InPayload) override;
#endif //WITH_EDITOR

private:
	static bool GetGameWorldCameraInfo(const UWorld* World, FVector& OutLocation, FRotator& OutRotation);

#if WITH_EDITOR
private:
	TSharedPtr<FUICommandList> ViewportCommands{};
#endif //WITH_EDITOR
};
