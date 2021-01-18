#pragma once

#include "CoreMinimal.h"
#include <GameFramework/SpectatorPawn.h>

#include "EditingSpectatorPawn.generated.h"


/**
 * The top-down camera spectator pawn
 */
UCLASS()
class PROCEDURALMESHPROJECT_API AEditingSpectatorPawn : public ASpectatorPawn
{
	GENERATED_UCLASS_BODY()

public:

	virtual void MoveForward(float Val);

};
