#include "HyperlinkSettingsCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "HyperlinkClassEntryCustomization.h"
#include "HyperlinkSettings.h"

TSharedRef<IDetailCustomization> FHyperlinkSettingsCustomization::MakeInstance()
{
	return MakeShared<FHyperlinkSettingsCustomization>();
}

void FHyperlinkSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& DefinitionsCategory = DetailBuilder.EditCategory(TEXT("RegisteredDefinitions"));
	TSharedRef<IPropertyHandle> Prop = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UHyperlinkSettings, RegisteredDefinitions));
	ArrayBuilder = FHyperlinkClassEntryArrayBuilder::MakeInstance(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UHyperlinkSettings, RegisteredDefinitions)));
	DefinitionsCategory.AddCustomBuilder(ArrayBuilder.ToSharedRef());
	
	IDetailCategoryBuilder& GeneralCategory = DetailBuilder.EditCategory(TEXT("General"));
	GeneralCategory.SetSortOrder(0);
}
