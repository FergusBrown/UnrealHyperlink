﻿// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkUtils.h"

UObject* FHyperlinkUtils::LoadObjectFromPackageName(const FString& PackageName)
{
	UObject* Ret{ nullptr };
	
	if (UPackage* const Package{ LoadPackage(nullptr, *PackageName, LOAD_NoRedirects) })
	{
		Package->FullyLoad();

		const FString AssetName{ FPaths::GetBaseFilename(PackageName) };
		Ret = FindObject<UObject>(Package, *AssetName);
	}

	return Ret;
}