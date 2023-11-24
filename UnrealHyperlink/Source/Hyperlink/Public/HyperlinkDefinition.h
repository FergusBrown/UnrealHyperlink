// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HyperlinkDefinition.generated.h"

class UHyperlinkDefinitionSettings;

/**
 * Abstract class for defining hyperlink types
 */
UCLASS(Abstract, Config = Hyperlink, DefaultConfig) // TODO: consider making blueprintable. All the virtuals will need to be made into blueprint native events
class HYPERLINK_API UHyperlinkDefinition : public UObject
{
	GENERATED_BODY()
	
public:
	FString GetDefinitionIdentifier() const;
	TSubclassOf<UHyperlinkDefinitionSettings> GetSettingsClass() const;
	
	void ExecuteLinkBody(const FString& InLinkBody);
	
	/* Setup any commands, menu extensions etc. which can be used to generate this link */
	virtual void Initialize() {}

	/* Tear down anything setup in Initialize*/
	virtual void Deinitialize() {}

	/* Construct the base of the link without the body */
	FString GetHyperlinkBase() const;

	/* Generate a link using current editor/game state */
	virtual bool GenerateLink(FString& OutLink) const { return false; }

	/* Generate a link using the GenerateLink function and copy it to clipboard */
	void CopyLink() const;

	/* Generate a link using the GenerateLink function and log it */
	void PrintLink() const;

protected:
	static void CopyLink(const FString& InLink);

#if WITH_EDITOR
	virtual void ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments) PURE_VIRTUAL(UHyperlinkDefinition::ExecuteLinkBodyInternal, );
#endif //WITH_EDITOR

private:
	void PrintLinkInternal(bool bCopy = false) const;
	
protected:
	/* The name used to identify this type of link */
	UPROPERTY(Config, EditAnywhere)
	FString DefinitionIdentifier{ TEXT("") };

	/* Used to validate a received link */
	FString BodyPattern{ TEXT(".*") };

	/* The name used to identify this type of link */
	TSubclassOf<UHyperlinkDefinitionSettings> SettingsClass{ nullptr };
};
