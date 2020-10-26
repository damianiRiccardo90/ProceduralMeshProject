#include "EditingSpectatorPawn.h"


AEditingSpectatorPawn::AEditingSpectatorPawn(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAddDefaultMovementBindings = false;
}

void AEditingSpectatorPawn::MoveForward(float Val)
{
	Super::MoveUp_World(Val);
}
