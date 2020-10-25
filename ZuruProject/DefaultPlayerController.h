#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "DefaultPlayerController.generated.h"


UENUM()
enum class ECameraMode : uint8
{
	EditingMode,
	FreeMode
};

class AEditingSpectatorPawn;
class AFreeSpectatorPawn;

/**
 * The default player controller for this project
 */
UCLASS()
class ZURUPROJECT_API ADefaultPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

public:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Utility")
	virtual void CreateSpectators();

	UFUNCTION(BlueprintCallable, Category = "Utility")
	virtual AEditingSpectatorPawn* GetEditingSpectator() const;
	UFUNCTION(BlueprintCallable, Category = "Utility")
	virtual AFreeSpectatorPawn* GetFreeSpectator() const;

	UFUNCTION(BlueprintCallable, Category = "Utility")
	virtual void ShowHelpMessage(const bool InbShow);
		
protected:

	virtual void SetupInputComponent() override;

	void Bind(const bool bHasFixedSpectator);
	void Unbind();
	void SwitchToEditingSpectator();
	void SwitchToFreeSpectator();

	void EditingMoveForward(const float Value);
	void EditingMoveRight(const float Value);

	void SpectatorMoveForward(const float Value);
	void SpectatorMoveRight(const float Value);
	void SpectatorRotateX(const float Value);
	void SpectatorRotateY(const float Value);

	void ToggleSpawningComponent();
	void SpawnMesh();

	void ShowEditingModeHelpMessage();
	void ShowFreeModeHelpMessage();

	FText GetInputMappingKeyDisplayName(const FName& InputMappingName) const;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Cameras")
	FVector EditingSpectatorStartingLocation;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Cameras")
	FRotator EditingSpectatorStartingRotation;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Cameras")
	FVector FreeSpectatorStartingLocation;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Cameras")
	FRotator FreeSpectatorStartingRotation;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Cameras")
	bool bStartAsEditingCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Classes")
	TSubclassOf<ASpectatorPawn> EditingSpectatorClass;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Classes")
	TSubclassOf<ASpectatorPawn> FreeSpectatorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Input Actions")
	FName EditingCameraInputActionName = FName("EditingCamera");
	UPROPERTY(EditDefaultsOnly, Category = "Config|Input Actions")
	FName FreeCameraInputActionName = FName("FreeCamera");
	UPROPERTY(EditDefaultsOnly, Category = "Config|Input Actions")
	FName ToggleSpawningComponentInputActionName = FName("ToggleSpawningComponent");
	UPROPERTY(EditDefaultsOnly, Category = "Config|Input Actions")
	FName SpawnMeshInputActionName = FName("SpawnMesh");
	UPROPERTY(EditDefaultsOnly, Category = "Config|Axis Mapping")
	FName MoveForwardAxisMappingName = FName("MoveForward");
	UPROPERTY(EditDefaultsOnly, Category = "Config|Axis Mapping")
	FName MoveRightAxisMappingName = FName("MoveRight");
	UPROPERTY(EditDefaultsOnly, Category = "Config|Axis Mapping")
	FName RotateXAxisMappingName = FName("RotateX");
	UPROPERTY(EditDefaultsOnly, Category = "Config|Axis Mapping")
	FName RotateYAxisMappingName = FName("RotateY");

private:

	UPROPERTY()
	AEditingSpectatorPawn* EditingSpectator = nullptr;
	UPROPERTY()
	AFreeSpectatorPawn* FreeSpectator = nullptr;

	ECameraMode Mode;
	bool bIsHelpMessageShown;

};
