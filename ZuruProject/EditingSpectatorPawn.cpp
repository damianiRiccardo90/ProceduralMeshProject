#include "EditingSpectatorPawn.h"


AEditingSpectatorPawn::AEditingSpectatorPawn(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAddDefaultMovementBindings = false;
}

void AEditingSpectatorPawn::MoveForward(float Val)
{
	AddMovementInput(FVector(0.f, -1.f, 0.f), Val);
}
