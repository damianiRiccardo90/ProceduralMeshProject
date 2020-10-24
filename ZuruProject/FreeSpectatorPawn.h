#pragma once

#include "CoreMinimal.h"
#include <GameFramework/SpectatorPawn.h>

#include "FreeSpectatorPawn.generated.h"


/**
 * The free camera spectator pawn
 */
UCLASS()
class ZURUPROJECT_API AFreeSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

public:

	AFreeSpectatorPawn(const class FObjectInitializer& ObjectInitializer);

};
