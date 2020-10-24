#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "DefaultPlayerController.generated.h"


class AFixedEditingCamera;
class AFreeSpectatorPawn;

/**
 * The default player controller for this project
 */
UCLASS()
class ZURUPROJECT_API ADefaultPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ADefaultPlayerController(const class FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	bool bStartAsEditingCamera = false;

	UPROPERTY()
	AFixedEditingCamera* EditingCamera = nullptr;
	UPROPERTY()
	AFreeSpectatorPawn* FreeSpectator = nullptr;
		
protected:

	virtual void SetupInputComponent() override;
	void Bind(const bool bHasFixedSpectator);
	void Unbind();
	void SwitchToEditingCamera();
	void SwitchToFreeSpectator();
	void MoveForward(const float Value);
	void MoveRight(const float Value);
	void RotateX(const float Value);
	void RotateY(const float Value);

};
