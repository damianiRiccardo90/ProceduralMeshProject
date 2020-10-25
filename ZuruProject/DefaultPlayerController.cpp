#include "DefaultPlayerController.h"
#include "EditingSpectatorPawn.h"
#include "FreeSpectatorPawn.h"
#include "ProcMeshSpawningComponent.h"
#include <GameFramework/InputSettings.h>


ADefaultPlayerController::ADefaultPlayerController(const class FObjectInitializer& ObjectInitializer)
{
	bAutoManageActiveCameraTarget = false;

	EditingSpectatorStartingLocation = FVector(0.f, 0.f, 800.f);
	EditingSpectatorStartingRotation  = FRotator(-90.f, -90.f, 0.f);
	FreeSpectatorStartingLocation = FVector(-400.f, 0.f, 50.f);
	FreeSpectatorStartingRotation = FRotator(0.f, 0.f, 0.f);
	bStartAsEditingCamera = false;

	EditingSpectatorClass = AEditingSpectatorPawn::StaticClass();
	FreeSpectatorClass = AFreeSpectatorPawn::StaticClass();

	bIsHelpMessageShown = false;
}

void ADefaultPlayerController::BeginPlay()
{
	CreateSpectators();

	bStartAsEditingCamera ? SwitchToEditingSpectator() : SwitchToFreeSpectator();
}

void ADefaultPlayerController::CreateSpectators()
{
	if (UWorld* World = GetWorld())
	{
		EditingSpectator = World->SpawnActor<AEditingSpectatorPawn>(
			EditingSpectatorClass,
			EditingSpectatorStartingLocation,
			EditingSpectatorStartingRotation,
			FActorSpawnParameters()
		);

		FreeSpectator = World->SpawnActor<AFreeSpectatorPawn>(
			FreeSpectatorClass,
			FreeSpectatorStartingLocation,
			FreeSpectatorStartingRotation,
			FActorSpawnParameters()
		);
	}
}

AEditingSpectatorPawn* ADefaultPlayerController::GetEditingSpectator() const
{
	return EditingSpectator;
}

AFreeSpectatorPawn* ADefaultPlayerController::GetFreeSpectator() const
{
	return FreeSpectator;
}

void ADefaultPlayerController::ToggleHelpMessage()
{
	if (bIsHelpMessageShown)
	{
		if (GEngine)
		{
			GEngine->ClearOnScreenDebugMessages();
		}
	}
	else
	{
		if (Mode == ECameraMode::EditingMode)
		{
			ShowEditingModeHelpMessage();
		}
		else if (Mode == ECameraMode::FreeMode)
		{
			ShowFreeModeHelpMessage();
		}
	}

	bIsHelpMessageShown = !bIsHelpMessageShown;
}

void ADefaultPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void ADefaultPlayerController::Bind(const bool bHasFixedSpectator)
{
	if (InputComponent)
	{
		InputComponent->BindAction(TEXT("EditingCamera"), IE_Pressed, this, &ThisClass::SwitchToEditingSpectator);
		InputComponent->BindAction(TEXT("FreeCamera"), IE_Pressed, this, &ThisClass::SwitchToFreeSpectator);

		if (bHasFixedSpectator)
		{
			InputComponent->BindAction(TEXT("ToggleSpawningComponent"), IE_Pressed, this, &ThisClass::ToggleSpawningComponent);
			InputComponent->BindAction(TEXT("SpawnMesh"), IE_Pressed, this, &ThisClass::SpawnMesh);
			InputComponent->BindAxis(TEXT("MoveForward"), this, &ThisClass::EditingMoveForward);
			InputComponent->BindAxis(TEXT("MoveRight"), this, &ThisClass::EditingMoveRight);
		}
		else
		{
			InputComponent->BindAxis(TEXT("MoveForward"), this, &ThisClass::SpectatorMoveForward);
			InputComponent->BindAxis(TEXT("MoveRight"), this, &ThisClass::SpectatorMoveRight);
			InputComponent->BindAxis(TEXT("RotateX"), this, &ThisClass::SpectatorRotateX);
			InputComponent->BindAxis(TEXT("RotateY"), this, &ThisClass::SpectatorRotateY);
		}
	}
}

void ADefaultPlayerController::Unbind()
{
	if (InputComponent)
	{
		InputComponent->AxisBindings.Empty();
		InputComponent->ClearActionBindings();
	}
}

void ADefaultPlayerController::SwitchToEditingSpectator()
{
	Unbind();
	Bind(true);

	if (EditingSpectator)
	{
		Possess(EditingSpectator);
		SetViewTargetWithBlend(EditingSpectator);
	}

	bShowMouseCursor = true;
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;

	Mode = ECameraMode::EditingMode;
}

void ADefaultPlayerController::SwitchToFreeSpectator()
{
	Unbind();
	Bind(false);

	if (FreeSpectator)
	{
		Possess(FreeSpectator);
		SetViewTargetWithBlend(FreeSpectator);
	}

	bShowMouseCursor = false;
	bEnableMouseOverEvents = false;
	bEnableClickEvents = false;

	Mode = ECameraMode::FreeMode;
}

void ADefaultPlayerController::EditingMoveForward(const float Value)
{
	if (EditingSpectator)
	{
		EditingSpectator->MoveForward(Value);
	}
}

void ADefaultPlayerController::EditingMoveRight(const float Value)
{
	if (EditingSpectator)
	{
		EditingSpectator->MoveRight(Value);
	}
}

void ADefaultPlayerController::SpectatorMoveForward(const float Value)
{
	if (FreeSpectator)
	{
		FreeSpectator->MoveForward(Value);
	}
}

void ADefaultPlayerController::SpectatorMoveRight(const float Value)
{
	if (FreeSpectator)
	{
		FreeSpectator->MoveRight(Value);
	}
}

void ADefaultPlayerController::SpectatorRotateX(const float Value)
{
	if (FreeSpectator)
	{
		FreeSpectator->TurnAtRate(Value);
	}
}

void ADefaultPlayerController::SpectatorRotateY(const float Value)
{
	if (FreeSpectator)
	{
		FreeSpectator->LookUpAtRate(Value);
	}
}

void ADefaultPlayerController::ToggleSpawningComponent()
{
	if (UProcMeshSpawningComponent* SpawningComponent = FindComponentByClass<UProcMeshSpawningComponent>())
	{
		SpawningComponent->ActivateComponent(!SpawningComponent->IsComponentActive());
	}
}

void ADefaultPlayerController::SpawnMesh()
{
	if (UProcMeshSpawningComponent* SpawningComponent = FindComponentByClass<UProcMeshSpawningComponent>())
	{
		SpawningComponent->SpawnMesh();
	}
}

void ADefaultPlayerController::ShowEditingModeHelpMessage()
{
	if (GEngine)
	{
		FText EditCamera_KeyDisplayName;
		FText FreeCamera_KeyDisplayName;
		FText CameraMovement_KeyDisplayName;
		FString MoveRightKeyName;
		FString EditingModeKeyName;
		FString FreeModeKeyName;
		FString ToggleSpawningModeKeyName;
		FString SpawnMeshKeyName;
		if (UInputSettings* InputSettings = UInputSettings::GetInputSettings())
		{
			TArray<FInputActionKeyMapping> InputActionMappings;
			InputSettings->GetActionMappingByName(EditingCameraInputActionName, InputActionMappings);
			for (int32 i = 0; i < InputActionMappings.Num(); i++)
			{
				InputActionMappings[i].Key.GetDisplayName()
			}
		}
		FString ModeStr = "EditingMode";
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, ModeStr);


		FString FormattedString = FText::Format(FText::FromString("The O'Bow Lya ORIGIN IS --> Xo: {0} Yo: {1} Zo: {2}!"),
			FText::AsNumber(TestMeshActor->GetActorLocation().X), FText::AsNumber(TestMeshActor->GetActorLocation().Y), FText::AsNumber(TestMeshActor->GetActorLocation().Z)).ToString();
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, TEXT("The Oak Anne IS MARONNO!"));
	}
}

void ADefaultPlayerController::ShowFreeModeHelpMessage()
{
	if (GEngine)
	{
		FString ModeStr = "FreeCameraMode";
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, ModeStr);


		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, TEXT("The Oak Anne IS BOYADIO!"));
	}
}

FText ADefaultPlayerController::GetInputMappingKeyDisplayName(const FName& InputMappingName) const
{
	FString KeysDisplayNameStr = "";

	if (UInputSettings* InputSettings = UInputSettings::GetInputSettings())
	{
		TArray<FInputActionKeyMapping> InputActionMappings;
		InputSettings->GetActionMappingByName(EditingCameraInputActionName, InputActionMappings);
		for (int32 i = 0; i < InputActionMappings.Num(); i++)
		{
			KeysDisplayNameStr += InputActionMappings[i].Key.GetDisplayName().ToString();
			if (InputActionMappings.Num() > 1 && i < InputActionMappings.Num() - 1)
			{
				KeysDisplayNameStr += ", ";
			}
		}
	}

	return FText::FromString(KeysDisplayNameStr);
}
