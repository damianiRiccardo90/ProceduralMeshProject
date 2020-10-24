#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "ZuruProjectGameModeBase.generated.h"


class AProcMeshCompositeTable;

UCLASS()
class ZURUPROJECT_API AZuruProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	AZuruProjectGameModeBase(const class FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Actor Positions")
	FVector SpawnOrigin = FVector(0.f, 0.f, 0.f);
	UPROPERTY(EditDefaultsOnly, Category = "Config|Actor Positions")
	FVector FixedCameraSpawnLocation = FVector(0.f, 0.f, 800.f);
	UPROPERTY(EditDefaultsOnly, Category = "Config|Actor Positions")
	FRotator FixedCameraSpawnRotation = FRotator(-90.f, -90.f, 0.f);
	UPROPERTY(EditDefaultsOnly, Category = "Config|Actor Positions")
	FVector FreeSpectatorSpawnLocation = FVector(-400.f, 0.f, 50.f);
	UPROPERTY(EditDefaultsOnly, Category = "Config|Actor Positions")
	FRotator FreeSpectatorSpawnRotation = FRotator(0.f, 0.f, 0.f);
	UPROPERTY(EditDefaultsOnly, Category = "Config|Actor Positions")
	FVector TestMeshSpawnLocation = FVector(0.f, 0.f, 0.f);
	UPROPERTY(EditDefaultsOnly, Category = "Config|Actor Positions")
	FRotator TestMeshSpawnRotation = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(EditDefaultsOnly, Category = "Config|Actor Classes")
	TSubclassOf<AProcMeshCompositeTable> TableClass;
	
};
