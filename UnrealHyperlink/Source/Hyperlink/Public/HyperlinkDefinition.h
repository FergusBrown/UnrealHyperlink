﻿// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HyperlinkDefinition.generated.h"

/**
 * Abstract class for defining hyperlink types
 */
UCLASS(Abstract) // TODO: consider making blueprintable. All the virtuals will need to be made into blueprint native events
class HYPERLINK_API UHyperlinkDefinition : public UObject
{
	GENERATED_BODY()
	
public:
	virtual FString GetDefinitionName() const PURE_VIRTUAL(UHyperlinkDefinition::GetDefinitionName, return FString(););
	
	virtual void ExecuteLinkBody(const FString& LinkBody) PURE_VIRTUAL(UHyperlinkDefinition::ExecuteLinkBody, );

	/* Setup any commands, menu extensions etc. which can be used to generate this link */
	virtual void Initialize() PURE_VIRTUAL(UHyperlinkDefinition::Initialize, );

	/* Tear down anything setup in Initialize*/
	virtual void Deinitialize() {}
	
	/* Used to validate a received link */
	virtual FString GetBodyPattern() const;

	/* Construct the base of the link without the body */
	FString GetHyperlinkBase() const;
	
};
