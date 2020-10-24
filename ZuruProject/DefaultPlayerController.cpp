#include "DefaultPlayerController.h"
#include "FixedEditingCamera.h"
#include "FreeSpectatorPawn.h"

ADefaultPlayerController::ADefaultPlayerController(const class FObjectInitializer& ObjectInitializer)
{
	bAutoManageActiveCameraTarget = false;
}

void ADefaultPlayerController::BeginPlay()
{
	bStartAsEditingCamera ? SwitchToEditingCamera() : SwitchToFreeSpectator();
}

void ADefaultPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void ADefaultPlayerController::Bind(const bool bHasFixedSpectator)
{
	if (InputComponent)
	{
		InputComponent->BindAction(TEXT("FixedCamera"), IE_Pressed, this, &ThisClass::SwitchToEditingCamera);
		InputComponent->BindAction(TEXT("FreeCamera"), IE_Pressed, this, &ThisClass::SwitchToFreeSpectator);

		if (!bHasFixedSpectator)
		{
			InputComponent->BindAxis(TEXT("MoveForward"), this, &ThisClass::MoveForward);
			InputComponent->BindAxis(TEXT("MoveRight"), this, &ThisClass::MoveRight);
			InputComponent->BindAxis(TEXT("RotateX"), this, &ThisClass::RotateX);
			InputComponent->BindAxis(TEXT("RotateY"), this, &ThisClass::RotateY);
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

void ADefaultPlayerController::SwitchToEditingCamera()
{
	Unbind();
	Bind(true);

	if (EditingCamera)
	{
		SetViewTargetWithBlend(EditingCamera);
	}

	if (AFreeSpectatorPawn* ControlledPawn = GetPawn<AFreeSpectatorPawn>())
	{
		UnPossess();
	}

	bShowMouseCursor = true;
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;
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
}

void ADefaultPlayerController::MoveForward(const float Value)
{
	if (FreeSpectator)
	{
		FreeSpectator->MoveForward(Value);
	}
}

void ADefaultPlayerController::MoveRight(const float Value)
{
	if (FreeSpectator)
	{
		FreeSpectator->MoveRight(Value);
	}
}

void ADefaultPlayerController::RotateX(const float Value)
{
	if (FreeSpectator)
	{
		FreeSpectator->TurnAtRate(Value);
	}
}

void ADefaultPlayerController::RotateY(const float Value)
{
	if (FreeSpectator)
	{
		FreeSpectator->LookUpAtRate(Value);
	}
}
