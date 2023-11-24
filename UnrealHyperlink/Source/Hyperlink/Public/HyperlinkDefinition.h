// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HyperlinkDefinition.generated.h"

/**
 * Abstract class for defining hyperlink types
 */
UCLASS(Abstract, Config = Hyperlink)
class HYPERLINK_API UHyperlinkDefinition : public UObject
{
	GENERATED_BODY()

public:
	/* Setup any commands, menu extensions etc. which can be used to generate this link */
	virtual void Initialize() {}

	/* Tear down anything setup in Initialize */
	virtual void Deinitialize() {}

	/* Generate payload using current editor/game state */
	virtual TSharedPtr<FJsonObject> GeneratePayload() const { return TSharedPtr<FJsonObject>(); }

#if WITH_EDITOR
	virtual void ExecutePayload(const TSharedRef<FJsonObject>& InPayload) PURE_VIRTUAL(UHyperlinkDefinition::ExecutePayload, );
#endif //WITH_EDITOR

	/* Generate a link using the GeneratePayload function and copy it to clipboard */
	void CopyLink() const;

	/* Generate a link using the GeneratePayload function and log it */
	void PrintLink() const;
	
protected:
	static void CopyLink(const FString& InLink);
	
};
