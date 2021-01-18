#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "ProceduralMeshProjectGameModeBase.generated.h"


class AProcMeshBase;

UCLASS()
class PROCEDURALMESHPROJECT_API AProceduralMeshProjectGameModeBase : public AGameModeBase
{
	GENERATED_UCLASS_BODY()

public:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Actor Positions")
	FVector SpawnOrigin = FVector(0.f, 0.f, 0.f);
	UPROPERTY(EditDefaultsOnly, Category = "Config|Actor Positions")
	FVector StartingMeshSpawnLocation = FVector(0.f, 0.f, 0.f);
	UPROPERTY(EditDefaultsOnly, Category = "Config|Actor Positions")
	FRotator StartingMeshSpawnRotation = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(EditDefaultsOnly, Category = "Config|Actor Classes")
	TSubclassOf<AProcMeshBase> StartingMeshClass;
	
};
