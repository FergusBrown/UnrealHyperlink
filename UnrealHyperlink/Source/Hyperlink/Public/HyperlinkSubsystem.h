// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "HyperlinkSubsystem.generated.h"

class UHyperlinkDefinition;

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
	/* Decode the provided unreal://... link and execute the associated action */
	void ExecuteLink(const FString& Link);
#endif //WITH_EDITOR

	void RefreshDefinitions();

	/**
	 * @tparam T the class of the desired definition
	 * @return the requested definition, nullptr if not registered
	 */
	template<typename T>
	const T* GetDefinition() const
	{
		const T* Ret{ nullptr };
		for (const TPair<FString, TObjectPtr<UHyperlinkDefinition>>& Pair : Definitions)
		{
			if (Pair.Value->IsA<T>())
			{
				Ret = Pair.Value;
				break;
			}
		}
		return Ret;
	}
	
private:
	void InitDefinitions();
	void DeinitDefinitions();
	
	void CopyLinkConsole(const TArray<FString>& Args);
#if WITH_EDITOR
	void ExecuteLinkConsole(const TArray<FString>& Args);
	void ExecuteLinkDeferred(FString Link) const;
#endif //WITH_EDITOR

private:
	UPROPERTY()
	TMap<FString, TObjectPtr<UHyperlinkDefinition>> Definitions{};

	IConsoleObject* CopyConsoleCommand{ nullptr };
#if WITH_EDITOR
	FDelegateHandle PostEditorTickHandle{};
	IConsoleObject* ExecuteConsoleCommand{ nullptr };
#endif //WITH_EDITOR
};
