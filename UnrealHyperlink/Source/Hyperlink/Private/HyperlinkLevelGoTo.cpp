// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "HyperlinkLevelGoTo.h"

#include "GameFramework/PlayerController.h"
#include "HyperlinkFormat.h"
#include "HyperlinkUtils.h"
#include "LevelEditor.h"
#include "Log.h"
#if WITH_EDITOR
#include "Subsystems/UnrealEditorSubsystem.h"

#define LOCTEXT_NAMESPACE "HyperlinkGoTo"

FHyperlinkGoToCommands::FHyperlinkGoToCommands()
	: TCommands<FHyperlinkGoToCommands>(
		TEXT("HyperlinkGoTo"),
		NSLOCTEXT("Contexts", "HyperlinkGoTo", "Hyperlink GoTo"),
		NAME_None,
		FAppStyle::GetAppStyleSetName())
{
}

void FHyperlinkGoToCommands::RegisterCommands()
{
	UI_COMMAND(CopyGoToLink, "Copy GoTo Link", "Copy a link to go to the specified location in a level", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Alt | EModifierKey::Shift, EKeys::C));
}

#undef LOCTEXT_NAMESPACE

#endif //WITH_EDITOR

UHyperlinkLevelGoTo::UHyperlinkLevelGoTo()
{
	DefinitionIdentifier = TEXT("LevelGoTo");
	BodyPattern = TEXT(R"((.+):([0-9A-F]{96}))");
}

void UHyperlinkLevelGoTo::Initialize()
{
#if WITH_EDITOR
	// Check for GEditor in case launching uncooked game
	if (GEditor)
	{
		FHyperlinkGoToCommands::Register();
		GoToCommands = MakeShared<FUICommandList>();
		GoToCommands->MapAction(
			FHyperlinkGoToCommands::Get().CopyGoToLink,
			FExecuteAction::CreateUObject(this, &UHyperlinkDefinition::CopyLink));

		const FLevelEditorModule& LevelEditor{ FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor")) };
		LevelEditor.GetGlobalLevelEditorActions()->Append(GoToCommands.ToSharedRef());
	}

#endif //WITH_EDITOR
}

void UHyperlinkLevelGoTo::Deinitialize()
{
#if WITH_EDITOR
	const FLevelEditorModule& LevelEditor{ FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor")) };
	// Note that we don't need to unmap the actions from Level Editor Module's GlobalLevelEditorActions
	FHyperlinkGoToCommands::Unregister();
#endif //WITH_EDITOR
}

bool UHyperlinkLevelGoTo::GenerateLink(FString& OutLink) const
{
	FString LevelPackageName{};
	FVector Location{};
	FRotator Rotation{};
	bool bCameraInfoFound{ false };

// TODO: move getting level and getting rotation/location in to separate functions in utilities
#if WITH_EDITOR
	// Use active level editor viewport if we're in the editor
	if (GEditor)
	{
		if (const FWorldContext* const PieWorldContext{ GEditor->GetPIEWorldContext() })
		{
			if (const UWorld* const PieWorld{ PieWorldContext->World() })
			{
				LevelPackageName = PieWorld->PersistentLevel->GetPackage()->GetName();
				bCameraInfoFound |= GetGameWorldCameraInfo(PieWorldContext->World(), Location, Rotation);
			}
		}
		else
		{
			UUnrealEditorSubsystem* const UnrealEditorSubsystem{ GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>() };
			LevelPackageName = UnrealEditorSubsystem->GetEditorWorld()->PersistentLevel->GetPackage()->GetName();
			bCameraInfoFound = UnrealEditorSubsystem->GetLevelViewportCameraInfo(Location, Rotation);
		}
	}
	else
#endif //WITH_EDITOR
	if (GetWorld())
	{
		LevelPackageName = GetWorld()->PersistentLevel->GetPackage()->GetName();
		bCameraInfoFound = GetGameWorldCameraInfo(GetWorld(), Location, Rotation);
	}
	
	if (bCameraInfoFound)
	{
		OutLink = GenerateLink(LevelPackageName, Location, Rotation);
	}

	UE_CLOG(!bCameraInfoFound, LogHyperlink, Display, TEXT("Failed to generate LevelGoTo link: could not find viewport camera info."));

	return bCameraInfoFound;
}

FString UHyperlinkLevelGoTo::GenerateLink(const FString& InLevelPackageName, const FVector& InLocation, const FRotator& InRotation) const
{
	return GetHyperlinkBase() / InLevelPackageName + FHyperlinkFormat::ArgSeparator + FHyperlinkUtils::VectorToHexString(InLocation) + FHyperlinkUtils::VectorToHexString(InRotation.Vector());
}

// bool UHyperlinkLevelGoTo::GetLevelPackageName(const UWorld* const World, FString& OutLevelPackageName)
// {
// 	const bool bValidWorld{ World && World->PersistentLevel };
// 	
// 	if (bValidWorld)
// 	{
// 		OutLevelPackageName = World->PersistentLevel->GetPackage()->GetName();
// 	}
// 	
// 	return bValidWorld;
// }

/*static*/bool UHyperlinkLevelGoTo::GetGameWorldCameraInfo(const UWorld* const World, FVector& OutLocation, FRotator& OutRotation)
{
	bool bSuccess{ false };
	
	if (World && World->IsGameWorld())
	{
		if (const APlayerController* const PlayerController{ World->GetFirstPlayerController() })
		{
			PlayerController->GetPlayerViewPoint(OutLocation, OutRotation);
			bSuccess = true;
		}
	}
	
	return bSuccess;
}

#if WITH_EDITOR
void UHyperlinkLevelGoTo::ExecuteLinkBodyInternal(const TArray<FString>& LinkArguments)
{
	// Open level
	if(FHyperlinkUtils::OpenEditorForAsset(LinkArguments[1]))
	{
		// Set viewport position
		const FString& VectorStrings{ LinkArguments[2] };
		// TODO: Remove magic numbers
		const FString LocationString{ VectorStrings.Mid(0, 48) };
		const FString RotationString{ VectorStrings.Mid(48, 48) };

		const FVector Location{ FHyperlinkUtils::HexStringToVector(LocationString) };
		const FRotator Rotation{ FHyperlinkUtils::HexStringToVector(RotationString).Rotation() };
		
		UUnrealEditorSubsystem* const UnrealEditorSubsystem{ GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>() };
		UnrealEditorSubsystem->SetLevelViewportCameraInfo(Location, Rotation);
	}
}
#endif //WITH_EDITOR
