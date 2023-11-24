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
	UI_COMMAND(CopyGoToLink, "Copy GoTo Link", "Copy a link to go to the specified location in a level",
	           EUserInterfaceActionType::Button, FInputChord(EModifierKey::Alt | EModifierKey::Shift, EKeys::X));
}

#undef LOCTEXT_NAMESPACE

#endif //WITH_EDITOR

UHyperlinkLevelGoTo::UHyperlinkLevelGoTo()
{
	DefinitionIdentifier = TEXT("LevelGoTo");

	BodyPattern = FString::Printf(TEXT("(.+)%s([0-9A-F]{%d})"), &FHyperlinkFormat::ArgSeparator, FHyperlinkUtils::VectorStringLength * 2);
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
	
#if WITH_EDITOR
	// GEditor guard required as WITH_EDITOR code still runs in uncooked game
	if (GEditor)
	{
		if (const FWorldContext* const PieWorldContext{ GEditor->GetPIEWorldContext() })
		{
			if (const UWorld* const PieWorld{ PieWorldContext->World() })
			{
				LevelPackageName = PieWorld->PersistentLevel->GetPackage()->GetName();
				bCameraInfoFound |= GetGameWorldCameraInfo(PieWorld, Location, Rotation);
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
	return GetHyperlinkBase() / InLevelPackageName + FHyperlinkFormat::ArgSeparator +
		FHyperlinkUtils::VectorToHexString(InLocation) + FHyperlinkUtils::VectorToHexString(InRotation.Vector());
}

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
	// Extract link info
	const FString& LevelPackageName{ LinkArguments[1] };
	
	const FString& VectorStrings{ LinkArguments[2] };
	const FString LocationString{ VectorStrings.Mid(0, FHyperlinkUtils::VectorStringLength) };
	const FString RotationString{ VectorStrings.Mid(FHyperlinkUtils::VectorStringLength, FHyperlinkUtils::VectorStringLength) };

	const FVector Location{ FHyperlinkUtils::HexStringToVector(LocationString) };
	const FRotator Rotation{ FHyperlinkUtils::HexStringToVector(RotationString).Rotation() };
	
	// Attempt to teleport pawn in PIE
	if (const FWorldContext* const PieWorldContext{ GEditor->GetPIEWorldContext() })
	{
		if (const UWorld* const PieWorld{ PieWorldContext->World() })
		{
			const FString EditorWorldPackageName{ GEditor->EditorWorld->PersistentLevel->GetPackage()->GetName() };
			if (EditorWorldPackageName == LevelPackageName)
			{
				if (APawn* const Pawn{ PieWorld->GetFirstPlayerController()->GetPawn() })
				{
					Pawn->TeleportTo(Location, Rotation);
					return;
				}
			}
		}
	}
	
	// If PIE teleport fails open the level and move viewport to location
	if(FHyperlinkUtils::OpenEditorForAsset(LevelPackageName))
	{
			UUnrealEditorSubsystem* const UnrealEditorSubsystem{ GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>() };
			UnrealEditorSubsystem->SetLevelViewportCameraInfo(Location, Rotation);
	}
}
#endif //WITH_EDITOR
