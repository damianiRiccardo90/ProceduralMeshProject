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
class UPrintToScreenWidget;
class UProcMeshSpawningComponent;

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
	void ShowEditingModeHelpMessage();
	void ShowFreeModeHelpMessage();

	UFUNCTION()
	void SwitchToEditingSpectator();
	UFUNCTION()
	void SwitchToFreeSpectator();
	UFUNCTION()
	void ToggleHelpMessage();
	UFUNCTION()
	void ToggleSpawningComponent();
	UFUNCTION()
	void SpawnMesh();
	UFUNCTION()
	void EditingMoveForward(const float Value);
	UFUNCTION()
	void EditingMoveRight(const float Value);
	UFUNCTION()
	void SpectatorMoveForward(const float Value);
	UFUNCTION()
	void SpectatorMoveRight(const float Value);
	UFUNCTION()
	void SpectatorRotateX(const float Value);
	UFUNCTION()
	void SpectatorRotateY(const float Value);

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
	UPROPERTY(EditDefaultsOnly, Category = "Config|Classes")
	TSubclassOf<UPrintToScreenWidget> PrintToScreenWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Classes")
	TSubclassOf<UProcMeshSpawningComponent> SpawningComponentClass;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Input Actions")
	FName EditingCameraInputActionName;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Input Actions")
	FName FreeCameraInputActionName;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Input Actions")
	FName ToggleSpawningComponentInputActionName;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Input Actions")
	FName SpawnMeshInputActionName;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Input Actions")
	FName ToggleUIInputActionName;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Axis Mapping")
	FName MoveForwardAxisMappingName;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Axis Mapping")
	FName MoveRightAxisMappingName;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Axis Mapping")
	FName RotateXAxisMappingName;
	UPROPERTY(EditDefaultsOnly, Category = "Config|Axis Mapping")
	FName RotateYAxisMappingName;

private:

	UPROPERTY()
	AEditingSpectatorPawn* EditingSpectator = nullptr;
	UPROPERTY()
	AFreeSpectatorPawn* FreeSpectator = nullptr;
	UPROPERTY()
	UPrintToScreenWidget* PrintWidget = nullptr;

	ECameraMode Mode;
	bool bIsHelpMessageShown;

};
