// Copyright (c) 2023-2024 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "IDetailCustomization.h"

class FHyperlinkClassEntryArrayBuilder;
class IDetailLayoutBuilder;
class IDetailsView;

class FHyperlinkSettingsCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	
	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
	// End of IDetailCustomization interface
	
private:
	TSharedPtr<FHyperlinkClassEntryArrayBuilder> ArrayBuilder{ nullptr };
};
