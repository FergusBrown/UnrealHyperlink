// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "PropertyCustomizationHelpers.h"

class IPropertyHandle;

class FHyperlinkClassEntryArrayBuilder
	: public FDetailArrayBuilder
	, public TSharedFromThis<FHyperlinkClassEntryArrayBuilder> // Required to allow shared pointers of this class
{
public:
	FHyperlinkClassEntryArrayBuilder(const TSharedRef<IPropertyHandle>& InBaseProperty);
	
	static TSharedRef<FHyperlinkClassEntryArrayBuilder> MakeInstance(const TSharedRef<IPropertyHandle>& InPropertyHandle);
	
	// FDetailArrayBuilder Interface
	virtual void GenerateHeaderRowContent(FDetailWidgetRow& NodeRow) override;

private:
	/* Generates the UI for each array entry */
	void OnGenerateEntry(TSharedRef<IPropertyHandle> ElementProperty, int32 ElementIndex, IDetailChildrenBuilder& ChildrenBuilder);
	
	static TSharedRef<SWidget> CreateNameWidget(const TSharedPtr<IPropertyHandle>& EnablePropertyHandle, const TSharedPtr<IPropertyHandle>& ClassPropertyHandle, bool bAddColumnTitles);
	static TSharedRef<SWidget> CreateNameWidget(const TSharedPtr<IPropertyHandle>& EnablePropertyHandle, const TSharedPtr<IPropertyHandle>& ClassPropertyHandle);
	static TSharedRef<SWidget> CreateCustomValueWidget(const TSharedPtr<IPropertyHandle>& IdentifierPropertyHandle, bool bAddColumnTitles);

	static bool IsResetToDefaultVisible(TSharedPtr<IPropertyHandle> ElementPropertyHandle);
	static void ResetToDefaultHandler(TSharedPtr<IPropertyHandle> ElementPropertyHandle);
};
