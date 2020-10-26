#include "DefaultPlayerController.h"
#include "EditingSpectatorPawn.h"
#include "FreeSpectatorPawn.h"
#include "ProcMeshSpawningComponent.h"
#include <GameFramework/InputSettings.h>
#include "PrintToScreenWidget.h"


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

	EditingCameraInputActionName = FName("EditingCamera");
	FreeCameraInputActionName = FName("FreeCamera");
	ToggleSpawningComponentInputActionName = FName("ToggleSpawningComponent");
	SpawnMeshInputActionName = FName("SpawnMesh");
	ToggleUIInputActionName = FName("ToggleUI");
	MoveForwardAxisMappingName = FName("MoveForward");
	MoveRightAxisMappingName = FName("MoveRight");
	RotateXAxisMappingName = FName("RotateX");
	RotateYAxisMappingName = FName("RotateY");

	bIsHelpMessageShown = false;
}

void ADefaultPlayerController::BeginPlay()
{
	CreateSpectators();

	SpawningComponent = NewObject<UProcMeshSpawningComponent>(this, ProcMeshSpawningComponentClass);
	if (SpawningComponent)
	{
		SpawningComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}

	PrintWidget = CreateWidget<UPrintToScreenWidget>(this, PrintToScreenWidgetClass);
	if (PrintWidget)
	{
		PrintWidget->AddToViewport();
		PrintWidget->SetVisibility(ESlateVisibility::Hidden);
	}

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

void ADefaultPlayerController::ShowHelpMessage(const bool InbShow)
{
	if (bIsHelpMessageShown)
	{
		if (PrintWidget)
		{
			PrintWidget->SetVisibility(ESlateVisibility::Hidden);
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
		InputComponent->BindAction(TEXT("ToggleUI"), IE_Pressed, this, &ThisClass::ToggleHelpMessage);

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

void ADefaultPlayerController::ShowEditingModeHelpMessage()
{
	if (PrintWidget)
	{
		TArray<FText> MessageLines;

		MessageLines.Add(
			FText::FromString("EditingMode")
		);
		MessageLines.Add(
			FText::Format(
				FText::FromString("{0} => {1}"),
				FText::FromName(EditingCameraInputActionName), GetInputMappingKeyDisplayName(EditingCameraInputActionName)
			)
		);
		MessageLines.Add(
			FText::Format(
				FText::FromString("{0} => {1}"),
				FText::FromName(FreeCameraInputActionName), GetInputMappingKeyDisplayName(FreeCameraInputActionName)
			)
		);
		MessageLines.Add(
			FText::Format(
				FText::FromString("CameraMovements => {0}"),
				GetInputMappingKeyDisplayName(EditingCameraInputActionName)
			)
		);
		MessageLines.Add(
			FText::Format(
				FText::FromString("{0} => {1}"),
				FText::FromName(ToggleSpawningComponentInputActionName), GetInputMappingKeyDisplayName(ToggleSpawningComponentInputActionName)
			)
		);
		MessageLines.Add(
			FText::Format(
				FText::FromString("{0} => {1}"),
				FText::FromName(SpawnMeshInputActionName), GetInputMappingKeyDisplayName(SpawnMeshInputActionName)
			)
		);
		MessageLines.Add(
			FText::Format(
				FText::FromString("{0} => {1}"),
				FText::FromName(ToggleUIInputActionName), GetInputMappingKeyDisplayName(ToggleUIInputActionName)
			)
		);

		PrintWidget->PrintToScreen(MessageLines);

		PrintWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void ADefaultPlayerController::ShowFreeModeHelpMessage()
{
	if (PrintWidget)
	{
		TArray<FText> MessageLines;

		MessageLines.Add(
			FText::FromString("FreeCameraMode")
		);
		MessageLines.Add(
			FText::Format(
				FText::FromString("{0} => {1}"),
				FText::FromName(EditingCameraInputActionName), GetInputMappingKeyDisplayName(EditingCameraInputActionName)
			)
		);
		MessageLines.Add(
			FText::Format(
				FText::FromString("{0} => {1}"),
				FText::FromName(FreeCameraInputActionName), GetInputMappingKeyDisplayName(FreeCameraInputActionName)
			)
		);
		MessageLines.Add(
			FText::Format(
				FText::FromString("CameraMovements => {0}"),
				GetInputMappingKeyDisplayName(EditingCameraInputActionName)
			)
		);
		MessageLines.Add(
			FText::Format(
				FText::FromString("{0} => {1}"),
				FText::FromName(ToggleUIInputActionName), GetInputMappingKeyDisplayName(ToggleUIInputActionName)
			)
		);

		PrintWidget->PrintToScreen(MessageLines);

		PrintWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
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
	if (SpawningComponent)
	{
		SpawningComponent->ActivateComponent(!SpawningComponent->IsComponentActive());
	}
}

void ADefaultPlayerController::SpawnMesh()
{
	if (SpawningComponent)
	{
		SpawningComponent->SpawnMesh();
	}
}

void ADefaultPlayerController::ToggleHelpMessage()
{
	ShowHelpMessage(!bIsHelpMessageShown);
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
				KeysDisplayNameStr += " | ";
			}
		}
	}

	return FText::FromString(KeysDisplayNameStr);
}
