#include "HyperlinkClassEntryCustomization.h"

#include "HyperlinkClassEntry.h"
#include "HyperlinkUtility.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyHandle.h"
#include "PropertyCustomizationHelpers.h"

FHyperlinkClassEntryArrayBuilder::FHyperlinkClassEntryArrayBuilder(const TSharedRef<IPropertyHandle>& InBaseProperty)
	: FDetailArrayBuilder(InBaseProperty, false, false, false)
{
}

/*static*/TSharedRef<FHyperlinkClassEntryArrayBuilder> FHyperlinkClassEntryArrayBuilder::MakeInstance(const TSharedRef<IPropertyHandle>& InPropertyHandle)
{
	TSharedRef<FHyperlinkClassEntryArrayBuilder> Builder{ MakeShared<FHyperlinkClassEntryArrayBuilder>(InPropertyHandle) };

	Builder->OnGenerateArrayElementWidget(FOnGenerateArrayElementWidget::CreateSP(Builder, &FHyperlinkClassEntryArrayBuilder::OnGenerateEntry));
	
	return Builder;
}

void FHyperlinkClassEntryArrayBuilder::GenerateHeaderRowContent(FDetailWidgetRow& NodeRow)
{
	NodeRow.ShouldAutoExpand(true);
	FDetailArrayBuilder::GenerateHeaderRowContent(NodeRow);
}

// NOLINTNEXTLINE (performance-unnecessary-value-param) Delegate signature
void FHyperlinkClassEntryArrayBuilder::OnGenerateEntry(TSharedRef<IPropertyHandle> ElementProperty, const int32 ElementIndex, IDetailChildrenBuilder& ChildrenBuilder)
{
	IDetailPropertyRow& PropertyRow = ChildrenBuilder.AddProperty(ElementProperty);
	PropertyRow.ShowPropertyButtons(false); // Hide entry delete buttons
	
	const TSharedPtr<IPropertyHandle> EnablePropertyHandle{ ElementProperty->GetChildHandle(0) };
	const TSharedPtr<IPropertyHandle> ClassPropertyHandle{ ElementProperty->GetChildHandle(1) };
	const TSharedPtr<IPropertyHandle> IdentifierPropertyHandle{ ElementProperty->GetChildHandle(2) };

	const FResetToDefaultOverride ResetToDefaultOverride
	{
		FResetToDefaultOverride::Create(FIsResetToDefaultVisible::CreateStatic(&FHyperlinkClassEntryArrayBuilder::IsResetToDefaultVisible),
			FResetToDefaultHandler::CreateStatic(&FHyperlinkClassEntryArrayBuilder::ResetToDefaultHandler))
	};
	
	static constexpr float MinWidth{ 150.0f };
	const bool bAddColumnTitles{ ElementIndex == 0 };
	
	PropertyRow.CustomWidget()
		.NameContent()
		.MinDesiredWidth(MinWidth)
		[
			CreateNameWidget(EnablePropertyHandle, ClassPropertyHandle, bAddColumnTitles)
		]
		.ValueContent()
		.MinDesiredWidth(MinWidth)
		[
			CreateCustomValueWidget(IdentifierPropertyHandle, bAddColumnTitles)
		]
		.OverrideResetToDefault(ResetToDefaultOverride);
}

TSharedRef<SWidget> FHyperlinkClassEntryArrayBuilder::CreateNameWidget(const TSharedPtr<IPropertyHandle>& EnablePropertyHandle, const TSharedPtr<IPropertyHandle>& ClassPropertyHandle, const bool bAddColumnTitles)
{
	if (bAddColumnTitles)
	{
		return SNew(SBorder)
			.Padding(0.0f, 5.0f)
			.BorderImage(FCoreStyle::Get().GetBrush(TEXT("NoBorder")))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(30.0f, 0.0f, 0.0f, 5.0f)
				[
					SNew(STextBlock)
					.Text(ClassPropertyHandle->GetPropertyDisplayName())
					.ToolTipText(ClassPropertyHandle->GetToolTipText())
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 8.0f))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					CreateNameWidget(EnablePropertyHandle, ClassPropertyHandle)
				]
			];
	}
	else
	{
		return CreateNameWidget(EnablePropertyHandle, ClassPropertyHandle);
	}
}

TSharedRef<SWidget> FHyperlinkClassEntryArrayBuilder::CreateNameWidget(const TSharedPtr<IPropertyHandle>& EnablePropertyHandle, const TSharedPtr<IPropertyHandle>& ClassPropertyHandle)
{
	FText ClassName{};
	FText ClassToolTip{};
		
	UObject* PropValue{ nullptr };
	ClassPropertyHandle->GetValue(PropValue);
	if (const UClass* const Class{ Cast<UClass>(PropValue) })
	{
		ClassName = Class->GetDisplayNameText();
		ClassToolTip = Class->GetToolTipText();
	}
	
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(5.0f, 0.0)
		[
			EnablePropertyHandle->CreatePropertyValueWidget()
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(ClassName)
			.ToolTipText(ClassToolTip)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8.0f))
		];
}

TSharedRef<SWidget> FHyperlinkClassEntryArrayBuilder::CreateCustomValueWidget(const TSharedPtr<IPropertyHandle>& IdentifierPropertyHandle, const bool bAddColumnTitles)
{
	if (bAddColumnTitles)
	{
		return SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(8.0f, 0.0f, 0.0f, 5.0f)
			[
				SNew(STextBlock)
				.Text(IdentifierPropertyHandle->GetPropertyDisplayName())
				.ToolTipText(IdentifierPropertyHandle->GetToolTipText())
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 8.0f))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				IdentifierPropertyHandle->CreatePropertyValueWidget()
			];
	}
	else
	{
		return IdentifierPropertyHandle->CreatePropertyValueWidget();
	}
}

// NOLINTNEXTLINE (performance-unnecessary-value-param) Delegate signature
bool FHyperlinkClassEntryArrayBuilder::IsResetToDefaultVisible(TSharedPtr<IPropertyHandle> ElementPropertyHandle)
{
	const TSharedPtr<IPropertyHandle> ClassPropertyHandle{ ElementPropertyHandle->GetChildHandle(1) };
	const TSharedPtr<IPropertyHandle> IdentifierPropertyHandle{ ElementPropertyHandle->GetChildHandle(2) };
	
	UObject* ClassValue{ nullptr };
	ClassPropertyHandle->GetValue(ClassValue);
	
	bool bResult{ true };
	
	if (const UClass* const Class{ Cast<UClass>(ClassValue) })
	{
		FString IdentifierValue{};
		IdentifierPropertyHandle->GetValue(IdentifierValue);
		const FString DefaultIdentifier{ FHyperlinkUtility::CreateClassDisplayString(Class) };
		bResult = IdentifierValue != DefaultIdentifier;
	}
	
	return bResult;	
}

// NOLINTNEXTLINE (performance-unnecessary-value-param) Delegate signature
void FHyperlinkClassEntryArrayBuilder::ResetToDefaultHandler(TSharedPtr<IPropertyHandle> ElementPropertyHandle)
{
	const TSharedPtr<IPropertyHandle> ClassPropertyHandle{ ElementPropertyHandle->GetChildHandle(1) };
	const TSharedPtr<IPropertyHandle> IdentifierPropertyHandle{ ElementPropertyHandle->GetChildHandle(2) };
	
	UObject* ClassValue{ nullptr };
	ClassPropertyHandle->GetValue(ClassValue);
	
	if (const UClass* const Class{ Cast<UClass>(ClassValue) })
	{
		const FString DefaultIdentifier{ FHyperlinkUtility::CreateClassDisplayString(Class) };
		IdentifierPropertyHandle->SetValue(DefaultIdentifier);
	}
}