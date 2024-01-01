// Copyright (c) 2023-2024 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "HyperlinkSubsystem.generated.h"

class UHyperlinkDefinition;
struct FHyperlinkExecutePayload;

/**
 * 
 */
UCLASS()
class HYPERLINK_API UHyperlinkSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
#if WITH_EDITOR
	/* Static so that we can easily call this function from remote control API */
	UFUNCTION(BlueprintCallable, Category = "Hyperlink")
	static void StaticExecuteLink(const FHyperlinkExecutePayload& ExecutePayload);
	
	void ExecuteLink(const FHyperlinkExecutePayload& ExecutePayload);
	void ExecuteLink(const FString& InString);
#endif //WITH_EDITOR

	void RefreshDefinitions();

	/**
	 * @tparam T the class of the desired definition
	 * @return the requested definition, nullptr if not registered
	 */
	template<typename T>
	T* GetDefinition() const
	{
		return GetDefinition(T::StaticClass());
	}

	UHyperlinkDefinition* GetDefinition(const TSubclassOf<UHyperlinkDefinition> DefinitionClass) const;
	
private:
	void InitDefinitions();
	void DeinitDefinitions();
	
	void HelpConsole(const TArray<FString>& Args);
	void CopyLinkConsole(const TArray<FString>& Args);
#if WITH_EDITOR
	void ExecuteLinkConsole(const TArray<FString>& Args);
	void ExecuteLinkDeferred(FHyperlinkExecutePayload ExecutePayload) const;
	
	// TODO: move this to utility?
	/**
	 * @brief Attempts to extract a JSON substring from the provided string and deserialize it
	 * @param InString InString String which contains a JSON substring
	 * @param OutPayload Deserialized payload object
	 * @return true if operation was successful
	 */
	static bool TryGetPayloadFromString(const FString& InString, FHyperlinkExecutePayload& OutPayload);
#endif //WITH_EDITOR

private:
	UPROPERTY()
	TMap<FString, TObjectPtr<UHyperlinkDefinition>> Definitions{};

	TArray<IConsoleObject*> ConsoleCommands{ nullptr };
#if WITH_EDITOR
	FDelegateHandle PostEditorTickHandle{};
#endif //WITH_EDITOR
};
