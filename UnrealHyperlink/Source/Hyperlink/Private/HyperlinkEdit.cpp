// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkEdit.h"

#include "AssetViewUtils.h"
#include "HyperlinkUtils.h"

FString UHyperlinkEdit::GetDefinitionName() const
{
	return TEXT("edit");
}

void UHyperlinkEdit::Initialize()
{
	// TODO
}

void UHyperlinkEdit::Deinitialize()
{
	// TODO
}

void UHyperlinkEdit::ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments)
{
	if (UObject* const Object{ FHyperlinkUtils::LoadObjectFromPackageName(LinkArguments[0]) })
	{
		AssetViewUtils::OpenEditorForAsset(Object);
	}
}
