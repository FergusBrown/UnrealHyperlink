// Copyright (c) 2023-2024 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "HyperlinkDefinition.h"
#include "HyperlinkEdit.generated.h"

#if WITH_EDITOR
class FHyperlinkEditCommands : public TCommands<FHyperlinkEditCommands>
{
public:
	FHyperlinkEditCommands();
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> CopyContentBrowserLink{ nullptr };
	TSharedPtr<FUICommandInfo> CopyAssetEditorLink{ nullptr };
};
#endif //WITH_EDITOR


/**
 * Hyperlink for opening the editor for an asset
 */
UCLASS()
class HYPERLINK_API UHyperlinkEdit : public UHyperlinkDefinition
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual void Initialize() override;
	virtual void Deinitialize() override;
#endif //WITH_EDITOR
	
	virtual TSharedPtr<FJsonObject> GeneratePayload(const TArray<FString>& Args) const override;
	static TSharedPtr<FJsonObject> GeneratePayloadFromPackageName(const FName& PackageName);
#if WITH_EDITOR
	static TSharedPtr<FJsonObject> GeneratePayloadFromContentBrowser();
	TSharedPtr<FJsonObject> GeneratePayloadFromAssetEditor() const;
	
	virtual void ExecutePayload(const TSharedRef<FJsonObject>& InPayload) override;
private:
	TSharedRef<FExtender> OnExtendAssetEditor(const TSharedRef<FUICommandList> CommandList,
	                                          const TArray<UObject*> ContextSensitiveObjects);
	
private:
	FDelegateHandle KeyboardShortcutHandle{};
	FDelegateHandle AssetEditorExtensionHandle{};
	TSharedPtr<FUICommandList> EditCommands{};

	FName AssetEditorPackageName{};
#endif //WITH_EDITOR
};
