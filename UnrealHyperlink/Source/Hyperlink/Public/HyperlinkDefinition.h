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
	
	void ExecuteLinkBody(const FString& LinkBody);
	
	/* Setup any commands, menu extensions etc. which can be used to generate this link */
	virtual void Initialize() PURE_VIRTUAL(UHyperlinkDefinition::Initialize, );

	/* Tear down anything setup in Initialize*/
	virtual void Deinitialize() {}

	/* Construct the base of the link without the body */
	FString GetHyperlinkBase() const;

	/* Define your own GenerateLink function */
	/* FString GenerateLink(Args...); */

protected:
	/* Used to validate a received link TODO: consider moving to member variable */
	virtual FString GetBodyPattern() const;

	virtual void ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments) PURE_VIRTUAL(UHyperlinkDefinition::ExecuteLinkBodyInternal, );

protected:
	/* The name used to identify this type of link */
	UPROPERTY(Config, EditAnywhere)
	FString DefinitionIdentifier{ TEXT("") };

	/* The name used to identify this type of link */
	TSubclassOf<UHyperlinkDefinitionSettings> SettingsClass{ nullptr };
};
