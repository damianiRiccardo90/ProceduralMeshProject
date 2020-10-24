#pragma once

#include <GameFramework/Actor.h>

#include "ProcMeshTestDiCazzoActor.generated.h"


class AProcMeshCompositeTable;
class USceneComponent;

/**
 * A procedural dio merda
 */
UCLASS(Blueprintable)
class ZURUPROJECT_API AProcMeshTestDiCazzoActor : public AActor
{
	GENERATED_UCLASS_BODY()

public:

	virtual void BeginPlay() override;

private:

	UPROPERTY()
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY()
	AProcMeshCompositeTable* Mesh = nullptr;

	bool bHasSpawned = false;

};
