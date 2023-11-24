// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HyperlinkDefinition.generated.h"

/**
 * Abstract class for defining hyperlink types
 */
UCLASS(Abstract) // TODO: consider making blueprintable
class HYPERLINK_API UHyperlinkDefinition : public UObject
{
	GENERATED_BODY()

protected:
	/* */
	virtual FName GetDefinitionName() const PURE_VIRTUAL(UHyperlinkDefinition::GetDefinitionName, return FName(););

	/* Used to validate a received link */
	virtual FName GetBodyPattern() const;

	/* Setup any commands, menu extensions etc. which can be used to generate this link */
	virtual void Initialize() PURE_VIRTUAL(UHyperlinkDefinition::Initialize, );

	/* Tear down anything setup in Initialize*/
	virtual void Deinitialize() {}

	virtual FName GetHyperlinkBase() const;
};
