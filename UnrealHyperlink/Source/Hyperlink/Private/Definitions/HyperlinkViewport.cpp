// Copyright (c) 2023 Fergus Brown. Licensed under the MIT license. See "LICENSE" file for details.


#include "Definitions/HyperlinkViewport.h"

#include "GameFramework/PlayerController.h"
#include "HyperlinkUtility.h"
#include "JsonObjectConverter.h"
#include "LevelEditor.h"
#include "Log.h"
#if WITH_EDITOR
#include "Subsystems/UnrealEditorSubsystem.h"

#define LOCTEXT_NAMESPACE "HyperlinkViewport"

FHyperlinkViewportCommands::FHyperlinkViewportCommands()
	: TCommands<FHyperlinkViewportCommands>(
		TEXT("HyperlinkViewport"),
		NSLOCTEXT("Contexts", "HyperlinkViewport", "Hyperlink Viewport"),
		NAME_None,
		FAppStyle::GetAppStyleSetName())
{
}

void FHyperlinkViewportCommands::RegisterCommands()
{
	UI_COMMAND(CopyViewportLink, "Copy Viewport Link", "Copy a link to go to the current viewport position in the level editor",
	           EUserInterfaceActionType::Button, FInputChord(EModifierKey::Alt | EModifierKey::Shift, EKeys::X));
}

#undef LOCTEXT_NAMESPACE

#endif //WITH_EDITOR

void UHyperlinkViewport::Initialize()
{
#if WITH_EDITOR
	// Check for editor in case launching game with editor build
	if (GIsEditor)
	{
		FHyperlinkViewportCommands::Register();
		ViewportCommands = MakeShared<FUICommandList>();
		ViewportCommands->MapAction(
			FHyperlinkViewportCommands::Get().CopyViewportLink,
			FExecuteAction::CreateUObject(this, &UHyperlinkDefinition::CopyLink));

		const FLevelEditorModule& LevelEditor{ FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor")) };
		LevelEditor.GetGlobalLevelEditorActions()->Append(ViewportCommands.ToSharedRef());

		UHyperlinkUtility::ExtendToolMenuSection(TEXT("LevelEditor.ActorContextMenu"),
			TEXT("ActorOptions"), ViewportCommands, FHyperlinkViewportCommands::Get().CopyViewportLink);
		UHyperlinkUtility::ExtendToolMenuSection(TEXT("LevelEditor.LevelViewportToolBar.Options"),
			TEXT("LevelViewportViewportOptions"), ViewportCommands,
			FHyperlinkViewportCommands::Get().CopyViewportLink);
	}
#endif //WITH_EDITOR
}

void UHyperlinkViewport::Deinitialize()
{
#if WITH_EDITOR
	// Note that we don't need to unmap the actions from Level Editor Module's GlobalLevelEditorActions
	FHyperlinkViewportCommands::Unregister();
#endif //WITH_EDITOR
}

TSharedPtr<FJsonObject> UHyperlinkViewport::GeneratePayload() const
{
	TSharedPtr<FJsonObject> Payload{ nullptr };
	
	FName LevelPackageName{};
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
				LevelPackageName = PieWorld->PersistentLevel->GetPackage()->GetFName();
				bCameraInfoFound |= GetGameWorldCameraInfo(PieWorld, Location, Rotation);
			}
		}
		else
		{
			UUnrealEditorSubsystem* const UnrealEditorSubsystem{ GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>() };
			LevelPackageName = UnrealEditorSubsystem->GetEditorWorld()->PersistentLevel->GetPackage()->GetFName();
			bCameraInfoFound = UnrealEditorSubsystem->GetLevelViewportCameraInfo(Location, Rotation);
		}
	}
	else
#endif //WITH_EDITOR
	if (GetWorld())
	{
		LevelPackageName = GetWorld()->PersistentLevel->GetPackage()->GetFName();
		bCameraInfoFound = GetGameWorldCameraInfo(GetWorld(), Location, Rotation);
	}
	
	if (bCameraInfoFound)
	{
		Payload = GeneratePayload(LevelPackageName, Location, Rotation);
	}
	else
	{
		UE_LOG(LogHyperlink, Display, TEXT("Failed to generate Viewport link: could not find viewport camera info."));

	}
	
	return Payload;
}

TSharedPtr<FJsonObject> UHyperlinkViewport::GeneratePayload(const FName& InLevelPackageName,
	const FVector& InLocation, const FRotator& InRotation) const
{
	const FHyperlinkViewportPayload PayloadStruct
	{
		InLevelPackageName,
		InLocation,
		InRotation
	};
	return FJsonObjectConverter::UStructToJsonObject(PayloadStruct);
}

/*static*/bool UHyperlinkViewport::GetGameWorldCameraInfo(const UWorld* const World, FVector& OutLocation, FRotator& OutRotation)
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
void UHyperlinkViewport::ExecutePayload(const TSharedRef<FJsonObject>& InPayload)
{
	FHyperlinkViewportPayload PayloadStruct{};

	if (FJsonObjectConverter::JsonObjectToUStruct(InPayload, &PayloadStruct))
	{
		const FName& LevelPackageName{ PayloadStruct.LevelPackageName };
		const FVector& Location{ PayloadStruct.Location };
		const FRotator& Rotation{ PayloadStruct.Rotation };
	
		// Attempt to teleport pawn in PIE
		if (const FWorldContext* const PieWorldContext{ GEditor->GetPIEWorldContext() })
		{
			if (const UWorld* const PieWorld{ PieWorldContext->World() })
			{
				const FName EditorWorldPackageName{ GEditor->EditorWorld->PersistentLevel->GetPackage()->GetFName() };
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
		if(UHyperlinkUtility::OpenEditorForAsset(LevelPackageName))
		{
			UUnrealEditorSubsystem* const UnrealEditorSubsystem{ GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>() };
			UnrealEditorSubsystem->SetLevelViewportCameraInfo(Location, Rotation);
		}
	}
}
#endif //WITH_EDITOR
